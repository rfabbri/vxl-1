// This is lemsvxlsrc/contrib/bwm_lidar/algo/set_operations.cxx

//:
// \file
// \brief Standard Set Operations
//
// \author Ibrahim Eden, (ieden@lems.brown.edu)
// \date 12/16/2007
//      
// \endverbatim

#include "set_operations.h"

//: Initializes an empty set
set_operations::set_operations(){
}

//: Add a free element (i.e., the insertion will create a new set)
void set_operations::add(int element){
  vcl_vector<int> new_set;
  new_set.push_back(element);
  this->sets.push_back(new_set);
}

//: Add a dependent element (i.e., the insertion will be added to an existing set that contains set_element)
void set_operations::add(int element, int set_element){
  for(unsigned i=0; i<this->sets.size(); i++){
    for(unsigned j=0; j<this->sets[i].size(); j++){
      if(this->sets[i][j]==set_element){
        this->sets[i].push_back(element);
        return;
      }
    }
  }
}

//: Join two sets that has element1 and element2
void set_operations::join(int element1, int element2){
  int el1_id = -1;
  int el2_id = -1;
  for(unsigned i=0; i<this->sets.size(); i++){
    for(unsigned j=0; j<this->sets[i].size(); j++){
      if(this->sets[i][j]==element1){
        el1_id = i;
      }
      if(this->sets[i][j]==element2){
        el2_id = i;
      }
    }
  }
  if(el1_id==-1 || el2_id==-1){
    return;
  }
  if(el1_id==el2_id){
    return;
  }
  int min_id = vnl_math_min(el1_id,el2_id);  
  int max_id = vnl_math_max(el1_id,el2_id);  

  for(unsigned i=0; i<this->sets[max_id].size(); i++){
    this->sets[min_id].push_back(this->sets[max_id][i]);
  }
  this->sets[max_id].clear();
  this->sets.erase(this->sets.begin()+max_id);
}

//: Print all existing sets
void set_operations::print(){
  for(unsigned i=0; i<this->sets.size(); i++){
    vcl_cout << "Set " << i << ": ";
    for(unsigned j=0; j<this->sets[i].size(); j++){
      vcl_cout << this->sets[i][j] << " ";
    }
    vcl_cout << "\n";
  }
  vcl_cout << "\n";
}

//: Get the set index for the given element
int set_operations::get_set_id(int element){
  for(unsigned i=0; i<this->sets.size(); i++){
    for(unsigned j=0; j<this->sets[i].size(); j++){
      if(this->sets[i][j]==element){
        return i;
      }
    }
  }
  return -1;
}
