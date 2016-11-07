#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
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
#include <vil/vil_new.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_copy.h>
#include <math.h>

#if 0

// read a huge jpg file, save it as blocked pyrimad file list.

int main(int argc, char **argv)
{
    // load the base image in JPEG format;
    vil_image_resource_sptr base_image = vil_load_image_resource("world_shaded_43k.jpg");;
    //vil_image_resource_sptr base_image = vil_load_image_resource("wallpaper.jpg");;
    //vil_image_resource_sptr base_image = vil_load_image_resource("huston.jpg");;

    unsigned number_of_levels = 4;
    bool copy_base = true;

    // Generate a pyramid as an image_list (files in a directory)
    vil_pyramid_image_resource_sptr pyril = vil_new_pyramid_image_list_from_base(".",
                                          base_image,
                                          number_of_levels,
                                          copy_base,
                                          "tiff",
                                          "World_Map");

/*
    // Generate a pyramid as a multi-image tiff file
    vil_pyramid_image_resource_sptr pytif = vil_new_pyramid_image_from_base("worldmap_pyramid_file.tif",
                                    base_image,
                                    number_of_levels,
                                    "ptif",
                                    ".");
*/

  return 1;
}

 
#endif


#if 0
// convert all pyrimad files to blocked images;
int main(int argc, char **argv)
{
    int number_of_levels = 4;
    int current_level = number_of_levels - 1; // starting display from top level;
    vcl_stringstream cur_layer_filename_SS;
    vcl_string cur_layer_filename, cur_output_filename;

    for(; current_level >=0; current_level--)
    {
        cur_layer_filename.clear();
        cur_output_filename.clear();
        cur_layer_filename_SS.clear();
        cur_layer_filename_SS << "World_Map_" << current_level << ".tiff";
        cur_layer_filename_SS >> cur_layer_filename;
        cur_layer_filename_SS.clear();
        cur_layer_filename_SS << "World_Map_Blocked_" << current_level << ".tiff";
        cur_layer_filename_SS >> cur_output_filename;

        vil_image_resource_sptr imgr = vil_load_image_resource(cur_layer_filename.c_str());
        vil_image_view<vxl_byte> input_image;
        if (!imgr) return 0;
        input_image = imgr->get_view();


        unsigned size_block_i = 256, size_block_j = 256;
        unsigned ni = imgr->ni();
        unsigned nj = imgr->nj();
        unsigned nplanes = imgr->nplanes();

        vil_blocked_image_resource_sptr bimgr =  vil_new_blocked_image_resource(cur_output_filename.c_str(),
                                           ni, nj, nplanes,
                                           imgr->pixel_format(),
                                           size_block_i, size_block_j, 
                                           "tiff");
        
        if(bimgr)
        {
            bool put_view_worked = bimgr->vil_image_resource::put_view(input_image);
            if (!put_view_worked)
            { //report trouble
                vcl_cout << "Block image conversion failed." << vcl_endl;
                return 0;
            }
            else
            {
                vcl_cout << "Block image conversion succedd: " << cur_output_filename << vcl_endl;
            }
        }
    }

  return 1;
}

#endif 

#if 0

static unsigned number_of_levels = 4;
static unsigned current_level_left = number_of_levels - 1; // current layer displayed on the left side, from 3 to 1.
static unsigned current_zoomin_size = 4;    // size of blocks on the top layer;
static unsigned current_level_right = current_zoomin_size - 1; // current layer displayed on the left side, from 2 to 0.
static vcl_stringstream filename_SS;   
static vcl_string cur_layer_filename_left;
static vcl_string cur_layer_filename_right;
static unsigned zooming_grid_i = 1;
static unsigned zooming_grid_j = 1;
static unsigned zooming_box_size = static_cast<unsigned>(pow(2.0,4.0+static_cast<double>(current_zoomin_size)));
static vil_image_view<vxl_byte> left_image_view;
static vil_image_resource_sptr left_image_resource;
static vil_blocked_image_resource_sptr right_image_resource;
static vil_image_view<vxl_byte> right_image_view;
static vgui_image_tableau_sptr image_tab2;
static vgui_easy2D_tableau_sptr  easy2D;
static vgui_viewer2D_tableau_sptr viewer1, viewer2;
static vgui_grid_tableau_sptr grid;
static vgui_shell_tableau_sptr shell;
static int current_mouse_x, current_mouse_y;

void draw_zooming_box(void)
{
    easy2D->clear();

    easy2D->set_foreground(0,1,0);
    easy2D->set_line_width(3);

    easy2D->add_line(zooming_grid_i*zooming_box_size,zooming_grid_j*zooming_box_size, zooming_grid_i*zooming_box_size,(zooming_grid_j+1)*zooming_box_size);
    easy2D->add_line(zooming_grid_i*zooming_box_size,(zooming_grid_j+1)*zooming_box_size, (zooming_grid_i+1)*zooming_box_size,(zooming_grid_j+1)*zooming_box_size);
    easy2D->add_line((zooming_grid_i+1)*zooming_box_size,(zooming_grid_j+1)*zooming_box_size, (zooming_grid_i+1)*zooming_box_size,(zooming_grid_j)*zooming_box_size);
    easy2D->add_line(zooming_grid_i*zooming_box_size,zooming_grid_j*zooming_box_size, (zooming_grid_i+1)*zooming_box_size,(zooming_grid_j)*zooming_box_size);

    return;
}


struct my_pyrimad_zoom_tableau : public vgui_image_tableau
{
  my_pyrimad_zoom_tableau() : vgui_image_tableau(){ }
  my_pyrimad_zoom_tableau(vil_image_view<vxl_byte> f) : vgui_image_tableau(f){ }

  ~my_pyrimad_zoom_tableau() { }

  bool handle(vgui_event const& e)
  {    

    if (e.type == vgui_MOTION && e.type != vgui_BUTTON_DOWN)
    {
        float pointx, pointy;
        vgui_projection_inspector p_insp;
        p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
        int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);

        current_mouse_x = intx;
        current_mouse_y = inty;

        zooming_grid_i = current_mouse_x/zooming_box_size;
        zooming_grid_j = current_mouse_y/zooming_box_size;
        draw_zooming_box();

        vgui::out << '(' << intx << ' ' << inty << ")\n";
        return true; // event has been used
    }

    
    if (e.type == vgui_BUTTON_DOWN)
    {
        zooming_grid_i = current_mouse_x/zooming_box_size;
        zooming_grid_j = current_mouse_y/zooming_box_size;
        draw_zooming_box();

        right_image_view.deep_copy(right_image_resource->get_block(zooming_grid_i, zooming_grid_j));
        image_tab2->set_image_view(right_image_view);
        viewer2->post_redraw();

        return true; // event has been used
    }


    if (e.type ==  vgui_KEY_DOWN && e.key == 's' && e.modifier == vgui_CTRL)
    {
        if(    current_zoomin_size == 1)
            return true;
        
        current_zoomin_size--;
        current_level_right = current_zoomin_size-1;
        zooming_box_size = static_cast<unsigned>(pow(2.0,4.0+static_cast<double>(current_zoomin_size)));
        vcl_cout << "zooming box size: " << zooming_box_size << vcl_endl;

        zooming_grid_i = current_mouse_x/zooming_box_size;
        zooming_grid_j = current_mouse_y/zooming_box_size;
        draw_zooming_box();

        // load new image resource;
        filename_SS.clear();
        filename_SS << "World_Map_Blocked_" << current_level_right << ".tiff";
        filename_SS >> cur_layer_filename_right;


        right_image_resource = blocked_image_resource(vil_load_image_resource(cur_layer_filename_right.c_str()));
        if (!right_image_resource) 
        {
            vcl_cout << "Loading image failed: " << cur_layer_filename_right << vcl_endl;
            return 0;
        }

        unsigned nbi = right_image_resource->n_block_i();
        unsigned nbj = right_image_resource->n_block_j();

        right_image_view.deep_copy(right_image_resource->get_block(zooming_grid_i, zooming_grid_j));
        image_tab2->set_image_view(right_image_view);
        viewer2->post_redraw();

        return true; // event has been used
    }
    if (e.type ==  vgui_KEY_DOWN && e.key == 'b' && e.modifier == vgui_CTRL)
    {
        if(    current_zoomin_size == 4)
            return true;
        
        current_zoomin_size++;
        current_level_right = current_zoomin_size-1;
        zooming_box_size = static_cast<unsigned>(pow(2.0,4.0+static_cast<double>(current_zoomin_size)));
        vcl_cout << "zooming box size: " << zooming_box_size << vcl_endl;


        zooming_grid_i = current_mouse_x/zooming_box_size;
        zooming_grid_j = current_mouse_y/zooming_box_size;
        draw_zooming_box();

        // load new image resource;
        filename_SS.clear();
        filename_SS << "World_Map_Blocked_" << current_level_right << ".tiff";
        filename_SS >> cur_layer_filename_right;


        right_image_resource = blocked_image_resource(vil_load_image_resource(cur_layer_filename_right.c_str()));
        if (!right_image_resource) 
        {
            vcl_cout << "Loading image failed: " << cur_layer_filename_right << vcl_endl;
            return 0;
        }

        unsigned nbi = right_image_resource->n_block_i();
        unsigned nbj = right_image_resource->n_block_j();

        right_image_view.deep_copy(right_image_resource->get_block(zooming_grid_i, zooming_grid_j));
        image_tab2->set_image_view(right_image_view);
        viewer2->post_redraw();

        return true; // event has been used
    }
    //  We are not interested in other events,
    //  so pass event to base class:
    return vgui_image_tableau::handle(e);
  }

  vcl_string type_name() const
  {
    return vcl_string("my_pyrimad_zoom_tableau");      
  }
};
//-------------------------------------------------------------
// Make a smart-pointer constructor for our tableau.
struct my_pyrimad_zoom_tableau_new : public vgui_image_tableau_sptr
{
  my_pyrimad_zoom_tableau_new(vil_image_view<vxl_byte> const f) : vgui_image_tableau_sptr(
    new my_pyrimad_zoom_tableau(f)) { }
};

static vgui_tableau_sptr image_tab1;


int main(int argc, char **argv)
{

    filename_SS.clear();
    filename_SS << "World_Map_Blocked_" << current_level_left << ".tiff";
    filename_SS >> cur_layer_filename_left;


    vgui::init(argc, argv);

    // Load top layer image to the left grid
    left_image_resource = vil_load_image_resource(cur_layer_filename_left.c_str());
    if (!left_image_resource) 
    {
        vcl_cout << "Loading image failed: " << cur_layer_filename_left << vcl_endl;    
        return 0;
    }
    left_image_view = left_image_resource->get_view();


    // load the upper-left block to the right grid
    filename_SS.clear();
    filename_SS << "World_Map_Blocked_" << current_level_right << ".tiff";
    filename_SS >> cur_layer_filename_right;

    right_image_resource = blocked_image_resource(vil_load_image_resource(cur_layer_filename_right.c_str()));
    if (!right_image_resource) 
    {
        vcl_cout << "Loading image failed: " << cur_layer_filename_right << vcl_endl;
        return 0;
    }

    right_image_view = right_image_resource->get_block(zooming_grid_i, zooming_grid_j);

    image_tab1 = my_pyrimad_zoom_tableau_new(left_image_view);
    image_tab2 = vgui_image_tableau_new(right_image_view);


    // Put the image.tableau into a easy2D tableau
    easy2D = vgui_easy2D_tableau_new(image_tab1);


    // Add a point, line, and infinite line
    draw_zooming_box();

    //Put the image tableaux into viewers
    viewer1 = vgui_viewer2D_tableau_new(easy2D);
    viewer2 = vgui_viewer2D_tableau_new(image_tab2);

    //Put the viewers into a grid
    grid = new vgui_grid_tableau(2,1);
    grid->add_at(viewer1, 0,0);
    grid->add_at(viewer2, 1,0);
    // Put the grid into a shell tableau at the top the hierarchy
    shell = vgui_shell_tableau_new(grid);

    // Create a window, add the tableau and show it on screen.
    return vgui::run(shell, 1000, 600);
}


#endif

