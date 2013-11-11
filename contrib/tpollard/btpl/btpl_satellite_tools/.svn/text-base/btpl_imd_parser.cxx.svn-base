#ifndef btpl_imd_parser_cxx_
#define btpl_imd_parser_cxx_

#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vul/vul_awk.h>
#include <vnl/vnl_vector.h>

#include "btpl_imd_parser.h"


imd_parser::imd_parser( vcl_string file_name )
{
  vgl_point_2d<double> new_UL(0.0,0.0), new_UR(0.0,0.0), new_LR(0.0,0.0), 
    new_LL(0.0,0.0), new_sun_azel(0.0,0.0), new_cam_azel(0.0,0.0);
        
  vcl_ifstream ifs( file_name.c_str() ); vul_awk awk( ifs );
  while( awk ){
    if( awk.NF() == 0 ) continue;
    vcl_string this_field = awk[0];
    vcl_string this_val; if( awk.NF() >= 3 ) this_val = awk[2];
    if( this_field == "ULLon" )
      new_UL.x() = atof( this_val.c_str() );
    else if( this_field == "ULLat" )
      new_UL.y() = atof( this_val.c_str() );
    else if( this_field == "URLon" )
      new_UR.x() = atof( this_val.c_str() );
    else if( this_field == "URLat" )
      new_UR.y() = atof( this_val.c_str() );
    else if( this_field == "LRLon" )
      new_LR.x() = atof( this_val.c_str() );
    else if( this_field == "LRLat" )
      new_LR.y() = atof( this_val.c_str() );
    else if( this_field == "LLLon" )
      new_LL.x() = atof( this_val.c_str() );
    else if( this_field == "LLLat" )
      new_LL.y() = atof( this_val.c_str() );
    else if( this_field == "meanSunAz" )
      new_sun_azel.x() = atof( this_val.c_str() );
    else if( this_field == "meanSunEl" )
      new_sun_azel.y() = atof( this_val.c_str() );
    else if( this_field == "meanSatAz" )
      new_cam_azel.x() = atof( this_val.c_str() );
    else if( this_field == "meanSatEl" )
      new_cam_azel.y() = atof( this_val.c_str() );      
    ++awk;
  }
        
  region.push_back( new_LL ); region.push_back( new_UL );
  region.push_back( new_UR ); region.push_back( new_LR );

  double az = new_cam_azel.x()*3.1417/180, el = new_cam_azel.y()*3.1417/180;  
  camera_dir.set( cos(el)*cos(az), cos(el)*sin(az), sin(el) );

  az = new_sun_azel.x()*3.1417/180, el = new_sun_azel.y()*3.1417/180;
  sun_dir.set( cos(el)*cos(az), cos(el)*sin(az), sin(el) );
}

#endif // btpl_imd_parser_h_
