// This is brcv/rec/dbru/pro/dbru_polygon_process.cxx
//:
// \file

#include <dbru/pro/dbru_polygon_process.h>
#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <bvis1/bvis1_macros.h>

#include <vgui/vgui_projection_inspector.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vcl_cstdlib.h> // for rand()
#include <vgui/vgui.h> 
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <dbdet/lvwr/dbdet_lvwr_fit.h>

#define MIN_PERCENTAGE  (70.0f)

dbru_polygon_process::dbru_polygon_process()
{  
  if (
      !parameters()->add( "n: " , "-n" , 2 ) 
      ) {
    vcl_cerr << "ERROR: Adding parameters in dbcvr_mutual_info_process::dbcvr_mutual_info_process()" << vcl_endl;
  }

  //vgui_dialog open_dl("Open file");
  poly_filename_ = "D:\\lockheed_videos\\video391\\poly_2915.txt";
  /*poly_filename.append(".txt");
  static vcl_string regexp = "*.*";
  open_dl.file("Video Object Polynoms Filename: ", regexp, poly_filename);

  open_dl.field("Weight for Laplacian Zero-Crossing", iparams_.weight_z);
  open_dl.field("Weight for Gradient Magnitude", iparams_.weight_g);
  open_dl.field("Weight for Gradient Direction", iparams_.weight_d);
  open_dl.field("Sigma for Gaussian smoothing", iparams_.gauss_sigma);
  open_dl.field("Window width for optimum paths' evaluation", iparams_.window_w);
  open_dl.field("Window height for optimum paths' evaluation", iparams_.window_h);
  open_dl.field("Weight for Path Length penalty", iparams_.weight_l);
  open_dl.field("Normalization factor for path length", iparams_.path_norm);
  open_dl.checkbox("Use canny", iparams_.canny);
  open_dl.checkbox("Use image", iparams_.use_given_image);
  open_dl.field("Weight for costs from canny edges", iparams_.weight_canny);
  open_dl.field("Weight for path length on using canny", iparams_.weight_canny_l);
  
  open_dl.ask();
  */
  

}


//: Clone the process
bpro1_process*
dbru_polygon_process::clone() const
{
  return new dbru_polygon_process(*this);
}

bool dbru_polygon_process::execute()
{
  parameters()->get_value( "-n" , n_ );
  clear_output();
  
  vcl_ifstream fs(poly_filename_.c_str());
  
  if (!fs) {
    vcl_cout << "Problems in opening file: " << poly_filename_ << "\n";
  } else {
    vcl_string dummy;

    fs >> dummy; // VIDEOID:
    if (dummy != "VIDEOID:" && dummy != "FILEID:" && dummy != "VIDEOFILEID:") {
      vcl_cout << "No video id specified in input file!\n";
      video_id_ = 0;
      return false;
    } else {
      fs >> video_id_;
      fs >> dummy;   // NFRAMES:
    }
    
    int frame_cnt;
    fs >> frame_cnt;

    //: initialize polygon vector
    for (int i = 0; i<frame_cnt; i++) {
      vcl_vector<vsol_polygon_2d_sptr> tmp;
      polygons_.push_back(tmp);
    }

    for (int i = 0; i<frame_cnt; i++)  {  // read each frame
      //: currently assuming that for each polygon I have one polygon,
      //  but if we decide to use shadow separately, there might be
      //  more than one in the future.
      fs >> dummy;   // NOBJECTS:   
      fs >> dummy;   // 

      int polygon_cnt;
      fs >> dummy;   // NPOLYS:
      fs >> polygon_cnt;
      for (int j = 0; j<polygon_cnt; j++) {
        fs >> dummy; // NVERTS: 
        int vertex_cnt;
        fs >> vertex_cnt;
        vcl_vector<float> x_corners(vertex_cnt), y_corners(vertex_cnt);
        
        fs >> dummy; // X: 
        for (int k = 0; k<vertex_cnt; k++) 
          fs >> x_corners[k];

        fs >> dummy; // Y: 
        for (int k = 0; k<vertex_cnt; k++) 
          fs >> y_corners[k];

        vcl_vector<vsol_point_2d_sptr> vertices;
        for (int k = 0; k<vertex_cnt; k++) 
          vertices.push_back(new vsol_point_2d(x_corners[k], y_corners[k]));
        vsol_polygon_2d_sptr poly = new vsol_polygon_2d(vertices);        
        polygons_[i].push_back(poly);
      }
    }
  }
  
  fs.close();

  int current_frame = bvis1_manager::instance()->current_frame();

  for (unsigned i = current_frame-n_<0?0:current_frame-n_; i<=current_frame+n_; i++) {
      
      bpro1_storage_sptr input_storage_sptr = bvis1_manager::instance()->repository()->get_data_at("image",i,0);
      vidpro1_image_storage_sptr frame;
      frame.vertical_cast(input_storage_sptr);
      vil_image_resource_sptr image_sptr = frame->get_image();
      if (!image_sptr) {
        vcl_cout << "Image not get!\n";
        return false;
      }

      for (unsigned j = 0; j<polygons_[i].size(); j++) {
      
        vsol_polygon_2d_sptr poly = polygons_[i][j];
        dbdet_lvwr_fit fitter(iparams_, canny_params_);
        vsol_polygon_2d_sptr out_poly;
        bool flag = fitter.initialize(poly, image_sptr, true, true);
        
        if (flag) {
          out_poly = fitter.fit();
          if (out_poly->size() > 0) {
            fitter.initialize(out_poly, image_sptr, true, true);
            out_poly = fitter.fit(true);
          }   
        }

        if (!flag || out_poly->size() <= 0) {
          vcl_cout << "No livewire contour, problems in initialization!!!!!\n";
          continue;
        }

        contours_.push_back(out_poly->cast_to_spatial_object());
        contours_.push_back(poly->cast_to_spatial_object());
      }
    }

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(contours_, "vsol2D0");
  output_data_[0].push_back(output_vsol);
  return true;
}
