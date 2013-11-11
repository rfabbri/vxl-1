#ifndef on_demand_deck_tableau_h_
#define on_demand_deck_tableau_h_

#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_tableau_sptr.h>

class on_demand_deck_tableau : public vgui_deck_tableau{
   

  public:
        on_demand_deck_tableau() : vgui_deck_tableau()
         { }
        ~on_demand_deck_tableau() { }

    void set_filenames(const vcl_vector<vcl_string> & names){
      filenames_ = names;
    }
    
  private: 
    vcl_vector<vcl_string> filenames_;
};


typedef vgui_tableau_sptr_t<on_demand_deck_tableau> on_demand_deck_tableau_sptr;

struct on_demand_deck_tableau_new : public on_demand_deck_tableau_sptr
{
on_demand_deck_tableau_new() : on_demand_deck_tableau_sptr( new on_demand_deck_tableau()) { }
};

#endif // on_demand_deck_tableau_h_
