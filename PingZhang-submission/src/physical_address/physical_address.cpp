/**
 * This file contains implementations for methods in the PhysicalAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "physical_address/physical_address.h"

using namespace std;

string PhysicalAddress::to_string() const {
    // TODO: implement me
    return bitset<10>(this->frame).to_string() + bitset<6>(this->offset).to_string();;
}


ostream& operator <<(ostream& out, const PhysicalAddress& address) {
    // TODO: implement me
    string physical_address = address.to_string();    
    string address_frame = std::to_string(address.frame);
    string address_offset = std::to_string(address.offset);
    return out << physical_address << " [frame: " << address_frame << "; offset: " << address_offset << "]";
}
