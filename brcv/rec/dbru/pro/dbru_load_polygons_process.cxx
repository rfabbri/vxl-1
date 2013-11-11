//This is vidpro1/process/dbru_load_polygons_process.cxx

#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <dbru/pro/dbru_load_polygons_process.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vcl_cstring.h>
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <dbru/dbru_object.h>


dbru_load_polygons_process::dbru_load_polygons_process() : bpro1_process(), num_frames_(0)
{
  if( !parameters()->add( "Input file <filename...>" , "-coninput" , bpro1_filepath("","*.txt") ) ||
      !parameters()->add( "I have an objects file?" , "-objexists" , (bool)false ) ||
      !parameters()->add( "Object file <filename...>" , "-object" , bpro1_filepath("","*.*") ) ||
      //: if no object file is available, one should find the correct start frame in original video
      //  file by trial and error after loading the video and playing with offset to load the polygons
      //  polygons in the poly file are added to the repository after skipping offset many frames
      !parameters()->add( "number of frames to skip (offset) to start loading polygons: " , "-offset" , (int)0 ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Clone the process
bpro1_process*
dbru_load_polygons_process::clone() const
{
  return new dbru_load_polygons_process(*this);
}

vcl_vector< vcl_string > dbru_load_polygons_process::get_input_type() 
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}

vcl_vector< vcl_string > dbru_load_polygons_process::get_output_type() 
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}

bool dbru_load_polygons_process::execute()
{
  bpro1_filepath input;
  parameters()->get_value( "-coninput" , input);
  vcl_string input_file = input.path;

  // make sure that input_file_path is sane
  if (input_file == "") { return false; }

  int offset;
  parameters()->get_value( "-offset" , offset);

  vcl_vector<vcl_vector< vsol_polygon_2d_sptr > > frame_polys;
  int videoid = read_poly_file(input_file, frame_polys);
  vcl_cout << "videoid: " << videoid << vcl_endl;

  bool objexists;
  parameters()->get_value( "-objexists" , objexists);
  if (objexists) {
    bpro1_filepath oinput;
    parameters()->get_value( "-object" , oinput);
    vcl_string obj_file = oinput.path;
    
    vcl_vector<dbru_object_sptr> objects;
    read_objects_from_file(obj_file.c_str(), objects);
    vcl_cout << objects.size() << " objects in the file\n";

    bool flag = false;
    for (unsigned i = 0; i<frame_polys.size(); i++) {
      
      for (unsigned j = 0; j < frame_polys[i].size(); j++) {
        vsol_polygon_2d_sptr poly = frame_polys[i][j];
        
        for (unsigned k = 0; k < objects.size(); k++) {
          dbru_object_sptr obj = objects[k];
          
          for (unsigned m = 0; m < obj->n_polygons(); m++) {
            vsol_polygon_2d_sptr poly2 = obj->get_polygon(m);
            if (*poly == *poly2) {
              vcl_cout << "The first labeled polygon in poly file is in frame: " << i << " , the original frame number is: " << obj->start_frame()+m << vcl_endl;
              vcl_cout << "For this poly file start loading video from the original frame: " << obj->start_frame()+m-i << vcl_endl;
              flag = true;
              break;
            }
          }
          if (flag) 
            break;
        }

        if (flag)
          break;
        
      }
      if (flag)
        break;
    }

    if (!flag)
      vcl_cout << "No polygon in the objects xml file matched exactly to a polygon in poly file!!\n";
  }

  output_data_.clear();

  vidpro1_vsol2D_storage_sptr output_vsol_empty = vidpro1_vsol2D_storage_new();
  while (offset > 0) {
    output_data_.insert(output_data_.begin(),vcl_vector< bpro1_storage_sptr > (1,output_vsol_empty));
    offset--;
  }

  num_frames_ = 0;
  for (unsigned i = 0; i<frame_polys.size(); i++) {

    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    

    // new vector to store the contours
    vcl_vector< vsol_spatial_object_2d_sptr > contours;
    for (unsigned j = 0; j < frame_polys[i].size(); j++) {
      contours.push_back(frame_polys[i][j]->cast_to_spatial_object());
    }

    output_vsol->add_objects(contours, input_file);
    output_data_.insert(output_data_.begin(),vcl_vector< bpro1_storage_sptr > (1,output_vsol));
    num_frames_++;
  }
  return true;
}

