/**
 * This file contains implementations for methods in the PageTable class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include <climits>
#include "page_table/page_table.h"
#include <algorithm> 

using namespace std;


size_t PageTable::get_present_page_count() const {
    // TODO: implement me
   int present_page_count = 0;
   for(int i = 0; i< rows.size(); i++){
       if (rows[i].present == true){
       present_page_count++;
       }
   }
    return present_page_count;
}

size_t PageTable::get_oldest_page() const {
    // TODO: implement me
    int page_index = 0; 
    int max_time = INT_MAX;  
    for (int i = 0; i < rows.size(); i++) {
        if (rows[i].present && rows[i].loaded_at < max_time) {
            max_time = rows[i].loaded_at;
            page_index = i;    
        }
    }
    return page_index;
}


size_t PageTable::get_least_recently_used_page() const {
    // TODO: implement me
    int page_index = 0;  
	int access_time = INT_MAX;  
	for (int i = 0; i < rows.size(); i++)
	{	
		if (rows[i].present && rows[i].last_accessed_at < access_time){
			access_time = rows[i].last_accessed_at;
            page_index = i;
		}
	}
  return page_index;
}
