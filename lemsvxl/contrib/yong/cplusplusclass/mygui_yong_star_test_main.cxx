#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vgui/vgui.h>
#include <vnl/vnl_math.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vbl/vbl_bool_ostream.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_projection_inspector.h>
#include <vul/vul_file.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_observer.h>
#include <vgui/vgui_macro.h>

#include <vcl_cstdlib.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_statusbar.h>
#include <vgui/vgui_text_graph.h>
#include <vgui/vgui_tableau.h>

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>
#include <cplusplusclass/mygui_yong_star_tableau.h>
#include <cplusplusclass/mygui_yong_star_tableau_sptr.h>
#include <cplusplusclass/mygui_yong_star_tableau2.h>
#include <cplusplusclass/mygui_yong_star_tableau2_sptr.h>
#include <cplusplusclass/my_rectangle.h>
#include <cplusplusclass/my_rectangle_sptr.h>
#include <cplusplusclass/yong_star.h>
#include <cplusplusclass/yong_star_sptr.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vgui/vgui_style.h>

#if 0

  // Create a yong_star vector;
static  vcl_vector<yong_star_sptr> star_list;
static  vgui_text_tableau_sptr text_tab;

struct my_star_tableau_synchronizer_1to2 : public vgui_observer
{
  mygui_yong_star_tableau_sptr active_tab;
  mygui_yong_star_tableau2_sptr passive_tab;

  my_star_tableau_synchronizer_1to2(mygui_yong_star_tableau_sptr const &tab1_, mygui_yong_star_tableau2_sptr const &tab2_)
    : active_tab(tab1_), passive_tab(tab2_)
  {
    tab1_->observers.attach(this);
  }

  ~my_star_tableau_synchronizer_1to2()
  {
    active_tab->observers.detach(this);
  }

  // When this observer receives an update message, it changes the window title.
  void update()
  {
        passive_tab->draw();

        text_tab->clear();
        unsigned star_num = star_list.size();
        for(unsigned i=0; i<star_num; i++)
        {
            double x = star_list[i]->center_x();
            double y = star_list[i]->center_y();
            vcl_stringstream number_SS;
            vcl_string number_string;
            number_SS.clear();
            number_SS << i+1;
            number_SS >> number_string;
            text_tab->add(x,y,number_string.c_str());
        }        
  }
  void update(vgui_message const&) { update(); } // just ignore the message
  void update(vgui_observable const*) { update(); } // ignore the observable
};


int main(int argc, char **argv)
{
  vgui::init(argc, argv);


  //mygui_yong_star_tableau_sptr my_star_tab = 
  mygui_yong_star_tableau_new my_star_tab_left("my_star_tab");
  mygui_yong_star_tableau2_new my_star_tab_right("my_star_tab");


  // create three stars
  yong_star_sptr star_1 = new yong_star(100, 100, 3, 10, 0);
  yong_star_sptr star_2 = new yong_star(200, 200, 4, 20, 0);
  yong_star_sptr star_3 = new yong_star(350, 350, 5, 30, 0);

  star_list.push_back(star_1);
  star_list.push_back(star_2);
  star_list.push_back(star_3);
  my_star_tab_left->set_star_list(star_list);
  my_star_tab_right->set_star_list(star_list);

  text_tab = vgui_text_tableau_new();
  


    //Put the image tableaux into viewers
  vgui_viewer2D_tableau_sptr viewer1, viewer2, viewer3;
  viewer1 = vgui_viewer2D_tableau_new(my_star_tab_left);
  viewer2 = vgui_viewer2D_tableau_new(my_star_tab_right);
  viewer3 = vgui_viewer2D_tableau_new(text_tab);

    struct my_star_tableau_synchronizer_1to2 synchronizer(my_star_tab_left, my_star_tab_right);
        //new my_star_tableau_synchronizer_1to2(my_star_tab_left, my_star_tab_right);

    synchronizer.update();

    //Put the viewers into a grid
  vgui_grid_tableau_sptr grid;
    grid = new vgui_grid_tableau(2,1);
    grid->add_at(viewer1, 0,0);
    grid->add_at(viewer2, 1,0);

    // Put the grid into a shell tableau at the top the hierarchy
    vgui_shell_tableau_sptr shell;
    shell = vgui_shell_tableau_new(grid, viewer3);


  return vgui::run(shell, 900, 512);

}


#endif

