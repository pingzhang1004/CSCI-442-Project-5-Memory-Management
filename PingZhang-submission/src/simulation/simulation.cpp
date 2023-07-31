/**
 * This file contains implementations for the methods defined in the Simulation
 * class.
 *
 * You'll probably spend a lot of your time here.
 */

#include "simulation/simulation.h"
#include <stdexcept>

Simulation::Simulation(FlagOptions& flags)
{
    this->flags = flags;
    this->frames.reserve(this->NUM_FRAMES);
}

void Simulation::run() {
    // TODO: implement me

    //initialize the null frames and set all the frame free
    for (int i = 0; i < NUM_FRAMES; i++) {
        this->frames.push_back(Frame());
        this->free_frames.push_back(i);
    }
    bool error = false;
    for (auto & virtual_addre : this->virtual_addresses) {  
        if (perform_memory_access(virtual_addre) == 1) {
            error = true;
            break;
        }
    }
    if(error == false){
        this->print_summary();
    }

    for (auto entry: this->processes) 
    {
        for (auto page :  entry.second->pages) {
            delete(page);
    }
        delete(entry.second);
   } 
}


/**
    * Performs a memory access for the given virtual address, translating it to
    * a physical address and loading the page into memory if needed. Returns the
    * byte at the given address.
    */
char Simulation::perform_memory_access(const VirtualAddress& virtual_address) {
    // TODO: implement me
    Process *process =  this->processes[virtual_address.process_id];
    int page_Number = virtual_address.page;
    //page_valid true if this process contains a page with the provided index.
    bool page_valid = process->is_valid_page(page_Number);
    //True if the page is present in main memory, or false otherwise.
    bool page_present = process->page_table.rows[page_Number].present;
 
    //Whether information needs to be output for each memory access.
    if (this->flags.verbose == true) {
        std::cout << virtual_address << std::endl;
    }

    //'virtual time' at which this page was last accessed. 
    process->page_table.rows[page_Number].last_accessed_at = this->access_time++;
    
    //The total number of memory accesses this process performed.
    process->memory_accesses++;

    if (page_valid == false) {
        std::cout << "SEGFAULT - INVALID PAGE" << std::endl;
        return 1;
    }
    if (page_present == false) {  
        std::cout << "    -> PAGE FAULT" << std::endl;
        handle_page_fault(process, page_Number);
    } 
    else {
        std::cout << "    -> IN MEMORY" << std::endl;
    }
    
    // translate virtual address into physical address
    //The frame containing the corresponding page, if present in memory.
    int frame_Number = process->page_table.rows[page_Number].frame;
    int offset = virtual_address.offset;
    
    //Print the physical address: frame + offset (binary format)
    std::cout << "    -> physical address " << std::bitset<10>(frame_Number) << std::bitset<6>(offset)
                  << " [frame: " << frame_Number << "; offset: " << offset << "]\n";
    if (process->pages[page_Number]->is_valid_offset(offset) == false) {
        std::cout << "SEGFAULT - INVALID OFFSET\n";
            return 1;
    }
    //print the resident set size of this process. 
    std::cout << "    -> RSS: " << process->get_rss() << "\n\n";
    return 0;
}

void Simulation::handle_page_fault(Process* process, size_t page) {
    //TODO: implement me
    if (process->get_rss() <  this->flags.max_frames) {
        frames[free_frames.front()].set_page(process, page);
        process->page_table.rows[page].frame = free_frames.front();
        free_frames.pop_front();
    } else {
        //这种情况是没有free——frame了，所以要swapped out
        int replaced_page_number = 0;
        if(this->flags.strategy == ReplacementStrategy::FIFO){
            replaced_page_number = process->page_table.get_oldest_page();
        }
        else{
            replaced_page_number = process->page_table.get_least_recently_used_page();
        }
        //make replaced page.present be false
        process->page_table.rows[replaced_page_number].present = false;
        process->page_table.rows[replaced_page_number].loaded_at = -1;
        process->page_table.rows[page].frame = process->page_table.rows[replaced_page_number].frame;

    }
    process->page_table.rows[page].present = true;
    process->page_table.rows[page].loaded_at = process->memory_accesses;
    process->page_faults++; 
    this->page_faults++; 
   
}

void Simulation::print_summary() {
    if (!this->flags.csv) {
        boost::format process_fmt(
            "Process %3d:  "
            "ACCESSES: %-6lu "
            "FAULTS: %-6lu "
            "FAULT RATE: %-8.2f "
            "RSS: %-6lu\n");

        for (auto entry : this->processes) {
            std::cout << process_fmt
                % entry.first
                % entry.second->memory_accesses
                % entry.second->page_faults
                % entry.second->get_fault_percent()
                % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "\n%-25s %12lu\n"
            "%-25s %12lu\n"
            "%-25s %12lu\n");

        std::cout << summary_fmt
            % "Total memory accesses:"
            % this->virtual_addresses.size()
            % "Total page faults:"
            % this->page_faults
            % "Free frames remaining:"
            % this->free_frames.size();
    }

    if (this->flags.csv) {
        boost::format process_fmt(
            "%d,"
            "%lu,"
            "%lu,"
            "%.2f,"
            "%lu\n");

        for (auto entry : processes) {
            std::cout << process_fmt
                % entry.first
                % entry.second->memory_accesses
                % entry.second->page_faults
                % entry.second->get_fault_percent()
                % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "%lu,,,,\n"
            "%lu,,,,\n"
            "%lu,,,,\n");

        std::cout << summary_fmt
            % this->virtual_addresses.size()
            % this->page_faults
            % this->free_frames.size();
    }
}

int Simulation::read_processes(std::istream& simulation_file) {
    int num_processes;
    simulation_file >> num_processes;

    for (int i = 0; i < num_processes; ++i) {
        int pid;
        std::string process_image_path;

        simulation_file >> pid >> process_image_path;

        std::ifstream proc_img_file(process_image_path);

        if (!proc_img_file) {
            std::cerr << "Unable to read file for PID " << pid << ": " << process_image_path << std::endl;
            return 1;
        }
        this->processes[pid] = Process::read_from_input(proc_img_file);
    }
    return 0;
}

int Simulation::read_addresses(std::istream& simulation_file) {
    int pid;
    std::string virtual_address;

    try {
        while (simulation_file >> pid >> virtual_address) {
            this->virtual_addresses.push_back(VirtualAddress::from_string(pid, virtual_address));
        }
    } catch (const std::exception& except) {
        std::cerr << "Error reading virtual addresses." << std::endl;
        std::cerr << except.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Error reading virtual addresses." << std::endl;
        return 1;
    }
    return 0;
}

int Simulation::read_simulation_file() {
    std::ifstream simulation_file(this->flags.filename);
    // this->simulation_file.open(this->flags.filename);

    if (!simulation_file) {
        std::cerr << "Unable to open file: " << this->flags.filename << std::endl;
        return -1;
    }
    int error = 0;
    error = this->read_processes(simulation_file);

    if (error) {
        std::cerr << "Error reading processes. Exit: " << error << std::endl;
        return error;
    }

    error = this->read_addresses(simulation_file);

    if (error) {
        std::cerr << "Error reading addresses." << std::endl;
        return error;
    }

    if (this->flags.file_verbose) {
        for (auto entry: this->processes) {
            std::cout << "Process " << entry.first << ": Size: " << entry.second->size() << std::endl;
        }

        for (auto entry : this->virtual_addresses) {
            std::cout << entry << std::endl;
        }
    }

    return 0;
}
