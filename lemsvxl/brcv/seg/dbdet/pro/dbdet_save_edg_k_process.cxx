//This is brcv/seg/dbdet/pro/dbdet_save_edg_k_process.cxx

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vcl_list.h>
#include "dbdet_save_edg_k_process.h"

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/sel/dbdet_curvelet.h>

dbdet_save_edg_k_process::dbdet_save_edg_k_process() : bpro1_process()
{
  if( !parameters()->add( "Output file <filename...>" , "-edgoutput" , bpro1_filepath("","*.edg")))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

vcl_string dbdet_save_edg_k_process::name() 
{
  return "Save K .EDG File";
}

vcl_vector< vcl_string > dbdet_save_edg_k_process::get_input_type() 
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "edge_map");
  return to_return;
}

vcl_vector< vcl_string > dbdet_save_edg_k_process::get_output_type() 
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}

//: Clone the process
bpro1_process*
dbdet_save_edg_k_process::clone() const
{
  return new dbdet_save_edg_k_process(*this);
}


bool dbdet_save_edg_k_process::execute()
{  
  bpro1_filepath output;
  
  //get the input storage class
  dbdet_edgemap_storage_sptr input_edgemap;
  input_edgemap.vertical_cast(input_data_[0][0]);

  dbdet_edgemap_sptr edgemap = input_edgemap->get_edgemap();

  parameters()->get_value( "-edgoutput" , output );
  vcl_string output_file = output.path;

  //save this edge map onto a file
  
  return dbdet_save_edg_k(output_file, edgemap);
}


bool dbdet_save_edg_k_process::dbdet_save_edg_k(vcl_string filename, dbdet_edgemap_sptr edgemap)
{
  //1) If file open fails, return.
  vcl_ofstream outfp(filename.c_str(), vcl_ios::out);

  if (!outfp){
    vcl_cout << " Error opening file  " << filename.c_str() << vcl_endl;
    return false;
  }

  //2) write out the header block
  outfp << "# EDGE_MAP v2.0" << vcl_endl << vcl_endl;
  outfp << "# Format :  [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Strength D2f" << vcl_endl;
  outfp << vcl_endl;
  outfp << "WIDTH=" << edgemap->width() << vcl_endl;
  outfp << "HEIGHT=" << edgemap->height() << vcl_endl;
  outfp << "EDGE_COUNT=" << edgemap->num_edgels()  << vcl_endl;
  outfp << vcl_endl << vcl_endl;

  //save the edgel tokens
  for (unsigned row=0; row<edgemap->edge_cells.rows(); row++){
    for (unsigned col=0; col<edgemap->edge_cells.cols(); col++){
      for (unsigned k=0; k<edgemap->edge_cells(row, col).size(); k++){

        dbdet_edgel* edgel = edgemap->edge_cells(row, col)[k];

        double x = edgel->pt.x();
        double y = edgel->pt.y();

        int ix = col; //preserve the original pixel assignment
        int iy = row;
        
        double idir = edgel->tangent, iconf = edgel->strength;
        double dir= edgel->tangent, conf= edgel->strength, d2f=edgel->deriv;
        outfp << "[" << ix << ", " << iy << "]    " << idir << " " << iconf << "   [" << x << ", " << y << "]   " << dir << " " << conf << " " << d2f ;

        //vcl_list<dbdet_curvelet* >::iterator cv_it = edgel->curvelets.begin();
        //outfp<<" [";
        //for ( ; cv_it!=edgel->curvelets.end(); cv_it++)
        //{
        //    if((*cv_it)->curve_model->type==dbdet_curve_model::CC)
        //    {
        //        dbdet_CC_curve_model* cm = (dbdet_CC_curve_model*)((*cv_it)->curve_model);
        //        double kk = cm->k;
        //        outfp<<kk<<" ";
        //    }
        //}
        //outfp<<"]"<<vcl_endl;
      }
    }
  }

  outfp.close();
  return true;
}
