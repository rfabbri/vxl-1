// This is dber/pro/dber_edge_db_match_process.cxx

//:
// \file

#include <dber/pro/dber_edge_db_match_process.h>
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
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>

#include <vil/vil_new.h>
#include <vgl/vgl_distance.h>

dber_edge_db_match_process::dber_edge_db_match_process() : bpro1_process()
{
  if ( //!parameters()->add( "width ratio for sigma:" , "-rate" , (double) 0.2f ) ||
       !parameters()->add( "mi threshold for elimination during matching:" , "-threshold" , (double) 0.01f ) ||
       !parameters()->add( "smoothing sigma for the image gradient:" , "-smoothsigma" , (float) 1.0f ) ||
       !parameters()->add( "radius that defines neighborhood of each edgel (sigma square):" , "-radius" , (int) 10 ) ||
       !parameters()->add( "self sim radius for initial elimination:" , "-selfradius" , (double) 0.05f ) ||
       !parameters()->add( "poor affine? (otherwise uses TPS):" , "-tps" , (bool) false ) ||
       !parameters()->add( "skip every n edgel (for now):" , "-skip" , (int) 1 ) ||
       !parameters()->add( "Video file1 <filename...>" , "-image_filename" , bpro1_filepath("","*.dat") ) ||
       !parameters()->add( "Video file2 <filename...>" , "-image_filename2" , bpro1_filepath("","*.dat") ) ||
       !parameters()->add( "frame: " , "-frame" , (int) 0 ) ||
       !parameters()->add( "instance: " , "-instance" , (int) 0 ) ||
       !parameters()->add( "frame2: " , "-frame2" , (int) 0 ) ||
       !parameters()->add( "instance2: " , "-instance2" , (int) 1 ) 
       )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Clone the process
bpro1_process*
dber_edge_db_match_process::clone() const
{
  return new dber_edge_db_match_process(*this);
}

vcl_vector< vcl_string > dber_edge_db_match_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );  // image 1 
  to_return.push_back( "vsol2D" );  // vsol2D 1
  to_return.push_back( "vsol2D" ); // polygon 1
  return to_return;
}

vcl_vector< vcl_string > dber_edge_db_match_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("edge_match");
  to_return.push_back("image");
  return to_return;
}

void read_file(const char *filename, vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> >& frames)
{
 //   filename="D:\\Lockheed_Deliveries\\Nov_03_06\\280-203-303-0-20multi.dat";
    vsl_b_ifstream ifile(filename);
    unsigned numframes=0;
    vsl_b_read(ifile,numframes);
    for(unsigned i=0;i<numframes;i++)
    {
        vcl_vector<dbru_multiple_instance_object_sptr> temp;
        vsl_b_read(ifile,temp);
        vcl_cout << "frame: " << i << " size: " << temp.size() << vcl_endl;
        frames.push_back(temp);
    }
}

bool dber_edge_db_match_process::execute()
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
  double selfradius;
  parameters()->get_value( "-selfradius", selfradius );
  double threshold;
  parameters()->get_value( "-threshold" , threshold );
  bool poor_affine;
  parameters()->get_value( "-tps" , poor_affine );
  
  int frame_no; 
  parameters()->get_value( "-frame" , frame_no );
  int instance_no; 
  parameters()->get_value( "-instance" , instance_no );

  int frame_no2; 
  parameters()->get_value( "-frame2" , frame_no2 );
  int instance_no2; 
  parameters()->get_value( "-instance2" , instance_no2 );


  bpro1_filepath image_path;
  parameters()->get_value( "-image_filename" , image_path );
  vcl_string filename = image_path.path;

  vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> > frames;
  read_file(filename.c_str(), frames);
  vcl_cout << "read: " << frames.size() << " frames\n";

  bpro1_filepath image_path2;
  parameters()->get_value( "-image_filename2" , image_path2 );
  vcl_string filename2 = image_path2.path;

  vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> > frames2;
  read_file(filename2.c_str(), frames2);
  vcl_cout << "read: " << frames2.size() << " frames from the second video\n";


  //2) get input storage classes
  /*vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  vil_image_resource_sptr image_sptr = frame_image->get_image();

  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][1]);

  vidpro1_vsol2D_storage_sptr input_vsol3;
  input_vsol3.vertical_cast(input_data_[0][2]);

  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list3 = input_vsol3->all_data();
  vsol_polygon_2d_sptr poly = vsol_list3[0]->cast_to_region()->cast_to_polygon();
*/
  dber_match matcher;
/*
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol1->all_data();
  vcl_vector<vsol_line_2d_sptr> temp_lines;

  for (unsigned int b = 0 ; b < vsol_list.size() ; b += skip ) 
  {
    //we want line segments
    if( vsol_list[b]->cast_to_curve())
    {
      //LINE
      if( vsol_list[b]->cast_to_curve()->cast_to_line() )
      {
        temp_lines.push_back(vsol_list[b]->cast_to_curve()->cast_to_line());
      }
    }
  }

  //: database instance will be lines2 in the matcher
  vcl_vector<vsol_line_2d_sptr>& lines2 = matcher.get_lines2();

  //mask the edgels using the polygon
  vgl_polygon<double> polyg = bsol_algs::vgl_from_poly(poly);
  int cnt = 0, cnt_elim = 0;
  for (unsigned k=0; k<temp_lines.size(); k++) {
    vsol_line_2d_sptr line = temp_lines[k];
    cnt++;
    if (polyg.contains(line->middle()->get_p()))
      lines2.push_back(line);
    else
      cnt_elim++;
  }
  vcl_cout << "for database set: " << cnt << " edgels in vsol, " << cnt_elim << " are eliminated\n";

  vsol_box_2d_sptr box2 = dber_match::get_box(lines2);
  */

  //: put video edgels into lines1 of the matcher
  dbru_multiple_instance_object_sptr ins;
  if (frame_no < int(frames.size())) {
    if (instance_no < int(frames[frame_no].size()))
      ins = frames[frame_no][instance_no];
    else
      return false;
  } else
    return false;

  dbru_multiple_instance_object_sptr ins2;
  if (frame_no2 < int(frames2.size())) {
    if (instance_no2 < int(frames2[frame_no2].size()))
      ins2 = frames2[frame_no2][instance_no2];
    else
      return false;
  } else
    return false;

  dbru_label_sptr null_label = new dbru_label();
  // translate edgels to chipped image coordinates before matching
  vsol_box_2d_sptr ibox = ins->get_img_box();
  double x_min = ibox->get_min_x();
  double y_min = ibox->get_min_y();
  vgl_vector_2d<double> trans(-x_min, -y_min);
  vcl_vector<vsol_line_2d_sptr> edgels = ins->get_edges();
  for (unsigned i = 0; i<edgels.size(); i++) {
    vsol_line_2d_sptr ll = edgels[i];
    ll->p0()->add_vector(trans);
    ll->p1()->add_vector(trans);
  }

  vcl_cout << "eliminating edgels...\n";
  //eliminate extra edgels within 1 pixel
  vcl_vector<bool> eliminate(edgels.size(), false);
  for (unsigned i = 0; i<edgels.size(); i++) {
    if (eliminate[i]) continue;
    vsol_point_2d_sptr mi = edgels[i]->middle();
    for (unsigned j = i+1; j<edgels.size(); j++) {
      if (eliminate[j]) continue;
      vsol_point_2d_sptr mj = edgels[j]->middle();
      if (vgl_distance(mi->get_p(), mj->get_p()) < selfradius)
        eliminate[j] = true;
    }
  }
  int cnt = 0; 
  vcl_vector<vsol_line_2d_sptr> newedgels;
  for (unsigned i = 0; i<edgels.size(); i++) {
    if (eliminate[i]) {
      cnt++;
      continue;
    }
    newedgels.push_back(edgels[i]);
  }
  vcl_cout << "eliminated: " << cnt << " out of " << edgels.size() << " for self_similarity\n";
  
  vsol_polygon_2d_sptr poly = ins->get_poly();
  for (unsigned i = 0; i<poly->size(); i++) {
    vsol_point_2d_sptr p = poly->vertex(i);
    p->add_vector(trans);
  }
  if (!ins->get_label())
    ins->set_label(null_label.ptr());
  dbru_multiple_instance_object_sptr insnew = new dbru_multiple_instance_object(*ins);
  insnew->set_edges(newedgels);

  // translate edgels to chipped image coordinates before matching
  ibox = ins2->get_img_box();
  x_min = ibox->get_min_x();
  y_min = ibox->get_min_y();
  vgl_vector_2d<double> trans2(-x_min, -y_min);
  vcl_vector<vsol_line_2d_sptr> edgels2 = ins2->get_edges();
  for (unsigned i = 0; i<edgels2.size(); i++) {
    vsol_line_2d_sptr ll = edgels2[i];
    ll->p0()->add_vector(trans2);
    ll->p1()->add_vector(trans2);
  }
  
  //eliminate extra edgels within 1 pixel
  vcl_vector<bool> eliminate2(edgels2.size(), false);
  for (unsigned i = 0; i<edgels2.size(); i++) {
    if (eliminate2[i]) continue;
    vsol_point_2d_sptr mi = edgels2[i]->middle();
    for (unsigned j = i+1; j<edgels2.size(); j++) {
      if (eliminate2[j]) continue;
      vsol_point_2d_sptr mj = edgels2[j]->middle();
      if (vgl_distance(mi->get_p(), mj->get_p()) < selfradius)
        eliminate2[j] = true;
    }
  }
  cnt = 0; 
  vcl_vector<vsol_line_2d_sptr> newedgels2;
  for (unsigned i = 0; i<edgels2.size(); i++) {
    if (eliminate2[i]) {
      cnt++;
      continue;
    }
    newedgels2.push_back(edgels2[i]);
  }
  vcl_cout << "eliminated: " << cnt << " out of " << edgels2.size() << " for self_similarity\n";

  poly = ins2->get_poly();
  for (unsigned i = 0; i<poly->size(); i++) {
    vsol_point_2d_sptr p = poly->vertex(i);
    p->add_vector(trans2);
  }
  if (!ins2->get_label())
    ins2->set_label(null_label.ptr());
  dbru_multiple_instance_object_sptr insnew2 = new dbru_multiple_instance_object(*ins2);
  insnew2->set_edges(newedgels2);

  //: radius is the sigma_square in the current_matching norm,
  //  edge correspondences are found using this norm
  matcher.set_radius(radius);
  matcher.set_lines1(insnew->get_edges());
  matcher.set_poly1(insnew->get_poly());

  vcl_vector<dbinfo_observation_sptr> o_obs;
  for (unsigned i = 0; i<insnew->imgs_size(); i++) {
    vil_image_view<vxl_byte> img = insnew->get_image_i(i);
    vil_image_resource_sptr img_r = vil_new_image_resource_of_view(img);
    dbinfo_observation_sptr obs = new dbinfo_observation(0, img_r, poly, true, true, false);
    o_obs.push_back(obs);
  }

  vul_timer t;
  t.mark();

  vsol_polygon_2d_sptr db_poly = insnew2->get_poly();
  matcher.set_lines2(insnew2->get_edges());
  matcher.set_poly2(db_poly);
  matcher.match_greedy(threshold);

  if (!matcher.find_tps(false)) {
    vcl_cout << "tps could not be found\n";
    return false;
  }
  vcl_vector<dbinfo_observation_sptr> db_o_obs;
  for (unsigned i = 0; i<insnew2->imgs_size(); i++) {
    vil_image_view<vxl_byte> img = insnew2->get_image_i(i);
    vil_image_resource_sptr img_r = vil_new_image_resource_of_view(img);
    dbinfo_observation_sptr obs = new dbinfo_observation(0, img_r, db_poly, true, true, false);
    db_o_obs.push_back(obs);
  }

  double mi = 0;
  for (unsigned i = 0; i < db_o_obs.size(); i++) 
    for (unsigned j = 0; j < o_obs.size(); j++) 
      mi += matcher.find_global_mi(o_obs[j], db_o_obs[i]);    

  vcl_cout << " final mi: " << mi << " in " << t.real()/1000.0f << " seconds.\n";
    
  vil_image_resource_sptr correspondence_im = matcher.get_correspondence_image(); 

  vsol_point_2d_sptr gc1 = dber_utilities::center_of_gravity(edgels);  // video
  vsol_point_2d_sptr gc2 = dber_utilities::center_of_gravity(edgels2);  // database
  vgl_line_2d<double> dir1 = dber_utilities::find_dominant_dir(ins->get_poly(), gc1->x(), gc1->y());
  vgl_line_2d<double> dir2 = dber_utilities::find_dominant_dir(ins2->get_poly(), gc2->x(), gc2->y());
  
  
  // create the output storage class
  dber_edge_match_storage_sptr output_match = dber_edge_match_storage_new();  
  
  output_match->set_lines1(matcher.get_lines1());
  output_match->set_lines2(matcher.get_lines2());
  output_match->set_assignement(matcher.get_assignment());
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

