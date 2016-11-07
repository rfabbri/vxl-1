// This is brl/vidpro1/process/vidpro1_load_video_edgemap_process.cxx



#include <Spatemp/pro/vidpro1_load_video_edgemap_process.h>
#include <vcl_iostream.h>

#include <bpro1/bpro1_parameters.h>

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>



#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/algo/dbdet_load_edg.h>



//: Constructor
vidpro1_load_video_edgemap_process::vidpro1_load_video_edgemap_process() : bpro1_process(), num_frames_(0)
{
    if( !parameters()->add( "Edge Maps file dir <dirname...>" , "-edge_dirname", bpro1_filepath("","*") )
        )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
    num_frames_=0;
}


//: Destructor
vidpro1_load_video_edgemap_process::~vidpro1_load_video_edgemap_process()
{
}


//: Clone the process
bpro1_process*
vidpro1_load_video_edgemap_process::clone() const
{
    return new vidpro1_load_video_edgemap_process(*this);
}


//: Return the name of the process
vcl_string vidpro1_load_video_edgemap_process::name()
{
  return "Load .EDG Video";
}


//: Call the parent function and reset num_frames_
void
vidpro1_load_video_edgemap_process::clear_output(int resize)
{
  num_frames_ = 0;
  bpro1_process::clear_output(resize);  
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
vidpro1_load_video_edgemap_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    // no input type required
    to_return.clear();

    return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
vidpro1_load_video_edgemap_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "edge_map");

    return to_return;
}


//: Returns the number of input frames to this process
int
vidpro1_load_video_edgemap_process::input_frames()
{
    return 0;
}


//: Returns the number of output frames from this process
int
vidpro1_load_video_edgemap_process::output_frames()
{
    return num_frames_;
}


//: Run the process on the current frame
bool
vidpro1_load_video_edgemap_process::execute()
{
    bpro1_filepath video_path;
    parameters()->get_value( "-edge_dirname" , video_path );

    vcl_vector<vcl_string> video_files;
    for(vul_file_iterator fn = video_path.path+"*.edg"; fn; ++fn)
      video_files.push_back(fn());
  
    while(!video_files.empty())
    {
      vcl_string filename=video_files.back();
      video_files.pop_back();
      if(!loadEDG(filename))
          vcl_cerr<<" Could not open edgemap "<<filename<<"\n";
      else
          num_frames_++;
    }

    return true;   
}


//: Finish
bool
vidpro1_load_video_edgemap_process::finish() 
{
    return true;
}

bool vidpro1_load_video_edgemap_process::loadEDG(vcl_string input_file)
{
    bool bSubPixel=true, blines=false, bvsol=false;
    double scale=1;

    // edge_map 
    dbdet_edgemap_sptr edge_map;

    bool retval = dbdet_load_edg(input_file, bSubPixel, scale, edge_map);
    if (!retval)
      return false;

    vcl_cout << "N edgels: " << edge_map->num_edgels() << vcl_endl;
    // create the output storage class
    dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
    output_edgemap->set_edgemap(edge_map);
    output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,output_edgemap));
 
    vcl_cout << "Loaded: " << input_file.c_str() << ".\n";

  return true;
}

