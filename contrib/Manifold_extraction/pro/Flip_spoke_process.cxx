/*************************************************************************
 *    FILE: Flip_spoke_process.cxx
 *    DATE: Mon 17 September 2007
 *************************************************************************/


#include "Flip_spoke_process.h"
#include <manifold_extraction/Lie_spoke_utilities.h>

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>


#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <dbgl/dbgl_eno_curve.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>

Flip_spoke_process::Flip_spoke_process()
{  
 if (!parameters()->add( "Save contour file <filename...>" , "-filename", bpro1_filepath("","*")))
         {
    vcl_cerr << "ERROR: Adding parameters in Flip_spoke_process::Flip_spoke_process()" << vcl_endl;
}
}

//: Clone the process
bpro1_process*
Flip_spoke_process::clone() const
{
  return new Flip_spoke_process(*this);
}



/*************************************************************************
 * Function Name: Flip_spoke_process::execute
 * Parameters:
 * Returns: bool
 * Effects:
 *************************************************************************/
//: curve1 in input_spatial_object_[0], build curve from polyline (digital curve)
//: curve2 in input_spatial_object_[1]
bool Flip_spoke_process::execute()
{
  clear_output();
  bpro1_filepath file_dir;

   parameters()->get_value( "-filename" ,file_dir);
    vcl_string fpath = file_dir.path;

  ////----------------------------------
  //// get input vsol (two polygons)
  ////----------------------------------
  //vidpro1_vsol2D_storage_sptr input_vsol1;
  //input_vsol1.vertical_cast(input_data_[0][0]);

  //// The contour needs to be a polygon
  //vsol_polyline_2d_sptr poly1;
  //{
  //  const vcl_vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol1->all_data();
  //  poly1 = vsol_list[0]->cast_to_curve()->cast_to_polyline();
 
  //}
      vcl_vector<vcl_string> file_names = get_all_files(fpath);
      vcl_vector<vsol_point_2d_sptr> vertex_set,vertex_set_rev;

      for (unsigned int file_num = 0;file_num < file_names.size();file_num++)
          {
          vertex_set.clear();
          vertex_set_rev.clear();
          
          vcl_string fname = file_names[file_num];

           loadCON(fname,vertex_set);

           unsigned int num_vert = vertex_set.size();

          // vertex_set_rev.set_size(num_vert);

           for (unsigned int i=0;i<num_vert;i++)
               {
               vsol_point_2d_sptr pt = new vsol_point_2d();
               pt->set_x(-1*vertex_set[num_vert-1-i]->x());
               pt->set_y(vertex_set[num_vert-1-i]->y());

               vertex_set_rev.push_back(pt);
               }

        fname.erase(fname.size()-4,fname.size());
        vcl_string ext = "_flipped.con";
        fname.append(ext);
        writeCON(fname,vertex_set_rev);

      }

  // The contour can either be a polyline producing an open contour 
  // or a polygon producing a close contour
  vcl_vector< vsol_spatial_object_2d_sptr > flipped_contour;
  vsol_polyline_2d_sptr newpolyline = new vsol_polyline_2d (vertex_set_rev);
  flipped_contour.push_back(newpolyline->cast_to_spatial_object());
 

 vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
 output_vsol->add_objects(flipped_contour);
 

   output_data_[0].push_back(output_vsol);

  return true;
}


