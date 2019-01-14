// This is brcv/seg/dbdet/pro/dbdet_lvwr_process.cxx

//:
// \file

#include "dbdet_lvwr_process.h"

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vsol/vsol_point_2d.h>
//#include <vsol/vsol_line_2d.h>
//#include <vsol/vsol_polyline_2d.h>
//#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_digital_curve_2d.h>

#include <dbdet/lvwr/dbdet_lvwr.h>
#include <dbdet/lvwr/dbdet_lvwr_params.h>

#include <vil/vil_convert.h>
#include <vil/vil_crop.h>
//required because osl still uses vil1
#include <vil1/vil1_vil.h>
#include <vil1/vil1_memory_image_of.h>

#include <bdgl/bdgl_curve_algs.h>

#include <vgui/vgui_dialog.h>

dbdet_lvwr_process::dbdet_lvwr_process()
{
  if (!parameters()->add( "Input track file <filename...>" , "-input" , bpro1_filepath("","*.tr") ) ||
      !parameters()->add( "Smoothing sigma: " , "-sigma" , 1.0f) ) 
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}

dbdet_lvwr_process::~dbdet_lvwr_process()
{
}


//: Clone the process
bpro1_process* dbdet_lvwr_process::clone() const
{
  return new dbdet_lvwr_process(*this);
}


std::string dbdet_lvwr_process::name()
{
  return "Recorded Wire";
}

std::vector< std::string > dbdet_lvwr_process::get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "image" );
  return to_return;
}

std::vector< std::string > dbdet_lvwr_process::get_output_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  return to_return;
}

int
dbdet_lvwr_process::input_frames()
{
  return 1;
}

int
dbdet_lvwr_process::output_frames()
{
  return 1;
}


bool
dbdet_lvwr_process::execute()
{
  bpro1_filepath input;
  parameters()->get_value( "-input" , input);
  input_file = input.path;
  std::cout << " input file: " << input_file.c_str() << std::endl;
  
  parameters()->get_value( "-sigma" , sigma);
  std::cout << "sigma: " << sigma << "\n";

  //get_intscissors_params(&iparams, &canny_params);
  
  return true;
}

bool
dbdet_lvwr_process::finish()
{

  char file_name[1000];

  std::ifstream infp(input_file.c_str(), std::ios::in);

  if (!infp) {
    std::cout << " Error opening file  " << input_file << std::endl;
    return false;
  }

  std::string temp;
  float tempf;
  infp >> temp;  // START_FRAME;
  infp >> tempf;

  unsigned frame_cnt;
  infp >> temp;
  infp >> frame_cnt;

  std::cout << " input size: " << input_data_.size() << std::endl;
  std::cout << " frame cnt in track file: " << frame_cnt << std::endl;
  
  if (frame_cnt != input_data_.size()) 
    std::cout << " WARNING: Mismatch!\n";

  int input_cnt; // number of points on the polygon
  infp >> temp;
  infp >> input_cnt;

  // ignore COG's for now
  infp >> temp;
  for (unsigned i = 0; i<frame_cnt; i++) {
    infp >> tempf;  infp >> tempf;
  }

  std::vector<std::vector< float > > inp_x;
  std::vector<std::vector< float > > inp_y;
  for (unsigned i = 0; i<frame_cnt; i++) {
    std::vector<float> tmp1(input_cnt, 0);
    inp_x.push_back(tmp1);
    std::vector<float> tmp2(input_cnt, 0);
    inp_y.push_back(tmp2);
  }

  // X:
  infp >> temp;
  for (unsigned i = 0; i<frame_cnt; i++) {
    for (int j = 0; j<input_cnt; j++) {
      infp >> inp_x[i][j];
    }
  }

  // Y:
  infp >> temp;
  for (unsigned i = 0; i<frame_cnt; i++) {
    for (int j = 0; j<input_cnt; j++) {
      infp >> inp_y[i][j];
    }
  }

  infp.close();

  //determine the name of output files
  std::string output_initial;
  
  output_initial = input_file;
  int ii = output_initial.rfind("_",output_initial.size()); 
  // assuming track files has the extension .tr
  // and name is like: path/pickup1_1227.tr
  std::string frame_no = output_initial.substr(ii+1, 4);
  int fno = atoi(frame_no.c_str());
  std::cout << "frame_no: " << frame_no << std::endl;
  std::cout << "frame_no nmbr: " << fno << std::endl;

  output_initial.erase(ii);
  std::cout << "output_initial: " << output_initial << std::endl;

  

  // parse through all the vsol classes and save point objects only
/*  std::vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();

  std::vector<vsol_point_2d_sptr> inp_pts;
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_point())
      inp_pts.push_back(vsol_list[b]->cast_to_point());
  }

  std::cout << "Number of points: " << inp_pts.size() << std::endl;

  for (unsigned int b = 0 ; b < inp_pts.size() ; b++ )
  {
      std::cout << *(inp_pts[b]) << std::endl;
  }

  vsol_digital_curve_2d cv(inp_pts);
  cv.compute_bounding_box();
  int min_x = cv.get_min_x();
  int min_y = cv.get_min_y();
  int max_x = cv.get_max_x();
  int max_y = cv.get_max_y();
  std::cout << "min_x: " << min_x << " min_y: " << min_y << " max_x: " << max_x << " max_y: " << max_y << std::endl;

  */

  for (unsigned frame = 0; frame<input_data_.size(); frame++) {

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[frame][0]);

  vil_image_resource_sptr image_sptr = frame_image->get_image();

  std::vector<vsol_spatial_object_2d_sptr> out_pts_smt;
  std::vector<vsol_spatial_object_2d_sptr> out_pts;
  process_frame(image_sptr, inp_x[frame], inp_y[frame], out_pts_smt, out_pts, sigma);

  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(out_pts_smt, "output_points");
  output_data_[frame].push_back(output_vsol);

  // polygon points
  vidpro1_vsol2D_storage_sptr output_vsol2 = vidpro1_vsol2D_storage_new();
  output_vsol2->add_objects(out_pts, "output_points");
  output_data_[frame].push_back(output_vsol2);

  sprintf(file_name, "%s_%04d.con", output_initial.c_str(), fno+frame);
  std::cout << "file_name: " << file_name << std::endl;

  std::ofstream ofp(file_name, std::ios::out);

  if (!ofp) {
    std::cout << " Error opening file  " << file_name << std::endl;
    return false;
  }

  ofp << "CONTOUR\nOPEN\n" << out_pts_smt.size() << "\n";
  for (unsigned i = 0; i<out_pts_smt.size(); i++) {
    ofp << (out_pts_smt[i])->cast_to_point()->x() << " " << (out_pts_smt[i])->cast_to_point()->y() << "\n";
  }

  ofp.close();

  }

  return true;
}

bool 
dbdet_lvwr_process::process_frame(vil_image_resource_sptr image_sptr, 
                   std::vector<float> inp_x, std::vector<float> inp_y,  
                   std::vector<vsol_spatial_object_2d_sptr> &out_pts_smt,
                   std::vector<vsol_spatial_object_2d_sptr> &out_pts2, float sigma)
{
  
  vil_image_view< unsigned char > image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );
  vil_image_view< unsigned char > greyscale_view;

  // Convert to greyscale images if needed
  if( image_view.nplanes() == 3 ) {
    vil_convert_planes_to_grey( image_view , greyscale_view );
  }
  else if ( image_view.nplanes() == 1 ) {
    greyscale_view = image_view;
  } else {
    std::cerr << "Returning false. nplanes(): " << image_view.nplanes() << std::endl;
    return false;
  }

  /*unsigned i0 = (unsigned)(min_x - 20 < 0 ? 0 : min_x - 20);
  unsigned j0 = (unsigned)(min_y - 20 < 0 ? 0 : min_y - 20);
  unsigned n_i = (unsigned)(max_x + 20 - i0) ;
  unsigned n_j = (unsigned)(max_y + 20 - j0);
  
  std::cout << "i0: " << i0 << " j0: " << j0 << " n_i: " << n_i << " n_j: " << n_j << std::endl;
  
  vil_image_view< unsigned char > cropped;
  cropped = vil_crop(greyscale_view, i0, n_i, j0, n_j);
  
  if (!cropped)
    std::cout << "In dbdet_lvwr_process - crop failed\n";
  

  
  //vil1_memory_image_of< unsigned char > img = vil1_from_vil_image_view( cropped );

  //vil1_memory_image_of< unsigned char > img = vil1_memory_image_of< unsigned char >
  //                                            (const_cast<unsigned char*>(cropped.top_left_ptr()),
  //                                              cropped.ni(), cropped.nj());
*/
  vil1_memory_image_of< unsigned char > img = vil1_from_vil_image_view( greyscale_view );

  dbdet_lvwr intsciss;

  intsciss.set_params(iparams);
  intsciss.set_canny_params(canny_params);

  float seed_x = inp_x[0];
  float seed_y = inp_y[0];

  std::cout << "seed_x: " << seed_x << " seed_y: " << seed_y << std::endl;

      int seed_x_int = static_cast<int>(std::floor(seed_x+0.5));
      int seed_y_int = static_cast<int>(std::floor(seed_y+0.5));

  intsciss.compute(img, seed_x_int, seed_y_int);
  std::cout << "Paths are computed...\n";

  std::vector<std::pair<int, int> > cor;
  std::vector< vsol_point_2d_sptr > out_pts;

  bool out = false;
  unsigned int b;
  std::vector<std::pair<int, int> >::iterator p;
  for (b = 1 ; b < inp_x.size() ; b++ )
  {
    out = intsciss.get_path((int)std::floor(inp_x[b]+0.5), (int)std::floor(inp_y[b]+0.5), cor);
    if (!out) {   // in the rectangle

      //process cor to see if there is repetition
      for (unsigned int i = 0; i < cor.size(); i++) {
        
        if (cor[i].first < 0) continue;
        
        for (unsigned int j = i+1; j<cor.size(); j++) 
          if (cor[i].first == cor[j].first && cor[i].second == cor[j].second) {
            for (unsigned int d = i+1; d<j+1; d++)
              cor[d].first = -1;
            break;
          }  
      }
      
        for (p = cor.end()-1; p != cor.begin()-1; p--) {
       
          if (p->first < 0) continue;

          vsol_point_2d_sptr pt = new vsol_point_2d(p->second, p->first);
          
          bool exists = false;
          for (unsigned int j = 0; j<out_pts.size(); j++) 
            if (*(out_pts[j]) == *pt) {
              exists = true;
              break;
            }  

          if (!exists)
            out_pts.push_back(pt);
        }

    } else {
      std::cout << "It's out b: " << b << " !!!\n";
    }

    intsciss.compute_directions((int)std::floor(inp_x[b]), (int)std::floor(inp_y[b]));
  }

  out = intsciss.get_path((int)std::floor(inp_x[0]), (int)std::floor(inp_y[0]), cor);
  if (!out) {   // in the rectangle

    //process cor to see if there is repetition
      for (unsigned int i = 0; i < cor.size(); i++) {
        
        if (cor[i].first < 0) continue;
        
        for (unsigned int j = i+1; j<cor.size(); j++) 
          if (cor[i].first == cor[j].first && cor[i].second == cor[j].second) {
            for (unsigned int d = i+1; d<j+1; d++)
              cor[d].first = -1;
            break;
          }  
      }


    for (p = cor.end()-1; p != cor.begin()-1; p--) {

      if (p->first < 0) continue;

      vsol_point_2d_sptr pt = new vsol_point_2d(p->second, p->first);

      bool exists = false;
      for (unsigned int j = 0; j<out_pts.size(); j++) 
        if (*(out_pts[j]) == *pt) {
          exists = true;
          break;
        }  

      if (!exists)
        out_pts.push_back(pt);
    }
   } else {
      std::cout << "It's out!!!\n";
    }

   // smooth
   std::cout << "smoothing the curve...\n";
    std::vector<vgl_point_2d<double> > curve;
    
    for (unsigned i = 0; i<out_pts.size(); i++) {
      curve.push_back(vgl_point_2d<double>((out_pts[i])->x(), 
                                           (out_pts[i])->y()));
    }
  
    bdgl_curve_algs::smooth_curve(curve, (double)sigma);
    curve.erase(curve.begin());
    curve.erase(curve.begin());
    curve.erase(curve.begin());
    curve.erase(curve.end()-1);
    curve.erase(curve.end()-1);
    curve.erase(curve.end()-1);

    for (unsigned i = 0; i<curve.size(); i++) {
      vsol_point_2d_sptr pt = new vsol_point_2d(curve[i].x(), curve[i].y());
      out_pts_smt.push_back(pt->cast_to_spatial_object());
    }

    for (unsigned i = 0; i<inp_x.size(); i++) {
      vsol_point_2d_sptr pt = new vsol_point_2d(inp_x[i], inp_y[i]);
      out_pts2.push_back(pt->cast_to_spatial_object());
    }

    return true;
}


//-----------------------------------------------------------------------------
//: Make and display a dialog box to get Intelligent Scissors parameters.
//-----------------------------------------------------------------------------
bool dbdet_lvwr_process::get_intscissors_params(dbdet_lvwr_params* iparams, osl_canny_ox_params* /*params*/)
{
  vgui_dialog* intscissors_dialog = new vgui_dialog("Intelligent Scissors");
  intscissors_dialog->message("%%%%%% USAGE %%%%%%: Start by left clicking to select initial seed");
  intscissors_dialog->message("As mouse moves outside the rectangle, a new seed is selected automatically, and the rectangle is translated.");
  intscissors_dialog->message("To enforce a new seed, left click again in the rectangle.");
  intscissors_dialog->message("SHIFT + left click deletes the last 10 point portion of the contour.");
  intscissors_dialog->message("SHIFT + e activates edit mode. Select two points on the contour. An euler spiral completes the portion in between automatically.");
  intscissors_dialog->message("SHIFT + e in edit mode turns back to normal mode.");
  intscissors_dialog->message("SHIFT + s smoothes the current contour.");
  intscissors_dialog->message("right click (with or without SHIFT) saves the current contour to the specified file and restarts the tool.");

  //intscissors_dialog->field("Standard deviation (sigma)", params->sigma);
  //intscissors_dialog->field("Max smoothing kernel width", params->max_width);
  //intscissors_dialog->field("Gauss tail", params->gauss_tail);
  //intscissors_dialog->field("Low hysteresis threshold", params->low);
  //intscissors_dialog->field("High hysteresis threshold", params->high);
  //intscissors_dialog->field("Min edge pixel intensity", params->edge_min);
  //intscissors_dialog->field("Min pixels in curve", params->min_length);
  //intscissors_dialog->field("Border size", params->border_size);
  //intscissors_dialog->field("Scale", params->scale);
  //intscissors_dialog->field("Strategy", params->follow_strategy);
  //intscissors_dialog->checkbox("Enable pixel jumping?", params->join_flag);
  //intscissors_dialog->field("Junction option", params->junction_option);
  //intscissors_dialog->checkbox("Verbose?", params->verbose);
  
  //intscissors_dialog->field("Weight for Laplacian Zero-Crossing", iparams->weight_z);
  //intscissors_dialog->field("Weight for Gradient Magnitude", iparams->weight_g);
  //intscissors_dialog->field("Weight for Gradient Direction", iparams->weight_d);
  //intscissors_dialog->field("Sigma for Gaussian smoothing", iparams->gauss_sigma);
  intscissors_dialog->field("Window width for optimum paths' evaluation", iparams->window_w);
  intscissors_dialog->field("Window height for optimum paths' evaluation", iparams->window_h);
  //intscissors_dialog->field("Weight for Path Length penalty", iparams->weight_l);
  //intscissors_dialog->field("Normalization factor for path length", iparams->path_norm);
  intscissors_dialog->checkbox("Use canny", iparams->canny);

  intscissors_dialog->field("Weight for costs from canny edges", iparams->weight_canny);
  intscissors_dialog->field("Weight for path length on using canny", iparams->weight_canny_l);
  

  return intscissors_dialog->ask();
}
