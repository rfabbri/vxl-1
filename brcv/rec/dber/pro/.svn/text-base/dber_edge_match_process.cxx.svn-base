// This is dber/pro/dber_edge_match_process.cxx

//:
// \file

#include <dber/pro/dber_edge_match_process.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_region_2d.h>
#include <dbinfo/dbinfo_observation.h>
#include <vgl/vgl_polygon.h>
#include <dber/dber_match.h>
#include <dber/dber_utilities.h>
#include <vul/vul_timer.h>
#include <bsol/bsol_algs.h>
#include <dbru/dbru_multiple_instance_object.h>
#include <dbru/dbru_multiple_instance_object_sptr.h>

dber_edge_match_process::dber_edge_match_process() : bpro1_process()
{
  if ( //!parameters()->add( "width ratio for sigma:" , "-rate" , (double) 0.2f ) ||
       !parameters()->add( "mi threshold for elimination during matching:" , "-threshold" , (double) 0.0f ) ||
       !parameters()->add( "smoothing sigma for the image gradient:" , "-smoothsigma" , (float) 1.0f ) ||
       !parameters()->add( "radius that defines neighborhood of each edgel:" , "-radius" , (int) 10 ) ||
       !parameters()->add( "pure affine? (otherwise uses TPS):" , "-tps" , (bool) false ) ||
       !parameters()->add( "skip every n edgel (for now):" , "-skip" , (int) 1 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Clone the process
bpro1_process*
dber_edge_match_process::clone() const
{
  return new dber_edge_match_process(*this);
}

vcl_vector< vcl_string > dber_edge_match_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );  // image 1 
  to_return.push_back( "vsol2D" );  // vsol2D 1
  to_return.push_back( "image" );  // image 2
  to_return.push_back( "vsol2D" );  // vsol2D 2

  to_return.push_back( "vsol2D" ); // polygon 1
  to_return.push_back( "vsol2D" ); // polygon 2
  return to_return;
}

vcl_vector< vcl_string > dber_edge_match_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("edge_match");
  to_return.push_back("image");
  return to_return;
}

bool dber_edge_match_process::execute()
{
  //get parameters
  //double ratio;
  //parameters()->get_value( "-rate" , ratio );
  int skip;
  parameters()->get_value( "-skip", skip );
  float smooth_sigma;
  parameters()->get_value( "-smoothsigma", smooth_sigma );
  int radius;
  parameters()->get_value( "-radius", radius );
  double threshold;
  parameters()->get_value( "-threshold" , threshold );
  bool poor_affine;
  parameters()->get_value( "-tps" , poor_affine );

  //2) get input storage classes
  vidpro1_image_storage_sptr frame_image1, frame_image2;
  frame_image1.vertical_cast(input_data_[0][0]);
  frame_image2.vertical_cast(input_data_[0][2]);

  vil_image_resource_sptr image_sptr1 = frame_image1->get_image();
  vil_image_resource_sptr image_sptr2 = frame_image2->get_image();

  vidpro1_vsol2D_storage_sptr input_vsol1, input_vsol2;
  input_vsol1.vertical_cast(input_data_[0][1]);
  input_vsol2.vertical_cast(input_data_[0][3]);

  vidpro1_vsol2D_storage_sptr input_vsol3, input_vsol4;
  input_vsol3.vertical_cast(input_data_[0][4]);
  input_vsol4.vertical_cast(input_data_[0][5]);

  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list3 = input_vsol3->all_data();
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list4 = input_vsol4->all_data();
  vsol_polygon_2d_sptr poly1 = vsol_list3[0]->cast_to_region()->cast_to_polygon();
  vsol_polygon_2d_sptr poly2 = vsol_list4[0]->cast_to_region()->cast_to_polygon();

  dber_match matcher;

  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol1->all_data();
  vcl_vector<vsol_line_2d_sptr> temp_lines1;

  for (unsigned int b = 0 ; b < vsol_list.size() ; b += skip ) 
  {
    //we want line segments
    if( vsol_list[b]->cast_to_curve())
    {
      //LINE
      if( vsol_list[b]->cast_to_curve()->cast_to_line() )
      {
        temp_lines1.push_back(vsol_list[b]->cast_to_curve()->cast_to_line());
      }
    }
  }

  vcl_vector<vsol_line_2d_sptr> temp_lines2;
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list2 = input_vsol2->all_data();
  for (unsigned int b = 0 ; b < vsol_list2.size() ; b += skip ) 
  {
    //we want line segments
    if( vsol_list2[b]->cast_to_curve())
    {
      //LINE
      if( vsol_list2[b]->cast_to_curve()->cast_to_line() )
      {
        temp_lines2.push_back(vsol_list2[b]->cast_to_curve()->cast_to_line());
      }
    }
  }

  vcl_vector<vsol_line_2d_sptr>& lines1 = matcher.get_lines1();
  // find mean of height and width of data to set the sigma

  //mask the edgels using the polygon
  vgl_polygon<double> polyg = bsol_algs::vgl_from_poly(poly1);
  int cnt = 0, cnt_elim = 0;
  for (unsigned k=0; k<temp_lines1.size(); k++) {
    vsol_line_2d_sptr line = temp_lines1[k];
    cnt++;
    if (polyg.contains(line->middle()->get_p()))
      lines1.push_back(line);
    else
      cnt_elim++;
  }
  vcl_cout << "for set1: " << cnt << " edgels in vsol, " << cnt_elim << " are eliminated\n";

  vsol_box_2d_sptr box1 = dber_utilities::get_box(lines1);

  vcl_vector<vsol_line_2d_sptr>& lines2 = matcher.get_lines2();
  vgl_polygon<double> polyg2 = bsol_algs::vgl_from_poly(poly2);
  cnt = 0; cnt_elim = 0;
  for (unsigned k=0; k<temp_lines2.size(); k++) {
    vsol_line_2d_sptr line = temp_lines2[k];
    cnt++;
    if (polyg2.contains(line->middle()->get_p()))
      lines2.push_back(line);
    else
      cnt_elim++;
  }
  vsol_box_2d_sptr box2 = dber_utilities::get_box(lines2);
  vcl_cout << "for set2: " << cnt << " edgels in vsol, " << cnt_elim << " are eliminated\n";

  vul_timer t;
  t.mark();

  //matcher.set_image1(image_sptr1);
  //matcher.set_image2(image_sptr2);
  matcher.set_poly1(poly1);
  matcher.set_poly2(poly2);
  //matcher.prepare_images(smooth_sigma);
  matcher.set_radius(radius);
  //matcher.set_width_radius((box1->width() + box1->height())/4.0f);
  
  double scale_factor = box1->width()/box2->width();
  matcher.set_scale_factor(scale_factor);
  //matcher.scale_lines(lines2, scale_factor);
  vsol_point_2d_sptr gc1 = dber_utilities::center_of_gravity(lines1);
  vsol_point_2d_sptr gc2 = dber_utilities::center_of_gravity(lines2);
  //matcher.translate_lines(lines2, gc1->x()-gc2->x(), gc1->y()-gc2->y());
  // create the output storage class
  dber_edge_match_storage_sptr output_match = dber_edge_match_storage_new();
  
  vgl_line_2d<double> dir1 = dber_utilities::find_dominant_dir(poly1, gc1->x(), gc1->y());
  vgl_line_2d<double> dir2 = dber_utilities::find_dominant_dir(poly2, gc2->x(), gc2->y());

  vgl_h_matrix_2d<double> H;  H.set_identity(); H.set_rotation(-dir2.slope_radians()+dir2.slope_radians());
  dber_utilities::rotate_lines(lines2, H, gc2->x(), gc2->y());
  
  /*double cost = matcher.match_greedy(threshold);
  //vcl_cout << "cost of matching: " << cost << vcl_endl;

  dbinfo_observation_sptr obs1 = new dbinfo_observation(0, image_sptr1, poly1, true, true, false);
  dbinfo_observation_sptr obs2 = new dbinfo_observation(0, image_sptr2, poly2, true, true, false);

  double mi = matcher.find_global_mi(obs1, obs2, poor_affine);
  vcl_cout << "overall mutual information: " << mi << vcl_endl;
  */
  vil_image_resource_sptr correspondence_im = matcher.get_correspondence_image();

  
  //output_match->set_assignement(matcher.get_assignment());
  output_match->set_lines1(lines1);
  output_match->set_lines2(lines2);
  output_match->set_dominant_dir1(dir1);
  output_match->set_dominant_dir2(dir2);

  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(correspondence_im);

  output_data_.clear();
  output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_match));
  output_data_[0].push_back(output_storage);

  return true;
}

/************ Returning the current-matching norm after scaling and translation
  double support = dber_match::current_norm(lines1, lines2, sigma*sigma);
  vcl_cout << "support before scaling & translation: " << support << " time: " << t.real()/1000.0f << vcl_endl;

  double scale_factor = box1->width()/box2->width();
  matcher.scale_lines(lines2, scale_factor);
  support = dber_match::current_norm(lines1, lines2, sigma*sigma);
  vcl_cout << "support before translation: " << support << " time: " << t.real()/1000.0f << vcl_endl;

  vsol_point_2d_sptr gc1 = dber_match::center_of_gravity(lines1);
  vsol_point_2d_sptr gc2 = dber_match::center_of_gravity(lines2);
  matcher.translate_lines(lines2, gc1->x()-gc2->x(), gc1->y()-gc2->y());
  support = dber_match::current_norm(lines1, lines2, sigma*sigma);
  vcl_cout << "final support: " << support << " time: " << t.real()/1000.0f << vcl_endl;
  */

