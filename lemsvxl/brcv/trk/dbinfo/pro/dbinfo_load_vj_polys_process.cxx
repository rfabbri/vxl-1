// This is brl/brcv/trk/dbinfo/pro/dbinfo_load_vj_polys_process.cxx

//:
// \file

#include <dbinfo/pro/dbinfo_load_vj_polys_process.h>
#include <vcl_iostream.h>
#include <bpro1/bpro1_storage.h>
#include <bpro1/bpro1_parameters.h>
#include <bvis1/bvis1_manager.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

//: Constructor
dbinfo_load_vj_polys_process::dbinfo_load_vj_polys_process() : bpro1_process(), num_frames_(0)
{
  if( !parameters()->add( "VJ Poly file <filename...>" , "-poly_filename", bpro1_filepath("","*") ))
    {
      vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}


//: Destructor
dbinfo_load_vj_polys_process::~dbinfo_load_vj_polys_process()
{
}


//: Clone the process
bpro1_process*
dbinfo_load_vj_polys_process::clone() const
{
  return new dbinfo_load_vj_polys_process(*this);
}


//: Return the name of the process
vcl_string dbinfo_load_vj_polys_process::name()
{
  return "Load VJ_Polys";
}


//: Call the parent function and reset num_frames_
void
dbinfo_load_vj_polys_process::clear_output(int resize)
{
  num_frames_ = 0;
  bpro1_process::clear_output(resize);  
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbinfo_load_vj_polys_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // no input type required
  to_return.clear();

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbinfo_load_vj_polys_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Returns the number of input frames to this process
int
dbinfo_load_vj_polys_process::input_frames()
{
  return 0;
}


//: Returns the number of output frames from this process
int
dbinfo_load_vj_polys_process::output_frames()
{
  return num_frames_;
}

static bool read_poly_file(vcl_ifstream& str,
                           vcl_vector<vcl_vector<vsol_polygon_2d_sptr> >& polys)
{
  polys.clear();
  unsigned nframes = 0, nobj = 0, npoly = 0, nverts = 0, frame = 0;
  char buff[1000];
  vcl_string name;

  str.getline(buff,1000);
    str >> name;
  if(name != "NFRAMES:")
    return false;
  str >> nframes;
  for(unsigned fi = 0; fi<nframes; ++fi)
    {
      str >> name;
      if(name != "NOBJECTS:")
        return false;
      str >> nobj;
      str >> name;
      if(name != "NPOLYS:")
        return false;
      str >> npoly;
      vcl_vector<vsol_polygon_2d_sptr> fpolys;
      for(unsigned ip = 0; ip<npoly; ++ip)
        {
          str >> name;
          if(name != "NVERTS:")
            return false;
          str >> nverts;
          vcl_vector<double> x(nverts), y(nverts);
          str >> name;
          if(name != "X:")
            return false;
          for(unsigned iv = 0; iv<nverts; ++iv)
            str >> x[iv];
          str >> name;
          if(name != "Y:")
            return false;
          for(unsigned iv = 0; iv<nverts; ++iv)
            str >> y[iv];
          vcl_vector<vsol_point_2d_sptr> poly_verts;
          for(unsigned iv = 0; iv<nverts; ++iv)
            {
              vsol_point_2d_sptr p = new vsol_point_2d(x[iv],y[iv]);
              poly_verts.push_back(p);
            }
          vsol_polygon_2d_sptr poly = new vsol_polygon_2d(poly_verts);
          fpolys.push_back(poly);
        }
      polys.push_back(fpolys);
    }
 return true;
}
//: Run the process on the current frame
bool
dbinfo_load_vj_polys_process::execute()
{
  // read the polygons from the vj file
  bpro1_filepath poly_path;
  parameters()->get_value( "-poly_filename" , poly_path );
  vcl_string path = poly_path.path;
  vcl_ifstream is(path.c_str());
  if(!is.is_open())
    {
      vcl_cout << "Failed to open poly file\n";
      return  false;
    }
  vcl_vector<vcl_vector<vsol_polygon_2d_sptr> > polys;
  if(!read_poly_file(is, polys))
    return false;
  //for now assume the start frame is frame 0 - not general enough 
  //but will later have an approapriate storage class for the polys
  num_frames_ = polys.size();
  //construct the storage for each frame
  //each track at a given frame is inserted into the storage element 
  //constructed for the frame
  for(int frame = num_frames_-1; frame>=0; frame--)
    {
      //the storage element
      vidpro1_vsol2D_storage* str = new vidpro1_vsol2D_storage();
      //insert the polys for this frame
      vcl_vector<vsol_polygon_2d_sptr> fpolys = polys[frame];
      for(vcl_vector<vsol_polygon_2d_sptr>::iterator pit = fpolys.begin();
          pit != fpolys.end(); ++pit)
        {
          vsol_region_2d* r = (*pit)->cast_to_region();
          vsol_spatial_object_2d_sptr so = r->cast_to_spatial_object();
          str->add_object(so,"vjpoly");
        }
      vcl_vector< bpro1_storage_sptr > stores;
      stores.push_back(str);
      //save the vsol2D tracks into the output data for this frame
      output_data_.push_back(stores);
    }
  return true;
}

//: Finish
bool
dbinfo_load_vj_polys_process::finish() 
{
  return true;
}



