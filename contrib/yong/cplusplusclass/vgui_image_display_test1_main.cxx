#include <vcl_iostream.h>
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


#if 0
//:
// \file
// \brief  Example of text input/output windows.
// \author Peter Vanroose
// \date   March 2003


// Make a test tableau which catches the vgui_DESTROY event.


class example_flim_tableau : public vgui_tableau
{
  bool handle(vgui_event const& e)
  {
    if (e.type == vgui_BUTTON_DOWN) {
      vgui_dialog dialog("Farewell!");
      dialog.message("I am about to die!\nLast chance to save data would go here...");
      dialog.set_modal(true);
      dialog.ask();
      return true;
    }
    else
      return false;
  }
  vcl_string type_name() const { return "example_flim_tableau"; }
};

typedef vgui_tableau_sptr_t<example_flim_tableau> example_flim_tableau_sptr;

struct example_flim_tableau_new : public example_flim_tableau_sptr
{
  typedef example_flim_tableau_sptr base;
  example_flim_tableau_new() : base(new example_flim_tableau()) { }
};

// -----------------------------------------------------------------------------
// Now do some stuff...
int main (int argc, char** argv)
{
  vgui::init(argc, argv);

  if (argc < 2) {
    vcl_cerr << __FILE__ " : image_file argument required\n";
    vcl_abort();
  }

  vil_image_view<vxl_byte> img = vil_load(argv[1]);
  if (!img) {
    vcl_cerr << __FILE__ " : cannot load image from " << argv[1] << '\n';
    vcl_abort();
  }

  vgui_image_tableau_new img_tab(img);

  int wd=img.ni(), ht=img.nj();
  vgui_text_tableau_new text_tab;
  text_tab->add(wd*0.75f,ht*0.25f,"this is some text");

  example_flim_tableau_new flim_tab;

  vgui_viewer2D_tableau_new viewer(img_tab);


  vgui_shell_tableau_new shell_tab(viewer, text_tab, flim_tab);
  shell_tab->set_enable_key_bindings(true);

  vgui_window* main_window = vgui::produce_window(wd, ht);
  main_window->get_adaptor()->set_tableau(shell_tab);
  main_window->show();

  main_window->get_statusbar()->write("statusbar-text");

  vgui_text_graph(vcl_cerr);

  return vgui::run();
}

#endif

#if 0

// This is core/vgui/examples/example_vgui_observer.cxx
// This example displays some images in a window (the image filenames are
// given as parameters on the command line).  An observer is attached to the
// vgui_deck_tableau containing the images.  As the user flips through the
// images (using PageUp and PageDown), the observer changes the window
// title to the name of the appropriate image file.


struct example_window_title_setter : public vgui_observer
{
  char **argv;
  vgui_deck_tableau_sptr deck;

  example_window_title_setter(char **argv_, vgui_deck_tableau_sptr const &deck_)
    : argv(argv_)
    , deck(deck_)
  {
    deck->observers.attach(this);
  }

  ~example_window_title_setter()
  {
    deck->observers.detach(this);
  }

  vcl_string last_title;

  // When this observer receives an update message, it changes the window title.
  void update()
  {
    vgui_adaptor *a = vgui_adaptor::current;
    if (! a)
    {
      vgui_macro_warning << "no adaptor\n";
      return;
    }
    vgui_window *w = a->get_window();
    if (! w)
    {
      vgui_macro_warning << "no window\n";
      return;
    }
    int i = deck->index();
    if (last_title != argv[i + 1])
    {
      last_title = argv[i + 1];
      w->set_title(last_title);
    }
  }
  void update(vgui_message const&) { update(); } // just ignore the message
  void update(vgui_observable const*) { update(); } // ignore the observable
};

int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  // Create a deck tableau and add images given in the parameter list:
  vgui_deck_tableau_new deck;
  unsigned int w = 0, h = 0;
  for (int i=1; i<argc; ++i)
  {
    if (vul_file::exists(argv[i]))
    {
      vgui_image_tableau_new t(argv[i]);
      if (t->width ()>w) w = t->width ();
      if (t->height()>h) h = t->height();
      deck->add(t);
    }
    else
      vcl_cerr << "no such file : \'" << argv[i] << "\'\n";
  }
  deck->index(0);

  // Make an observer of the deck. It will set the window title
  // every time the deck's observers are notified.
  new example_window_title_setter(argv, deck);

  vgui_viewer2D_tableau_new viewer(deck);
  vgui_shell_tableau_new shell(viewer);
  shell->set_enable_key_bindings(true);

  return vgui::run(shell, w, h);
}


#endif

#if 0

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 1)
  {
    vcl_cerr << "Please give an image filename on the command line\n";
    return 0;
  }

  // Load image (given in the first command line param)
  // into an image tableau.
  vgui_image_tableau_new image(argv[1]);
  
  // Put the image tableau inside a 2D viewer tableau (for zoom, etc).
  vgui_viewer2D_tableau_new viewer(image);

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(viewer);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, image->width(), image->height());
}

#endif


#if 0

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 2)
  {
    vcl_cerr << "Please give two image filenames on the command line\n";
    return 0;
  }
  // Load two images(given in the first two command line args)
  // and construct separate image tableaux
  vgui_image_tableau_new image_tab1(argv[1]);
  vgui_image_tableau_new image_tab2(argv[2]);

  //Put the image tableaux into viewers
  vgui_viewer2D_tableau_new viewer1(image_tab1);
  vgui_viewer2D_tableau_new viewer2(image_tab2);

  //Put the viewers into a grid
  vgui_grid_tableau_sptr grid = new vgui_grid_tableau(2,1);
  grid->add_at(viewer1, 0,0);
  grid->add_at(viewer2, 1,0);
  // Put the grid into a shell tableau at the top the hierarchy
  vgui_shell_tableau_new shell(grid);

  // Create a window, add the tableau and show it on screen.
  int width = image_tab1->width() + image_tab2->width();
  int height = image_tab1->height() + image_tab2->height();
  return vgui::run(shell, width, height);
}

#endif

#if 0

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 2)
  {
    vcl_cerr << "Please give two image filenames on the command line\n";
    return 0;
  }
  // Load two images(given in the first two command line args)
  // and construct separate image tableaux
  vgui_image_tableau_new image_tab1(argv[1]);
  vgui_image_tableau_new image_tab2(argv[2]);

  //Put the image tableaux into a deck
  vgui_deck_tableau_sptr deck = vgui_deck_tableau_new();
  deck->add(image_tab1);
  deck->add(image_tab2);

  vgui_viewer2D_tableau_new viewer(deck);

  // Put the deck into a shell tableau at the top the hierarchy
  vgui_shell_tableau_new shell(viewer);

  // Create a window, add the tableau and show it on screen.
  int width = vnl_math_max(image_tab1->width(), image_tab2->width());
  int height = vnl_math_max(image_tab1->height(), image_tab2->height());

  //Add 50 to account for window borders
  return vgui::run(shell, width+50, height+50);
}

#endif

#if 0

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 1)
  {
    vcl_cerr << "Please give an image filename on the command line\n";
    return 0;
  }

  // Load an image into an image.tableau
  vgui_image_tableau_new image(argv[1]);

  // Put the image.tableau into a easy2D tableau
  vgui_easy2D_tableau_new easy2D(image);

  // Add a point, line, and infinite line
  easy2D->set_foreground(0,1,0);
  easy2D->set_point_radius(5);
  easy2D->add_point(10, 20);

  easy2D->set_foreground(0,0,1);
  easy2D->set_line_width(2);
  easy2D->add_line(100,100,200,400);

  easy2D->set_foreground(0,1,0);
  easy2D->set_line_width(2);
  easy2D->add_infinite_line(1,1,-100);

  // Put the easy2D tableau into a viewer2D tableau:
  vgui_viewer2D_tableau_new viewer(easy2D);
  vgui_shell_tableau_new shell(viewer);

  // Create a window, add the tableau and show it on screen:
  return vgui::run(shell, image->width(), image->height());
}


#endif

#if 0

//global pointer to the rubberband tableau
static vgui_rubberband_tableau_sptr rubber = 0;

//the menu callback functions
static void create_line()
{
  rubber->rubberband_line();
}
static void create_circle()
{
  rubber->rubberband_circle();
}

// Create the edit menu
vgui_menu create_menus()
{
  vgui_menu edit;
  edit.add("CreateLine",create_line,(vgui_key)'l',vgui_CTRL);
  edit.add("CreateCircle",create_circle,(vgui_key)'k',vgui_CTRL);
  vgui_menu bar;
  bar.add("Edit",edit);
  return bar;
}

int main(int argc, char ** argv)
{
  vgui::init(argc,argv);
  if (argc <= 1)
  {
    vcl_cerr << "Please give an image filename on the command line\n";
    return 0;
  }
  // Make the tableau hierarchy.
  vgui_image_tableau_new image(argv[1]);
  vgui_easy2D_tableau_new easy(image);
  vgui_rubberband_easy2D_client* r_client =
    new vgui_rubberband_easy2D_client(easy);
  rubber = vgui_rubberband_tableau_new(r_client);
  vgui_composite_tableau_new comp(easy, rubber);
  vgui_viewer2D_tableau_new viewer(comp);
  vgui_shell_tableau_new shell(viewer);

  // Create and run the window
  return vgui::run(shell, 512, 512, create_menus());
}


#endif

#if 0

// Set up a dummy callback function for the menu to call (for
// simplicity all menu items will call this function):
static void dummy()
{
  vcl_cerr << "Dummy function called\n";
}

// Create a vgui menu:
vgui_menu create_menus()
{
  vgui_menu file;
  file.add("Open",dummy,(vgui_key)'O',vgui_CTRL);
  file.add("Quit",dummy,(vgui_key)'R',vgui_SHIFT);

  vgui_menu image;
  image.add("Center image",dummy);
  image.add("Show histogram",dummy);

  vgui_menu bar;
  bar.add("File",file);
  bar.add("Image",image);

  return bar;
}

int main(int argc, char ** argv)
{
  vgui::init(argc,argv);
  if (argc <= 1)
 {
    vcl_cerr << "Please give an image filename on the command line\n";
    return 0;
 }

  // Make our tableau hierarchy.
  vgui_image_tableau_new image(argv[1]);
  vgui_viewer2D_tableau_new viewer(image);
  vgui_shell_tableau_new shell(viewer);

  // Create a window, but this time we also pass in a vgui_menu.
  return vgui::run(shell, 512, 512, create_menus());
}


#endif

#if 0

// Set up a dummy callback function for the menu to call (for
// simplicity all menu items will call this function):
static void dummy()
{
  vcl_cerr << "Dummy function called\n";
}

// Create a vgui_menu:
vgui_menu create_menus()
{
  vgui_menu file;
  file.add("Open", dummy);
  file.add("Quit", dummy);

  vgui_menu image;
  image.add("Center image", dummy);
  image.add("Show histogram", dummy);

  vgui_menu bar;
  bar.add("File",file);
  bar.add("Image",image);

  return bar;
}

 


  int main(int argc, char ** argv)
{
  vgui::init(argc,argv);
  if (argc <= 1)
  {
    vcl_cerr << "Please give an image filename on the command line\n";
    return 0;
  }
  // Load an image into an image tableau:
  vgui_image_tableau_new image(argv[1]);
  vgui_viewer2D_tableau_new viewer(image);
  vgui_shell_tableau_new shell(viewer);

  // Create a window and add the tableau:
  vgui_window *win = vgui::produce_window(512, 512);
  win->get_adaptor()->set_tableau(shell);

  // Add our menu items to the base pop-up
  // (this menu appears when the user clicks
  //  the right mouse button on the tableau)
  win->get_adaptor()->include_in_popup(create_menus());
  win->get_adaptor()->bind_popups();
  win->show();
  return vgui::run();
}


#endif


#if 0


// Make a vgui.dialog:
static void test_dialog()
{
  static int int_value = 2;
  static long long_value = 3;
  static float float_value = 3.1f;
  static double double_value = 4.2;
  static vcl_string string_value = "dialog test";
  static bool bool_value = true;
  static vcl_string inline_file_value = "/tmp/myfile_inline.txt";
  static vcl_string file_value = "/tmp/myfile.txt";
  static vcl_string regexp = "*.txt";
  static vcl_string inline_color_value = "blue";
  static vcl_string color_value = "red";

  static int choice_value = 1;
  vcl_vector<vcl_string> labels;
  labels.push_back(vcl_string("fltk"));
  labels.push_back(vcl_string("motif"));
  labels.push_back(vcl_string("gtk"));
  labels.push_back(vcl_string("glut"));
  labels.push_back(vcl_string("glX"));

  vgui_dialog mydialog("My dialog");
  mydialog.field("int value", int_value);
  mydialog.field("long value", long_value);
  mydialog.field("float value", float_value);
  mydialog.field("double value", double_value);
  mydialog.field("string value", string_value);
  mydialog.checkbox("bool value", bool_value);
  mydialog.choice("choice value", labels, choice_value);
  mydialog.inline_file("inline file browser", regexp, inline_file_value);
  mydialog.file("file browser", regexp, file_value);
  mydialog.inline_color("inline color value", inline_color_value);
  mydialog.color("color value", color_value);

  if (mydialog.ask())
  {
    vcl_cerr << "OK pressed.\n";
    vcl_cerr << "int_value : " << int_value << vcl_endl;
    vcl_cerr << "long_value : " << long_value << vcl_endl;
    vcl_cerr << "float_value : " << float_value << vcl_endl;
    vcl_cerr << "double_value : " << double_value << vcl_endl;
    vcl_cerr << "string_value : " << string_value << vcl_endl;
    vcl_cerr << "bool_value : " << vbl_bool_ostream::true_false(bool_value) << vcl_endl;
    vcl_cerr << "choice_value : " << choice_value << " " << labels[choice_value] << vcl_endl;
    vcl_cerr << "inline_file_value: " << inline_file_value << vcl_endl;
    vcl_cerr << "file_value: " << file_value << vcl_endl;
    vcl_cerr << "inline_color_value: " << inline_color_value << vcl_endl;
    vcl_cerr << "color_value: " << color_value << vcl_endl;
  } else {
    vcl_cerr << "Cancel pressed.\n";
    vcl_cerr << "int_value : " << int_value << vcl_endl;
    vcl_cerr << "long_value : " << long_value << vcl_endl;
    vcl_cerr << "float_value : " << float_value << vcl_endl;
    vcl_cerr << "double_value : " << double_value << vcl_endl;
    vcl_cerr << "string_value : " << string_value << vcl_endl;
    vcl_cerr << "bool_value : " << vbl_bool_ostream::true_false(bool_value) << vcl_endl;
    vcl_cerr << "choice_value : " << choice_value << " " << labels[choice_value] << vcl_endl;
    vcl_cerr << "inline_file_value: " << inline_file_value << vcl_endl;
    vcl_cerr << "file_value: " << file_value << vcl_endl;
    vcl_cerr << "inline_color_value: " << inline_color_value << vcl_endl;
    vcl_cerr << "color_value: " << color_value << vcl_endl;
  }
}

static void test_dialog2()
{
  vgui_dialog mydialog("My dialog2");
  vgui_image_tableau_new image("wallpaper_3.jpg");
  vgui_viewer2D_tableau_new viewer(image);
  mydialog.inline_tableau(viewer, 512, 512);

  mydialog.message("A picture");

  vcl_string button_txt("close");
  mydialog.set_ok_button(button_txt.c_str());
  mydialog.set_cancel_button(0);
  mydialog.ask();
}

// Create a vgui.menu with an item which shows the dialog box:
vgui_menu create_menus()
{
  vgui_menu test;
  test.add("Dialog", test_dialog);
  test.add("Dialog2", test_dialog2);

  vgui_menu bar;
  bar.add("Test",test);

  return bar;
}

int main(int argc, char ** argv)
{
  vgui::init(argc,argv);

  vgui_image_tableau_new image(argc>1 ? argv[1] : "az32_10.tif");
  vgui_viewer2D_tableau_new viewer(image);
  vgui_shell_tableau_new shell(viewer);

  // Create a window with a menu, add the tableau and show it on screen:
  return vgui::run(shell, image->width(), image->height(), create_menus());
}


#endif


#if 0

//------------------------------------------------------------
// A tableau that displays the mouse position
// when left mouse button is pressed.
struct example_tableau : public vgui_image_tableau
{
  example_tableau(char const* f) : vgui_image_tableau(f){ }

  ~example_tableau() { }

  bool handle(vgui_event const& e)
  {
     if (e.type == vgui_MOTION)
    {
      float pointx, pointy;
      vgui_projection_inspector p_insp;
      p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
      int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);
      vgui::out << '(' << intx << ' ' << inty << ")\n";
    }

    if (e.type == vgui_BUTTON_DOWN &&
        e.button == vgui_LEFT && e.modifier == 0)
    {
      vcl_cout << "selecting at " << e.wx << ' ' << e.wy << vcl_endl;
      return true; // event has been used
    }

    //  We are not interested in other events,
    //  so pass event to base class:
    return vgui_image_tableau::handle(e);
  }

  vcl_string type_name() const
  {
    return vcl_string("example_tableau");      
  }


};
//-------------------------------------------------------------
// Make a smart-pointer constructor for our tableau.
struct example_tableau_new : public vgui_image_tableau_sptr
{
  example_tableau_new(char const* f) : vgui_image_tableau_sptr(
    new example_tableau(f)) { }
};

 
  //-------------------------------------------------------------
//The first command line argument is expected
// to be an image filename.
int main(int argc,char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 1)
  {
    vcl_cerr << "Please give an image filename\n";
    return 0;
  }

  // Load an image into my tableau
  // (derived from vgui_image_tableau)
  vgui_tableau_sptr my_tab = example_tableau_new(argv[1]);

  vgui_viewer2D_tableau_new viewer(my_tab);
  vgui_shell_tableau_new shell(viewer);


  // Start event loop, using easy method.
  return vgui::run(shell, 512, 512);
}

 
#endif

