// This is shp/dbsksp/examples/xgraph2ps.cxx

// \file


#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/vis/dbsksp_xgraph_displayer.h>
#include <dbsksp/vis/dbsksp_xgraph_tableau.h>
#include <dbsksp/algo/dbsksp_trace_boundary.h>
#include <dbgui/dbgui_utils.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vnl/vnl_math.h>

#include <vcl_cstdlib.h>
#include <vul/vul_file.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>



//: Read in a shock graph and an image and produce a PostScript drawing the graph
// graph on top of the original image
int main(int argc, char *argv[])
{
  

  if (argc < 2)
  {
    vcl_cout << "This program draws an xshock graph on postscript file.\n"
      << "Output file can have extension: .ps, .eps, .pdf (default), or .txt\n" 
      << "Usage: " << argv[0] << " input_xgraph_xml [ output_file ]\n";
    return 0;
  }

  vgui::init(argc, argv);

  vcl_string xgraph_file = argv[1];
  vcl_string output_file = "";

  if (argc > 2)
  {
    output_file = argv[2];
  }
  else // if not specified, use input filename
  {
    output_file = vul_file::strip_extension(xgraph_file) + ".pdf";
  }
  

  // Load shock graph
  dbsksp_xshock_graph_sptr xgraph;
  if (!x_read(xgraph_file, xgraph))
  {
    vcl_cout << "ERROR: couldn't read xgraph XML file - " << xgraph_file << vcl_endl;
    return EXIT_FAILURE;
  }

  // scale the xgraph so that its size is 128 // arbitrary choice
  double norm_size = 128;
  double cur_size = vcl_sqrt(xgraph->area());
  xgraph->scale_up(0, 0, norm_size / cur_size);

  // Move the xgraph to the 1st quarter for drawing
  // 10-pixel padding
  vsol_polygon_2d_sptr boundary = dbsksp_trace_xgraph_boundary_as_polygon(xgraph);
  vsol_box_2d_sptr bbox = boundary->get_bounding_box();


  //xgraph->update_bounding_box();
  //vsol_box_2d_sptr bbox = xgraph->bounding_box();


  double dx = -bbox->get_min_x() + 5;
  double dy = -bbox->get_min_y() + 5;
  unsigned w = vnl_math_ceil(bbox->width() + 25);
  unsigned h = vnl_math_ceil(bbox->height() + 60); //
  xgraph->translate(dx, dy);
  xgraph->update_bounding_box();

  // Create a storage for the xgraph
  dbsksp_xgraph_storage_sptr xgraph_storage = dbsksp_xgraph_storage_new();
  xgraph_storage->set_xgraph(xgraph);

  // Create a tableau of the xgraph
  
  // display selections
  dbsksp_xgraph_tableau::display_shock_graph_elms_ = true;
  dbsksp_xgraph_tableau::display_shock_node_ = false;
  dbsksp_xgraph_tableau::display_shock_edge_chord_ = false;
  dbsksp_xgraph_tableau::display_shock_edge_curve_ = true;
  dbsksp_xgraph_tableau::display_shock_bnd_ = true;
  dbsksp_xgraph_tableau::display_contact_shock_ = false;
  dbsksp_xgraph_tableau::display_others_ = false;


  dbsksp_xgraph_displayer xgraph_displayer;
  vgui_tableau_sptr xgraph_tab = xgraph_displayer.make_tableau(xgraph_storage);
  
  vcl_string title = "xgraph renderer";

  // Put the image tableau inside a 2D viewer tableau (for zoom, etc).
  vgui_viewer2D_tableau_new viewer(xgraph_tab);

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(viewer);

  vgui_window* win = vgui::produce_window(w, h, title);
  win->get_adaptor()->set_tableau( shell );
  win->set_statusbar(false);
  //win->show();
  //vgui::run();

  // Write tableau to file
  dbgui_utils::render_to_ps(viewer, output_file);
  return EXIT_SUCCESS;
}

