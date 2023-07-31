/**
 * This file contains implementations for methods in the Process class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "process/process.h"

using namespace std;


Process* Process::read_from_input(std::istream& in) {
    // TODO
    Page* page;
    vector<Page*> pages;
    int process_size = 0;   
    while ((page = Page::read_from_input(in)) != nullptr){
        process_size = process_size + page->size();
        pages.push_back(page);
    }
    return new Process(process_size, pages);
}


size_t Process::size() const
{
    // TODO
    return this->num_bytes;
}

  /**
    * Returns true if this process contains a page with the provided index, or
    * false otherwise.
    */
bool Process::is_valid_page(size_t index) const
{
    // TODO
    if (index < this->pages.size()){  
        return true;
    }
    else{
        return false;
    }
}

 /**
    * Returns the resident set size of this process. 
    * resident set size (RSS) is the portion of memory occupied by a process that is 
    * held in main memory (RAM).
    */
size_t Process::get_rss() const
{
    // TODO
    int page_count = this->page_table.rows.size();   
    int rss_count = 0;   
    for(int i= 0; i < page_count; i++){
        if(this->page_table.rows[i].present == true){   
            rss_count++;
        }
    }
    return rss_count;
}

   /**
    * Returns the fault rate of this process, as a percentage of all memory
    * accesses.
    */
double Process::get_fault_percent() const
{
    if (this->memory_accesses > 0){  
        double memory_access_count = this->memory_accesses;  
        double page_faults_count = this->page_faults;
        double fault_percent = (page_faults_count / memory_access_count) * 100;   
        return fault_percent;
    }
    return 0.0;
}
