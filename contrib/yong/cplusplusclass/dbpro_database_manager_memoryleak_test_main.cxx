#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vcl_map.h>
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
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>

#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_statusbar.h>
#include <vgui/vgui_text_graph.h>
#include <vgui/vgui_tableau.h>

#include <dbpro/dbpro_db_value.h>
#include <dbpro/dbpro_db_tuple.h>
#include <dbpro/dbpro_db_relation.h>
#include <dbpro/dbpro_db_relation_sptr.h>
#include <dbpro/dbpro_db_selection.h>
#include <dbpro/dbpro_db_selection_sptr.h>
#include <dbpro/dbpro_database.h>
#include <dbpro/dbpro_database_sptr.h>
#include <dbpro/dbpro_database_manager.h>
#include <dbpro/dbpro_db_query.h>
#include <dbpro/dbpro_db_query_sptr.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <bpro/bpro_storage_sptr.h>



#if 1

#define BACKWARD_WINDOW   5
#define FORWARD_WINDOW    5
#define INPUT_F             true
#define OUTPUT_F            false

int frame_number; 
vcl_vector<vcl_string> filenames(100);


// get intput frame from database, if not exist, load from file
vil_image_resource_sptr get_frame(int frame_n, bool in_out)
{

  dbpro_db_query_sptr q1 = dbpro_db_query_new("Image", "Frame Number" , EQ, frame_n);
  dbpro_db_query_sptr q2;
  if(in_out == OUTPUT_F)
    q2 = dbpro_db_query_new("Image", "Image Name" , EQ, vcl_string("OUTPUT"));
  else
    q2 = dbpro_db_query_new("Image", "Image Name" , EQ, vcl_string("INPUT"));

  dbpro_db_selection_sptr s1;
  dbpro_db_selection_sptr s2;
  dbpro_database_manager::instance()->select(q1, s1);
  dbpro_database_manager::instance()->select(q2, s2);
  dbpro_db_selection_sptr s3 = s1 & s2;

  if(s3->size() == 1)
  {
    bpro_storage_sptr get_image_storage;
    vidpro_image_storage_sptr get_vidpro_image_storage;
    dbpro_database_manager::instance()->get(s3, "Image Storage", get_image_storage);
    get_vidpro_image_storage.vertical_cast(get_image_storage);
    vil_image_resource_sptr get_image_resource = get_vidpro_image_storage->get_image();
    return get_image_resource;
  }
  else if(s3->size() == 0)
  {
    // load from file;
    int file_ID = frame_n%100;
    if(file_ID < 0)
      file_ID = 100+file_ID;
    vil_image_view<vxl_byte> image_view = vil_load(filenames[file_ID].c_str());
    vil_image_resource_sptr image_resource = vil_new_image_resource_of_view(image_view);
    vidpro_image_storage_sptr image_storage = vidpro_image_storage_new();
    image_storage->set_image(image_resource);
    bpro_storage_sptr convert_storage_sptr = image_storage;
    dbpro_db_tuple_sptr image_tup = dbpro_db_tuple_new(frame_n, vcl_string("INPUT"), convert_storage_sptr);
    dbpro_database_manager::instance()->add_tuple(vcl_string("Image"), image_tup);
    return image_resource;
  }
  else
  {
    vcl_cout << "more than one frame is found in database!" << vcl_endl;
    exit(-1);
  }
}

// save frame to the database
void put_frame(int frame_n, bool in_out, vil_image_resource_sptr output)
{
    // load from file;
    vidpro_image_storage_sptr image_storage = vidpro_image_storage_new();
    image_storage->set_image(output);
    bpro_storage_sptr convert_storage_sptr = image_storage;
    dbpro_db_tuple_sptr image_tup;
    if(in_out == OUTPUT_F)
      image_tup = dbpro_db_tuple_new(frame_n, vcl_string("OUTPUT"), convert_storage_sptr);
    else
      image_tup = dbpro_db_tuple_new(frame_n, vcl_string("INPUT"), convert_storage_sptr);
    dbpro_database_manager::instance()->add_tuple(vcl_string("Image"), image_tup);
}

// check whether certain frame is in database
bool frame_exist(int frame_n, bool in_out)
{
  dbpro_db_query_sptr q1 = dbpro_db_query_new("Image", "Frame Number" , EQ, frame_n);
  dbpro_db_query_sptr q2;
  if(in_out == OUTPUT_F)
    q2 = dbpro_db_query_new("Image", "Image Name" , EQ, vcl_string("OUTPUT"));
  else
    q2 = dbpro_db_query_new("Image", "Image Name", EQ, vcl_string("INPUT"));

  dbpro_db_selection_sptr s1;
  dbpro_db_selection_sptr s2;
  dbpro_database_manager::instance()->select(q1, s1);
  dbpro_database_manager::instance()->select(q2, s2);
  dbpro_db_selection_sptr s3 = s1 & s2;

  if(s3->size() == 1)
    return true;
  else
    return false;
}

void sliding_window_management()
{
  dbpro_db_query_sptr q1 = dbpro_db_query_new("Image", "Frame Number", BT, frame_number+FORWARD_WINDOW);
  dbpro_db_query_sptr q2 = dbpro_db_query_new("Image", "Frame Number", LT, frame_number-BACKWARD_WINDOW);
  dbpro_db_selection_sptr s1;
  dbpro_db_selection_sptr s2;
  dbpro_database_manager::instance()->select(q1, s1);
  dbpro_database_manager::instance()->select(q2, s2);
  dbpro_db_selection_sptr s3 = s1 | s2;

  s3->delete_tuples();
}

//------------------------------------------------------------
// A tableau that displays the mouse position
// when left mouse button is pressed.
struct example_tableau : public vgui_image_tableau
{
  unsigned int image_cnt;

  example_tableau(char const* f) : vgui_image_tableau(f){ image_cnt = 0; }

  ~example_tableau() { }

  bool handle(vgui_event const& e)
  {
    if(e.type ==  vgui_KEY_DOWN)
    {
      int i=0;
    }
    if(e.type ==  vgui_KEY_DOWN && (e.key == vgui_CURSOR_RIGHT) || (e.key == vgui_CURSOR_LEFT))
    {
      if(e.key == vgui_CURSOR_RIGHT)
        ++frame_number;
      else if(e.key == vgui_CURSOR_LEFT)
        --frame_number;

      vcl_cout << "Processing frame number: " << frame_number << vcl_endl;

      if(!frame_exist(frame_number, OUTPUT_F))
      {
        vil_image_resource_sptr current_frame = get_frame(frame_number, INPUT_F);
        vil_image_resource_sptr prev_frame = get_frame(frame_number-1, INPUT_F);
        vil_image_view<vxl_byte> current_view = current_frame->get_view();
        vil_image_view<vxl_byte> prev_view = prev_frame->get_view();
        vil_image_view<vxl_byte> diff_img;
        vil_math_image_abs_difference(current_view, prev_view, diff_img);
        vil_image_resource_sptr diff_resource = vil_new_image_resource_of_view(diff_img);
        put_frame(frame_number, OUTPUT_F, diff_resource);
        this->set_image_resource(diff_resource);
        this->post_redraw();
      }
      else
      {
        vil_image_resource_sptr diff_resource = get_frame(frame_number, OUTPUT_F);
        this->set_image_resource(diff_resource);
        this->post_redraw();
      }

      sliding_window_management();
      dbpro_database_manager::instance()->print_database();
    }

    if(e.type == vgui_KEY_DOWN && e.key == 's')
    {
      dbpro_database_manager::instance()->save_database("image_diff.vsl");
      vcl_cout << "Save database to binary IO file " << "image_diff.vsl" << vcl_endl;
    }

    if(e.type == vgui_KEY_DOWN && e.key == 'l')
    {
      dbpro_database_manager::instance()->load_database("image_diff.vsl");
      vcl_cout << "Load database from binary IO file " << "image_diff.vsl" << vcl_endl;
      vil_image_resource_sptr diff_resource = get_frame(frame_number, OUTPUT_F);
      dbpro_database_manager::instance()->print_database();
      this->set_image_resource(diff_resource);
      this->post_redraw();
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

  // initialize frame number;
  frame_number = 0;
  // initialize filename
  for(unsigned int i=0; i<100; i++)
  {
    vcl_stringstream SS;
    SS.clear();
    SS << ".\\CIT\\CIT_004_";
      SS << vcl_setw (5);
      SS << vcl_setfill('0');
      SS << i;
      SS << ".bmp";
    filenames[i].clear();
    SS >> filenames[i];
  }

  // initialize the basebase;
  vcl_vector<vcl_string> r1_names(3);
  vcl_vector<vcl_string> r1_types(3);
  r1_names[0] = "Frame Number";
  r1_names[1] = "Image Name";
  r1_names[2] = "Image Storage";
  r1_types[0] = dbpro_db_value_t<int>::type();
  r1_types[1] = dbpro_db_value_t<vcl_string>::type();
  r1_types[2] = dbpro_db_value_t<bpro_storage_sptr>::type();
  dbpro_db_relation_sptr init_r = dbpro_db_relation_new(r1_names, r1_types);
  dbpro_database_sptr init_db = dbpro_database_new();
  init_db->add_new_relation("Image", init_r);
  dbpro_database_manager::instance();
  dbpro_database_manager::instance()->set_database(init_db);
  vcl_cout << "After database initialization: " << vcl_endl;
  dbpro_database_manager::instance()->print_database();
  vsl_add_to_binary_loader(vidpro_image_storage());

  // Load an image into my tableau
  // (derived from vgui_image_tableau)
  vgui_tableau_sptr my_tab = example_tableau_new(filenames[0].c_str());

  vgui_viewer2D_tableau_new viewer(my_tab);
  vgui_shell_tableau_new shell(viewer);


  // Start event loop, using easy method.
  return vgui::run(shell, 800, 600);
}

 
#endif

