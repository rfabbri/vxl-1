// This program reads in name of a folder that contains fish contours (Mumford
// AM282 project) and produce a smoothed signed distance transform of the
// contour. 

#include <iostream>
#include <string>
#include <cmath>
#include <vul/vul_file.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_gauss_filter.h>
#include <dbil/algo/dbil_roi_mask.h>
#include <dbil/algo/dbil_edt.h>



#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_box_2d.h>

#include <dbsol/dbsol_file_io.h>

int main(int argc, char **argv)
{
  if (argc <= 2)
  {
    std::cerr << "Syntax: arg[0] in_foldername out_foldername \n";
    return 0;
  }

  // parse command-line arguments
  std::string in_folder(argv[1]);
  std::string out_folder(argv[2]);

  // print out parsed variables
  std::cout << "Input contour folder = " << in_folder << std::endl;
  std::cout << "Ouput folder name = " << out_folder << std::endl;

  // load the contours
  std::string fish_label = vul_file::strip_directory(in_folder);
  
  // a. upper body
  std::string upb_con_file = fish_label + "_upb-2-0.15.con";
  vsol_polyline_2d_sptr upb = 0;
  std::string fullfile = in_folder + "/" + upb_con_file;
  vsol_spatial_object_2d_sptr obj = dbsol_load_con_file(fullfile.c_str());
  if (obj && obj->cast_to_curve())
  {
    if (obj->cast_to_curve()->cast_to_polyline())
    {
      upb = obj->cast_to_curve()->cast_to_polyline();
    }
  }
  // check validity
  if (!upb)
  {
    std::cerr << "Error loading upper body contour file. Exit now";
    return 0;
  }


  // b. tail
  std::string tail_con_file = fish_label + "_tail-2-0.15.con";
  vsol_polyline_2d_sptr tail = 0;
  fullfile = in_folder + "/" + tail_con_file;
  obj = dbsol_load_con_file(fullfile.c_str());
  if (obj && obj->cast_to_curve())
  {
    if (obj->cast_to_curve()->cast_to_polyline())
    {
      tail = obj->cast_to_curve()->cast_to_polyline();
    }
  }

  // check validity
  if (!tail)
  {
    std::cerr << "Error loading tail contour file. Exit now";
    return 0;
  }

  // b. lower body
  std::string lowb_con_file = fish_label + "_lowb-2-0.15.con";
  vsol_polyline_2d_sptr lowb = 0;
  fullfile = in_folder + "/" + lowb_con_file;
  obj = dbsol_load_con_file(fullfile.c_str());
  if (obj && obj->cast_to_curve())
  {
    if (obj->cast_to_curve()->cast_to_polyline())
    {
      lowb = obj->cast_to_curve()->cast_to_polyline();
    }
  }

  // check validity
  if (!lowb)
  {
    std::cerr << "Error loading lower body contour file. Exit now";
    return 0;
  }

  // Now we've got all the contours loaded. Let's merge them to form a closed
  // contour of the fish.
  std::vector<vsol_point_2d_sptr > pts;
  for (unsigned int i=0; i<upb->size(); ++i)
  {
    pts.push_back(upb->vertex(i));
  }

  for (unsigned int i=0; i<tail->size(); ++i)
  {
    pts.push_back(tail->vertex(i));
  }

  for (unsigned int i=lowb->size(); i>0; --i)
  {
    pts.push_back(lowb->vertex(i-1));
  }

  // form fish contour
  vsol_polygon_2d_sptr fish_polygon = new vsol_polygon_2d(pts);
  

  // now convert this polygon contour to a binary image to compute distance
  // transform

  // translate the fish to put it at the center of an image twice the bounding
  // box of the fish


  fish_polygon->compute_bounding_box();
  double dx = -fish_polygon->get_min_x() + fish_polygon->get_bounding_box()->width()/8;
  double dy = -fish_polygon->get_min_y() + fish_polygon->get_bounding_box()->height()/2;

  vgl_vector_2d<double > translate_v(dx, dy);

  for (unsigned int i=0; i < fish_polygon->size(); ++i)
  {
    fish_polygon->vertex(i)->add_vector(translate_v);
  }

  // write polygon file
  std::string fish_contour_file =  fish_label + ".con";
  fullfile = out_folder + "/" + fish_contour_file;
  if (!dbsol_save_con_file(fullfile.c_str(), fish_polygon))
  {
    std::cerr << "Error writing fish contour file";
    return 0;
  }

  // turn into a vgl_polygon
  std::vector<vgl_point_2d<double > > vgl_pts;
  for (unsigned m=0; m<fish_polygon->size(); ++m)
  {
    vgl_pts.push_back(fish_polygon->vertex(m)->get_p());
  }
  vgl_polygon<double > vgl_polygon(vgl_pts);
  

  fish_polygon->compute_bounding_box();
  // create an empty image that is slightly larger than the fish
  vil_image_view<bool > fish_mask((unsigned int)std::ceil(1.25*fish_polygon->get_bounding_box()->width()),
    (unsigned int)std::ceil(2*fish_polygon->get_bounding_box()->height()));
  fish_mask.fill(false);

  // super-impose the fish polygon on the image
  dbil_roi_mask_add_polygon(fish_mask, vgl_polygon);

  // convert boolean image to <vxl_byte> image because vxl doesn't seem to able
  // to save boolean image of dimension > 1800
  vil_image_view<vxl_byte > fish_mask_byte;
  vil_convert_stretch_range(fish_mask, fish_mask_byte);

  std::string fish_binary_file = fish_label + "_binary.tif";
  fullfile = out_folder + "/" + fish_binary_file;
  // write the image out
  if (!vil_save(fish_mask_byte, fullfile.c_str()))
  {
    std::cerr << "Error saving binary image of fish" << std::endl;
    return 0;
  }
  
  


  // from this polygon, compute signed distance transform
  vil_image_view<unsigned int > fish_mask_unsigned;
  vil_convert_cast(fish_mask_byte, fish_mask_unsigned);

  vil_image_view<float > fish_sedt;
  dbil_edt_signed( fish_mask_unsigned, fish_sedt );

  // smooth distance transform image using gaussian filter
  vil_image_view<float > fish_sedt_smoothed;
  double gauss_sigma = 0.5;
  vil_gauss_filter_5tap_params gauss_params(gauss_sigma);
  vil_gauss_filter_5tap(fish_sedt, fish_sedt_smoothed, gauss_params);

  // write distance transform to an image
  vil_image_view<vxl_byte > fish_sedt_byte;
  vil_convert_stretch_range(fish_sedt_smoothed, fish_sedt_byte);
  std::string fish_sedt_file = fish_label + "_sedt_smoothed.tif";
  fullfile = out_folder + "/" + fish_sedt_file;
  // write the image out
  if (!vil_save(fish_sedt_byte, fullfile.c_str()))
  {
    std::cerr << "Error saving distance transform image of fish" << std::endl;
    return 0;
  }
  




  return 0;
}
