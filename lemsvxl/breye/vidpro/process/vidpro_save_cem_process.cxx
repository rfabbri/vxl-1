// This is dvidpro/process/vidpro_save_cem_process.h

#include <iostream>
#include <fstream>

#include <vidpro/process/vidpro_save_cem_process.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vnl/vnl_math.h>

vidpro_save_cem_process::vidpro_save_cem_process() : bpro_process()
{
  if( !parameters()->add( "Output file <filename...>" , "-cemoutput" , bpro_filepath("","*.cem") ))
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Clone the process
bpro_process*
vidpro_save_cem_process::clone() const
{
  return new vidpro_save_cem_process(*this);
}


std::vector< std::string > vidpro_save_cem_process::get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}

std::vector< std::string > vidpro_save_cem_process::get_output_type()
{
  std::vector< std::string > to_return;
  to_return.clear();
  return to_return;
}

bool vidpro_save_cem_process::execute()
{
  bpro_filepath output;
  parameters()->get_value( "-cemoutput" , output );
  return saveCEM(output.path);
}

bool vidpro_save_cem_process::saveCEM (std::string filename)
{
  // get input storage class
    vidpro_vsol2D_storage_sptr input_vsol;
  input_vsol.vertical_cast(input_data_[0][0]);

  //1)If file open fails, return.
  std::ofstream outfp(filename.c_str(), std::ios::out);

  if (!outfp){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }

  // output header information
   outfp <<"# CONTOUR_EDGE_MAP : Logical-Linear + Shock_Grouping"<<std::endl;
   outfp <<"# .cem files"<<std::endl;
   outfp <<"#"<<std::endl;
   outfp <<"# Format :"<<std::endl;
   outfp <<"# Each contour block will consist of the following"<<std::endl;
   outfp <<"# [BEGIN CONTOUR]"<<std::endl;
   outfp <<"# EDGE_COUNT=num_of_edges"<<std::endl;
   outfp <<"# [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Sub_Pixel_Conf "<<std::endl;
   outfp <<"# ..."<<std::endl;
   outfp <<"# ..."<<std::endl;
   outfp <<"# [END CONTOUR]"<<std::endl;

  outfp<<std::endl;

  // Note: this count is currently missing
   outfp <<"CONTOUR_COUNT="<< std::endl;
   outfp <<"TOTAL_EDGE_COUNT="<<std::endl;

  // parse through all the vsol classes and save curve objects only
   
  std::vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        outfp <<"[BEGIN CONTOUR]"<<std::endl;
        outfp <<"EDGE_COUNT="<< vsol_list[b]->cast_to_curve()->cast_to_polyline()->size() <<std::endl;
        
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
          //output as subpixel contours
          // [%d, %d]\t%lf\t%lf\t[%lf, %lf]\t%lf\t%lf
          outfp <<" [0, 0]  0.0  0.0  [" << pt->x() << ", " << pt->y() << "]  0.0  0.0"  << std::endl;
        }
        outfp <<"[END CONTOUR]"<<std::endl<<std::endl;
      }
    }
  }
  
  //close file
  outfp.close();
  
  return true;
}
