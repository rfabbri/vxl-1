//This is dbsk2d/pro/dbmsh3d_isosurface_of_seg3d_process.cxx

//:
// \file


#include "dbmsh3d_isosurface_of_seg3d_process.h"

#include <dbmsh3d/algovtk/dbmsh3d_isosurface.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <dbil/algo/dbil_roi_mask.h>


#include <vsol/vsol_region_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <dbsol/dbsol_file_io.h>
#include <dbdet/xio/dbdet_xio_seg3d_info.h>






// ----------------------------------------------------------------------------
//: Constructor
dbmsh3d_isosurface_of_seg3d_process::
dbmsh3d_isosurface_of_seg3d_process() : bpro1_process()
{
  //
  if( !parameters()->add( "xml file <filename...>" , "-xml_filename", 
    bpro1_filepath("","*") ) || 
    !parameters()->add( "mesh file <filename...>" , "-mesh_filename", 
    bpro1_filepath("","*") ) ||
    !parameters()->add( "Data spacing dx"   , "-dx" , 1.0f ) ||
    !parameters()->add( "Data spacing dy"   , "-dy" , 1.0f ) ||
    !parameters()->add( "Data spacing dz"   , "-dz" , 1.0f ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

// ----------------------------------------------------------------------------
//: Destructor
dbmsh3d_isosurface_of_seg3d_process::
~dbmsh3d_isosurface_of_seg3d_process()
{
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbmsh3d_isosurface_of_seg3d_process::
clone() const
{
  return new dbmsh3d_isosurface_of_seg3d_process(*this);
}

// ----------------------------------------------------------------------------
//: Return the name of the process
vcl_string dbmsh3d_isosurface_of_seg3d_process::
name()
{
  return "isosurface of seg3d_info";
}


// ----------------------------------------------------------------------------
//: Returns the number of input frames to this process
int dbmsh3d_isosurface_of_seg3d_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Returns the number of output frames from this process
int dbmsh3d_isosurface_of_seg3d_process::
output_frames()
{
  return 0;
}



// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbmsh3d_isosurface_of_seg3d_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}


// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbmsh3d_isosurface_of_seg3d_process::
get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}


// ----------------------------------------------------------------------------
//: Run the process on the current frame
bool dbmsh3d_isosurface_of_seg3d_process::
execute()
{
  
  this->clear_output();

  // parse input data

  // xml filename
  bpro1_filepath xml_path;
  this->parameters()->get_value( "-xml_filename" , xml_path );    
  vcl_string xml_filename = xml_path.path;

  // mesh filename
  this->parameters()->get_value( "-mesh_filename" , xml_path );    
  vcl_string mesh_filename = xml_path.path;
  
  // data spacing
  float dx, dy, dz;
  this->parameters()->get_value( "-dx" , dx );    
  this->parameters()->get_value( "-dy" , dy );    
  this->parameters()->get_value( "-dz" , dz );    
  

  dbdet_seg3d_info_sptr seg3d = new dbdet_seg3d_info();

  
  x_read(xml_filename, seg3d);

  // get the max size to cover all the images
  unsigned int ni_max = 0;
  unsigned int nj_max = 0;

  vcl_cout << "Load the image resources to get image dimension\n";
  for (int i=0; i<seg3d->num_frames(); ++i)
  {
    dbdet_seg3d_info_frame frame = seg3d->frame(i);

    // 1) load image
    vcl_string image_filename = seg3d->image_folder() + 
      "/" + frame.image_file;

    vil_image_resource_sptr loaded_image = 
      vil_load_image_resource( image_filename.c_str() );
    if( !loaded_image ) 
    {
      vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
      return false;
    }

    ni_max = (loaded_image->ni() > ni_max) ? loaded_image->ni() : ni_max;
    nj_max = (loaded_image->nj() > nj_max) ? loaded_image->nj() : nj_max;
  }



  vcl_cout << "Loading the contours and convert to 3D image.\n";
  vil3d_image_view<vxl_byte > img3d;
  img3d.set_size(ni_max, nj_max, seg3d->num_frames());
  for (int i=0; i<seg3d->num_frames(); ++i)
  {
    vcl_cout<< "Loading contours for slice 1 ... ";
    vil_image_view<bool > mask;
    mask.set_size(ni_max, nj_max);
    mask.fill(false);

    // load the contours
    dbdet_seg3d_info_frame frame = seg3d->frame(i);
    for (unsigned int k=0; k<frame.contour_file_list.size(); ++k)
    {
      // full filename
      vcl_string contour_filename = seg3d->contour_folder() + 
        "/" + frame.contour_file_list[k];

      // load the contour
      vsol_spatial_object_2d_sptr contour = dbsol_load_con_file(
        contour_filename.c_str());

      if (! contour)
      {
        vcl_cerr << "Failed to load contour file" << contour_filename << vcl_endl;
        return false;
      }

      // convert the contour into a polygon
      vsol_polygon_2d_sptr poly;
     
      if (contour->cast_to_region() && contour->cast_to_region()->cast_to_polygon())
      {
        poly = contour->cast_to_region()->cast_to_polygon();
      }
      else
      {
        assert (contour->cast_to_curve()->cast_to_polyline());
        vsol_polyline_2d_sptr loaded_polyline = contour->cast_to_curve()->cast_to_polyline();
        vcl_vector<vsol_point_2d_sptr > points;
        for (unsigned int i=0; i<loaded_polyline->size(); ++i)
        {
          points.push_back(loaded_polyline->vertex(i));
        }
        poly = new vsol_polygon_2d(points);
      }

      // turn into a vgl_polygon
      vcl_vector<vgl_point_2d<double > > pts;
      for (unsigned m=0; m<poly->size(); ++m)
      {
        pts.push_back(poly->vertex(m)->get_p());
      }

      // scan polygon
      vgl_polygon<double > polygon(pts);
      dbil_roi_mask_add_polygon(mask, polygon);   
    }


    // put the binary image to the 3D volume image
    for (unsigned int wi=0; wi<mask.ni(); ++wi)
    {
      for (unsigned int wj=0; wj<mask.nj(); ++wj)
      {
        img3d(wi, wj, i) = mask(wi, wj) ? 255 : 0 ;
      }
    }
    vcl_cout << "Done. \n";
    int count = 0;
    for (unsigned int wi=0; wi<mask.ni(); ++wi)
    {
      for (unsigned int wj=0; wj<mask.nj(); ++wj)
      {
        count += (mask(wi, wj) ? 1 : 0);
      }
    }
    vcl_cout << "Number of non-zero pixels is = " << count << " .\n";
    
  }


  dbmsh3d_mesh mesh;
  dbmsh3d_isosurface(mesh, img3d, 127, dx, dy, dz);

  dbmsh3d_save_ply(&mesh, mesh_filename.c_str(), false);

  return true;
}


// ----------------------------------------------------------------------------
//: Finish
bool dbmsh3d_isosurface_of_seg3d_process::
finish() 
{
  return true;
}





