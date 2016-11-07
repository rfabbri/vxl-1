//This is vidpro1/process/dbrl_load_con_process.cxx

#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <pro/dbrl_load_con_process.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

#include <vcl_cstring.h>
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <bsol/bsol_algs.h>

dbrl_load_con_process::dbrl_load_con_process() : bpro1_process(), num_frames_(0)
{
  if( !parameters()->add( "Input file <filename...>" , "-coninput" , bpro1_filepath("","*.con") ) ||
      !parameters()->add( "Scale: " , "-scale" , 0.0f ) ||
      !parameters()->add( "Rotation: " , "-rot" , 0.0f ) ||
      !parameters()->add( "Translation: " , "-trans" , 0.0f )||
      !parameters()->add( "reverse: " , "-rev" , (bool)false ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Clone the process
bpro1_process*
dbrl_load_con_process::clone() const
{
  return new dbrl_load_con_process(*this);
}

vcl_vector< vcl_string > dbrl_load_con_process::get_input_type() 
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}

vcl_vector< vcl_string > dbrl_load_con_process::get_output_type() 
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbrl_id_point_2d" );
  return to_return;
}

bool dbrl_load_con_process::execute()
{
  bpro1_filepath input;
  parameters()->get_value( "-coninput" , input);
  vcl_string input_file_path = input.path;

  float scale=0, rot=0, trans=0;
  bool rev=false;
  parameters()->get_value( "-scale" , scale);
  parameters()->get_value( "-rot" , rot);
  parameters()->get_value( "-trans" , trans);
  parameters()->get_value( "-rev" , rev);

  int num_of_files = 0;

  output_data_.clear();

  // make sure that input_file_path is sane
  if (input_file_path == "") { return false; }

  vcl_cout<<vul_file::dirname(input_file_path);
  // test if fname is a directory
  if (vul_file::is_directory(input_file_path))
  {
    
    vul_file_iterator fn=vul_file::dirname(input_file_path)+"/*.con";
    //vul_file_iterator fn=input_file_path+"/*.con";
    for ( ; fn; ++fn) 
    {
      vcl_string input_file = fn();
  
      dbrl_id_point_2d_storage_sptr new_con = loadCON(input_file, scale, rot, trans);
      output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,new_con));
      num_of_files++;
    }

    //this is the number of frames to be outputted
    num_frames_ = num_of_files;
  }
  else {
    vcl_string input_file = input_file_path;

    dbrl_id_point_2d_storage_sptr new_con = loadCON(input_file, scale, rot, trans);
    output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,new_con));
    num_frames_ = 1;
  }


  // 7-11-2006
  // reversing the con seq. Dongjin Han
  if (rev)

  {
   
    vcl_vector<vcl_vector<bpro1_storage_sptr> > v_copy;
   
    for (vcl_vector<vcl_vector<bpro1_storage_sptr> >::reverse_iterator it = output_data_.rbegin(); it != output_data_.rend( ); it++ ) {

      v_copy.push_back(*it);
    }
    output_data_.clear();
    for (vcl_vector<vcl_vector<bpro1_storage_sptr> >::iterator it = v_copy.begin(); it != v_copy.end( ); it++ ) {

      output_data_.push_back(*it);
    }
    
  }
  return true;
}

dbrl_id_point_2d_storage_sptr dbrl_load_con_process::loadCON (vcl_string filename, float scale, float rot, float trans)
{
  // new vector to store the contours
  //vcl_vector< dbrl_id_point_2d_sptr > contours;
  
  // vector to store the points
  vcl_vector< dbrl_id_point_2d_sptr > points;

  vcl_ifstream infp(filename.c_str(), vcl_ios::in);
  bool isOpen_;

  if (!infp) {
    vcl_cout << " Error opening file  " << filename << vcl_endl;
    return false;
  }

  char lineBuffer[2000]; //200
  infp.getline(lineBuffer,2000);
  if (vcl_strncmp(lineBuffer,"CONTOUR",7)) {
    vcl_cerr << "Invalid File " << filename.c_str() << vcl_endl
             << "Should be CONTOUR " << lineBuffer << vcl_endl;
    return false;
  }

  char openFlag[2000];
  infp.getline(openFlag,2000);
  if (!vcl_strncmp(openFlag,"OPEN",4))
    isOpen_ = true;
  else if (!vcl_strncmp(openFlag,"CLOSE",5))
    isOpen_ = false;
  else{
    vcl_cerr << "Invalid File " << filename.c_str() << vcl_endl
             << "Should be OPEN/CLOSE " << openFlag << vcl_endl;
    return false;
  }

  int i, numOfPoints;
  if (infp.eof()) return false;
  
  
  infp >> numOfPoints;

  
  
  vcl_cout<<numOfPoints<<vcl_endl;
  if (infp.eof()) return false;
  float x,y; 
  for (i=0;i<numOfPoints;i++) {
   
    infp >> x >> y;
    dbrl_id_point_2d_sptr newPt = new dbrl_id_point_2d (x,y,i);
    points.push_back(newPt);
  }

  //close file
  infp.close();

  // create a polyline or a polygon depending on whether the contour
  // file is labelled OPEN or CLOSE
  vgl_h_matrix_2d<double> H;
  H.set_identity();
  if (rot != 0)
    H.set_rotation(double(rot));  // rot is angle in radians
  if (scale != 0)
    H.set_scale(double(scale));
  if (trans != 0)
    H.set_translation(double(trans), double(trans));

  //if (isOpen_) {
  //  vsol_polyline_2d_sptr newContour = new vsol_polyline_2d (points);
  //  // polyline does not have centroid method
  //  // TODO: find its centroid and transform it
  //  contours.push_back(newContour->cast_to_spatial_object());
  //}
  //else {
  //  vsol_polygon_2d_sptr newContour = new vsol_polygon_2d (points);
  //  vsol_point_2d_sptr gc = newContour->centroid();

  //  vsol_polygon_2d_sptr Hpoly = 
  //      bsol_algs::transform_about_point(newContour,gc, H);  
  //  contours.push_back(Hpoly->cast_to_spatial_object());
  //}

  // create the output storage class
  dbrl_id_point_2d_storage_sptr output_point_ids = dbrl_id_point_2d_storage_new();
  output_point_ids->set_id_points(points);
  
  vcl_cout << "Loaded: " << filename.c_str() << ".\n";

  return output_point_ids;
}



