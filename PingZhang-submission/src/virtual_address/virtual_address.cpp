/**
 * This file contains implementations for methods in the VirtualAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "virtual_address/virtual_address.h"

using namespace std;

 
VirtualAddress VirtualAddress::from_string(int process_id, string address) {
    // TODO: implement me
    // int temp = 2;    
    int page_bits = stoi(address.substr(0, PAGE_BITS), nullptr, 2);
    int offset_bits = stoi(address.substr(PAGE_BITS, ADDRESS_BITS), nullptr, 2);  
    VirtualAddress virtual_address = VirtualAddress(process_id, page_bits, offset_bits);
    return virtual_address;
}


string VirtualAddress::to_string() const {
    // TODO: implement me
    string page_index = std::bitset<PAGE_BITS>(page).to_string();  
    string page_offset = std::bitset<OFFSET_BITS>(offset).to_string();
    string virtual_address = page_index + page_offset;
    return virtual_address;
}


ostream& operator <<(ostream& out, const VirtualAddress& address) {
    // TODO: implement me
    string process_ID = std::to_string(address.process_id);   
    string virtual_address = address.to_string();
    string virtual_address_page = std::to_string(address.page);
    string virtual_address_offset = std::to_string(address.offset);
    return out << "PID " << process_ID << " @ " << virtual_address << " [page: " << virtual_address_page << "; offset: " << virtual_address_offset << "]";
}
