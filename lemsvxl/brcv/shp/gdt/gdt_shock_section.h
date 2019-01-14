//: Dec 20, 2005 MingChing Chang
//  Defines the Shock Section for Goedesic DT Wavefront Propagation Algorithm

#ifndef gdt_shock_section_h_
#define gdt_shock_section_h_

#include <cmath>
#include <utility>
#include <map>

#include <gdt/gdt_shock.h>

class dbmsh3d_gdt_edge;

//: The set of shock section
class gdt_shock_section 
{
protected:
  //: a map of shocks sorted by its tau as key. 
  std::map<double, gdt_shock*> S_map_;

public:
  //: ====== Constructor/Destructor ======
  gdt_shock_section () {
  }

  //: ====== Data access functions ======
  std::map<double, gdt_shock*>* S_map() {
    return &S_map_;
  }

  //: ====== Query functions ======  
  int size () {
    return S_map_.size();
  }
  gdt_shock* _find_shock (const double stau) {
    std::map<double, gdt_shock*>::iterator it = S_map_.find (stau);
    if (it == S_map_.end())
      return NULL;
    return (*it).second;
  }

  //: ====== Modification Functions ======
  void _add_shock (gdt_shock* S);
  void clear () {
    S_map_.clear ();
  }

  void put_all_shocks_to_queue (std::map<std::pair<double, int>, gdt_shock*>* shock_queue); 
};

#endif



