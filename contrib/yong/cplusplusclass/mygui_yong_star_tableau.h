#ifndef mygui_yong_star_tableau_h_
#define mygui_yong_star_tableau_h_
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <cplusplusclass/mygui_yong_star_tableau.h>
#include <cplusplusclass/mygui_yong_star_tableau_sptr.h>
#include <cplusplusclass/yong_star.h>
#include <cplusplusclass/yong_star_sptr.h>
#include <vgui/vgui_observable.h>

struct mygui_yong_star_tableau_new;

class mygui_yong_star_tableau : public vgui_easy2D_tableau

{
    private:
    vcl_vector<yong_star_sptr> star_list_;
    int current_selected_star_;
    int current_highlighted_star_;

    public:
  //: Constructor - don't use this, use vgui_easy2D_tableau_new.
  //  Create an easy2D with the given name.
  mygui_yong_star_tableau(const char* n="unnamed");


    ~mygui_yong_star_tableau() { }
    vcl_string mygui_yong_star_tableau::type_name() const;
    bool handle(const vgui_event& e);
    virtual bool draw();
    bool set_star_list(vcl_vector<yong_star_sptr> list);
    
    vgui_observable observers;

  //: Name of this tableau.
  vcl_string name_;

  //: the style of the objects
  vgui_style_sptr style_;

};


//-------------------------------------------------------------
// Make a smart-pointer constructor for our tableau.
struct mygui_yong_star_tableau_new : public mygui_yong_star_tableau_sptr
{
  //: Constructor - create an easy2D with the given name.
  mygui_yong_star_tableau_new(char const *n="unnamed") :
    mygui_yong_star_tableau_sptr(new mygui_yong_star_tableau(n)) { }

};

#endif // 
