#include "imgr_skyscan_reconlog.h"
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vnl/vnl_quaternion.h>
#include <xscan/xscan_uniform_orbit.h>
#include <xscan/xscan_orbit_base_sptr.h>


#define IMGR_SKYSCAN_RECONLOG_DEBUG 0 

#if 0
imgr_skyscan_reconlog::imgr_skyscan_reconlog(char const* fname)
{
  vcl_ifstream fs(fname);
  if(!fs)
  {
    vcl_cerr << "file open failed for reading skyscan reconstructed image header\n";
    exit(1);
  }

  is_valid_ = false;

  vcl_string tmp;
  //unused variable
  //char junk[256];

  // read magic words from line 5
  vcl_string check;
  vcl_string sub_check;
  for(int i=0; i<5; i++)
    getline(fs, check);
  sub_check = check.substr(0,31);

  if( sub_check == "Software=Version 1. 3 (build 2)"
      || sub_check == "Software=Version 1. 4 (build 3)"
      || sub_check == "Software=Version 1. 4 (build 3)"
      
      )
  {
    is_valid_ = true;

    vcl_istringstream iss;

    // get camera pixel size
    for(int i=0; i<4; i++)
      getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> camera_pixel_size_u_;

    
    
    fs.ignore(256, '=');
   

   
    double u_div_v ;
   
    fs >> u_div_v;
    
    camera_pixel_size_v_ = camera_pixel_size_u_ / u_div_v;




     for(int i=0; i<8; i++)
      getline(fs, tmp);

// get number of rows in the image from the dataset
    fs.ignore(256, '=');
    fs >> sv_;

    for(int i=0; i<3; i++)
      getline(fs, tmp);

// get source rotation center distance    
    fs.ignore(256, '=');
    fs >> src_rot_dist_;
   

    // get source sensor distance
   
    fs.ignore(265, '=');
    fs >> src_snsr_dist_;
    
    // get principle point
   
    fs.ignore(256, '=');
    fs >> v0_;
    
    // get starting position in z axis
    for(int i=0; i<26; i++)
      getline(fs, tmp);


    

    fs.ignore(256, '=');

    fs >> start_slice_;

    // get end slice number
    
    fs.ignore(256, '=');
    fs >> end_slice_;

    // get slice step
    if(sub_check == "Software=Version 1. 3 (build 2)"
                    || sub_check == "Software=Version 1. 4 (build 3)"
                    )
    {
      getline(fs, tmp); //skip reconstruction time
      getline(fs, tmp); // skip post alignment
      getline(fs,tmp);
    }
    
   
    fs.ignore(256, '=');
    fs >> slice_step_;


    //
    for(int i=0; i<4; i++)
      getline(fs, tmp);
  
    fs.ignore(256, '=');
    fs >> size_x_;

    
    fs.ignore(256, '=');
    fs >> size_y_;

    // going to voxel size
    
    fs.ignore(256, '=');
    fs >> voxel_size_;

    //correcting the effect of merging sensor pixels
    double multiplier = 1;
    if(voxel_size_ > 9. && voxel_size_ < 27 )
      multiplier = 2;
    
    if(voxel_size_ >= 27)
      multiplier = 4;

    camera_pixel_size_u_ *= multiplier;
    camera_pixel_size_v_ *= multiplier;
  }


  if (sub_check == "Software=Version 2. 5 (build 9)" )
      {
      is_valid_ = true;

    vcl_istringstream iss;

    // get camera pixel size
    for(int i=0; i<4; i++)
      getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> camera_pixel_size_u_;

    
    
    fs.ignore(256, '=');
   
// get the camera xy ratio
   
    double u_div_v ;
   
    fs >> u_div_v;
    
    camera_pixel_size_v_ = camera_pixel_size_u_ / u_div_v;




     for(int i=0; i<7; i++)
      getline(fs, tmp);

// get the object to source distance
    fs.ignore(256, '=');
    fs >> src_rot_dist_;

//get the source sensor distance
     fs.ignore(265, '=');
    fs >> src_snsr_dist_;

    
      getline(fs, tmp);
      getline(fs,tmp);
// get the number of rows
  
    fs.ignore(256, '=');
    fs >> sv_;

    getline(fs,tmp);
    getline(fs,tmp);
    
    // get principle point
   
    fs.ignore(256, '=');
    fs >> v0_;
  
    for(int i=0; i<30; i++)
      getline(fs, tmp);
  
    // get start slice 
    fs.ignore(256, '=');
    fs >> start_slice_;

    // get end slice
    
    fs.ignore(256, '=');
    fs >> end_slice_;

    // get slice step

       for(int i=0; i<3; i++)
      getline(fs, tmp);

       fs.ignore(256,'=');
    fs >> slice_step_;

    // going to size_x
    for(int i=0; i<4; i++)
      getline(fs, tmp);
    
    fs.ignore(256, '=');
    fs >> size_x_;

    //going to size_y
    fs.ignore(256, '=');
    fs >> size_y_;

    //going to voxel size
    fs.ignore(256,'=');
    fs>> voxel_size_;


    //correcting the effect of merging sensor pixels
    double multiplier = 1;
    if(voxel_size_ > 9. && voxel_size_ < 27 )
      multiplier = 2;
    
    if(voxel_size_ >= 27)
      multiplier = 4;

    camera_pixel_size_u_ *= multiplier;
    camera_pixel_size_v_ *= multiplier;

}


    if(!is_valid_){
            vcl_cerr << "Unable to find valid rule for parsing software version\n";
            vcl_cerr << sub_check <<  "\n";
    }

}
#endif
imgr_skyscan_reconlog::imgr_skyscan_reconlog(char const* fname)
{
  vcl_ifstream fs(fname);
  if(!fs)
  {
    vcl_cerr << "file open failed for reading skyscan reconstructed image header\n";
    exit(1);
  }

  is_valid_ = false;

  vcl_string tmp;
  //char junk[256];

  // read magic words from line 5
  vcl_string check;

  for(int i=0; i<5; i++)
    getline(fs, check);

  // in order to work for both windows & Linux
  vcl_string sub_check = check.substr(0,31);
  vcl_cerr << sub_check << "\n";

  //if( check == "Software=Version 2. 5 (build 9)\r" 
  //    || check == "Software=Version 1. 3 (build 2)\r"
  //    || check == "Software=Version 1. 2 (build 1)\r" )
    if( sub_check == "Software=Version 1. 3 (build 2)"
      || sub_check == "Software=Version 1. 3 (build 3)" 
      || sub_check == "software=Version 1. 2 (build 1)"
      || sub_check == "Software=Version 1. 4 (build 3)"
      )
  {
    is_valid_ = true;

    vcl_istringstream iss;

//first reading all the values from the log file 

    // get camera pixel size
    for(int i=0; i<4; i++){
      getline(fs, tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> camera_pixel_size_u_;
   
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " camera_pixel_size_u_ = " << camera_pixel_size_u_ << "\n"; 
    #endif

    fs.ignore(256, '=');
    double u_div_v ;
    fs >> u_div_v;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " u_div_v = " <<u_div_v << "\n"; 
    #endif
    camera_pixel_size_v_ = camera_pixel_size_u_ / u_div_v;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " camera_pixel_size_v_ = " << camera_pixel_size_v_ << "\n"; 
    #endif

    // get number of rows in the image from the dataset
    
    for(int i=0; i<8; i++){
      getline(fs, tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }

    fs.ignore(256, '=');
    fs >> sv_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " sv_ = " << sv_ << "\n"; 
    #endif
    for(int i=0; i<3; i++){
      getline(fs, tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }

// get source rotation center distance

    fs.ignore(256, '=');
    fs >> src_rot_dist_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " src_rot_dist_ = " << src_rot_dist_ << "\n"; 
    #endif
    // get source sensor distance
    
    fs.ignore(265, '=');
    fs >> src_snsr_dist_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " src_snsr_dist_ = " << src_snsr_dist_ << "\n"; 
    #endif
    
    // get principle point
    
    fs.ignore(256, '=');
    fs >> v0_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " v0_ = " <<v0_ << "\n"; 
    #endif
    // get starting position in z axis
    for(int i=0; i<26; i++){
      getline(fs, tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }


   

    fs.ignore(256, '=');

    fs >> start_slice_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " start_slice_ = " <<start_slice_ << "\n"; 
    #endif

    // get end slice number
    
    fs.ignore(256, '=');
    fs >> end_slice_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "end_slice_ = " <<end_slice_ << "\n"; 
    #endif

    // get slice step
    if(sub_check == "Software=Version 1. 3 (build 2)"
        ||sub_check == "Software=Version 1. 3 (build 3)"
        || sub_check == "Software=Version 1. 4 (build 3)"
        )
    {
      getline(fs, tmp); //skip reconstruction time
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
      getline(fs, tmp); // skip post alignment
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
      getline(fs,tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }

    
    
    
    fs.ignore(256, '=');
    fs >> slice_step_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "slice_step_ = " <<slice_step_ << "\n"; 
    #endif


    //
    for(int i=0; i<4; i++){
      getline(fs, tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }
   
    fs.ignore(256, '=');
    fs >> size_x_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "size_x_ = " <<size_x_ << "\n"; 
    #endif

    
    fs.ignore(256, '=');
    fs >> size_y_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "size_y_ = " <<size_y_ << "\n"; 
    #endif

    // going to voxel size
   
   fs.ignore(256, '=');
    fs >> voxel_size_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "voxel_size_ = " << voxel_size_ << "\n"; 
    #endif

    //correcting the effect of merging sensor pixels
    double multiplier = 1;
    if(voxel_size_ > 9. && voxel_size_ < 27 )
      multiplier = 2;
    
    if(voxel_size_ >= 27)
      multiplier = 4;

    camera_pixel_size_u_ *= multiplier;
    camera_pixel_size_v_ *= multiplier;

    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "multiplier-adjusted  camera_pixel_size_u_ = " << camera_pixel_size_u_ << "\n"; 
    #endif
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "multiplier-adjusted camera_pixel_size_v_ = " << camera_pixel_size_v_ << "\n"; 
    #endif
  }

    if(!is_valid_){
            vcl_cerr << "Unable to find valid rule for parsing software version\n";
            vcl_cerr << sub_check <<  "\n";
    }

}

imgr_skyscan_reconlog::imgr_skyscan_reconlog(char const* fname,xscan_scan & scan)
{
  vcl_ifstream fs(fname);
  if(!fs)
  {
    vcl_cerr << "file open failed for reading skyscan reconstructed image header\n";
    exit(1);
  }

  is_valid_ = false;

  vcl_string tmp;
  //char junk[256];

  // read magic words from line 5
  vcl_string check;

  for(int i=0; i<5; i++)
    getline(fs, check);

  // in order to work for both windows & Linux
  vcl_string sub_check = check.substr(0,31);
  vcl_cerr << sub_check << "\n";

  //if( check == "Software=Version 2. 5 (build 9)\r" 
  //    || check == "Software=Version 1. 3 (build 2)\r"
  //    || check == "Software=Version 1. 2 (build 1)\r" )
    if( sub_check == "Software=Version 1. 3 (build 2)"
      || sub_check == "Software=Version 1. 3 (build 3)" 
      || sub_check == "software=Version 1. 2 (build 1)"
      || sub_check == "Software=Version 1. 4 (build 3)"
      )
  {
    is_valid_ = true;

    vcl_istringstream iss;

//first reading all the values from the log file 

    // get camera pixel size
    for(int i=0; i<4; i++){
      getline(fs, tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> camera_pixel_size_u_;
   
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " camera_pixel_size_u_ = " << camera_pixel_size_u_ << "\n"; 
    #endif

    fs.ignore(256, '=');
    double u_div_v ;
    fs >> u_div_v;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " u_div_v = " <<u_div_v << "\n"; 
    #endif
    camera_pixel_size_v_ = camera_pixel_size_u_ / u_div_v;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " camera_pixel_size_v_ = " << camera_pixel_size_v_ << "\n"; 
    #endif

    // get number of rows in the image from the dataset
    
    for(int i=0; i<8; i++){
      getline(fs, tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }

    fs.ignore(256, '=');
    fs >> sv_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " sv_ = " << sv_ << "\n"; 
    #endif
    for(int i=0; i<3; i++){
      getline(fs, tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }

// get source rotation center distance

    fs.ignore(256, '=');
    fs >> src_rot_dist_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " src_rot_dist_ = " << src_rot_dist_ << "\n"; 
    #endif
    // get source sensor distance
    
    fs.ignore(265, '=');
    fs >> src_snsr_dist_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " src_snsr_dist_ = " << src_snsr_dist_ << "\n"; 
    #endif
    
    // get principle point
    
    fs.ignore(256, '=');
    fs >> v0_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " v0_ = " <<v0_ << "\n"; 
    #endif
    // get starting position in z axis
    for(int i=0; i<26; i++){
      getline(fs, tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }


   

    fs.ignore(256, '=');

    fs >> start_slice_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << " start_slice_ = " <<start_slice_ << "\n"; 
    #endif

    // get end slice number
    
    fs.ignore(256, '=');
    fs >> end_slice_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "end_slice_ = " <<end_slice_ << "\n"; 
    #endif

    // get slice step
    if(sub_check == "Software=Version 1. 3 (build 2)"
        ||sub_check == "Software=Version 1. 3 (build 3)"
        || sub_check == "Software=Version 1. 4 (build 3)"
        )
    {
      getline(fs, tmp); //skip reconstruction time
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
      getline(fs, tmp); // skip post alignment
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
      getline(fs,tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }

    
    
    
    fs.ignore(256, '=');
    fs >> slice_step_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "slice_step_ = " <<slice_step_ << "\n"; 
    #endif


    //
    for(int i=0; i<4; i++){
      getline(fs, tmp);
      #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "\t" << tmp << "\n"; 
    #endif
    }
   
    fs.ignore(256, '=');
    fs >> size_x_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "size_x_ = " <<size_x_ << "\n"; 
    #endif

    
    fs.ignore(256, '=');
    fs >> size_y_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "size_y_ = " <<size_y_ << "\n"; 
    #endif

    // going to voxel size
   
   fs.ignore(256, '=');
    fs >> voxel_size_;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "voxel_size_ = " << voxel_size_ << "\n"; 
    #endif

    //correcting the effect of merging sensor pixels
    double multiplier = 1;
    if(voxel_size_ > 9. && voxel_size_ < 27 )
      multiplier = 2;
    
    if(voxel_size_ >= 27)
      multiplier = 4;

    camera_pixel_size_u_ *= multiplier;
    camera_pixel_size_v_ *= multiplier;

    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "multiplier-adjusted  camera_pixel_size_u_ = " << camera_pixel_size_u_ << "\n"; 
    #endif
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "multiplier-adjusted camera_pixel_size_v_ = " << camera_pixel_size_v_ << "\n"; 
    #endif
    // changing some of the values based on the scan file values


   vpgl_calibration_matrix<double> kk(scan.kk());

    xscan_orbit_base_sptr orbit_base = scan.orbit();
  xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
  
   vnl_quaternion<double>turn_table_tr(orbit.t0());
  src_rot_dist_ = turn_table_tr.z();
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "scan-adjusted src_rot_dist_ = " << src_rot_dist_ << "\n"; 
    #endif

 vgl_point_2d<double> princp_pt(kk.principal_point());
 v0_ = princp_pt.y();
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "scan-adjusted v0_ = " << v0_ << "\n"; 
    #endif

 camera_pixel_size_u_ =  (src_snsr_dist_/kk.x_scale())*1e3;

 camera_pixel_size_v_ =  (src_snsr_dist_/kk.y_scale())*1e3;
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "scan-adjusted  camera_pixel_size_u_ = " << camera_pixel_size_u_ << "\n"; 
    #endif
    #if IMGR_SKYSCAN_RECONLOG_DEBUG 
    vcl_cout << "scan-adjusted camera_pixel_size_v_ = " << camera_pixel_size_v_ << "\n"; 
    #endif



//u_div_v = kk.x_scale()/kk.y_scale();
//camera_pixel_size_v_ = camera_pixel_size_u_/u_div_v;



  }


 if (sub_check == "Software=Version 2. 5 (build 9)" )
      {
      is_valid_ = true;

    vcl_istringstream iss;

    // get camera pixel size
    for(int i=0; i<4; i++)
      getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> camera_pixel_size_u_;

    
    
    fs.ignore(256, '=');
   
// get the camera xy ratio
   
    double u_div_v ;
   
    fs >> u_div_v;
    
    camera_pixel_size_v_ = camera_pixel_size_u_ / u_div_v;




     for(int i=0; i<7; i++)
      getline(fs, tmp);

// get the object to source distance
    fs.ignore(256, '=');
    fs >> src_rot_dist_;

//get the source sensor distance
     fs.ignore(265, '=');
    fs >> src_snsr_dist_;

    
      getline(fs, tmp);
      getline(fs,tmp);
// get the number of rows
  
    fs.ignore(256, '=');
    fs >> sv_;

    getline(fs,tmp);
    getline(fs,tmp);
    
    // get principle point
   
    fs.ignore(256, '=');
    fs >> v0_;
  
    for(int i=0; i<30; i++)
      getline(fs, tmp);
  
    // get start slice 
    fs.ignore(256, '=');
    fs >> start_slice_;

    // get end slice
    
    fs.ignore(256, '=');
    fs >> end_slice_;

    // get slice step

       for(int i=0; i<3; i++)
      getline(fs, tmp);

       fs.ignore(256,'=');
    fs >> slice_step_;

    // going to size_x
    for(int i=0; i<4; i++)
      getline(fs, tmp);
    
    fs.ignore(256, '=');
    fs >> size_x_;

    //going to size_y
    fs.ignore(256, '=');
    fs >> size_y_;

    //going to voxel size
    fs.ignore(256,'=');
    fs>> voxel_size_;


    //correcting the effect of merging sensor pixels
    double multiplier = 1;
    if(voxel_size_ > 9. && voxel_size_ < 27 )
      multiplier = 2;
    
    if(voxel_size_ >= 27)
      multiplier = 4;

    camera_pixel_size_u_ *= multiplier;
    camera_pixel_size_v_ *= multiplier;

}

    if(!is_valid_){
            vcl_cerr << "Unable to find valid rule for parsing software version\n";
            vcl_cerr << sub_check <<  "\n";
    }

}

vgl_point_3d<double> imgr_skyscan_reconlog::fbpc_to_bsc(vgl_point_3d<double> const& pt)
{
  double x = voxel_size_ * (pt.x() - size_x_ / 2.) * 0.001;
  double y = voxel_size_ * (pt.y() - size_y_ / 2.) * 0.001;
  double z = src_rot_dist_/src_snsr_dist_*camera_pixel_size_v_ * (pt.z() - (sv_ - v0_ )) * 0.001;
  
  return vgl_point_3d<double>(x, y, z);
}

vgl_point_3d<double> imgr_skyscan_reconlog::bsc_to_fbpc(vgl_point_3d<double> const& pt)
{
     #if IMGR_SKYSCAN_RECONLOG_DEBUG 
        vcl_cout << "voxel_size_ " << voxel_size_ << "\n"; 
     #endif
     #if IMGR_SKYSCAN_RECONLOG_DEBUG 
        vcl_cout << "size_x_ " << size_x_ << "\n"; 
     #endif
     #if IMGR_SKYSCAN_RECONLOG_DEBUG 
        vcl_cout << "size_y_ " << size_y_ << "\n"; 
     #endif
     #if IMGR_SKYSCAN_RECONLOG_DEBUG 
        vcl_cout << "src_snsr_dist_ " << src_snsr_dist_ << "\n"; 
     #endif
     #if IMGR_SKYSCAN_RECONLOG_DEBUG 
        vcl_cout << "src_rot_dist_ " << src_rot_dist_ << "\n"; 
     #endif
     #if IMGR_SKYSCAN_RECONLOG_DEBUG 
        vcl_cout << "camera_pixel_size_v_ " << camera_pixel_size_v_ << "\n"; 
     #endif
     #if IMGR_SKYSCAN_RECONLOG_DEBUG 
        vcl_cout << "sv_ " << sv_ << "\n"; 
     #endif
     #if IMGR_SKYSCAN_RECONLOG_DEBUG 
        vcl_cout << "v0_ " << v0_ << "\n"; 
     #endif
  double x = pt.x()*1000./voxel_size_ +  size_x_ / 2.;
  double y = pt.y()*1000./voxel_size_ +  size_y_ / 2.;
  double z = pt.z()*1000.*src_snsr_dist_/src_rot_dist_/camera_pixel_size_v_  + (sv_ - v0_) ;
  
  return vgl_point_3d<double>(x, y, z);
  
}
#if 0

imgr_skyscan_reconlog::imgr_skyscan_reconlog(char const* fname)
{
  vcl_ifstream fs(fname);
  if(!fs)
  {
    vcl_cerr << "file open failed for reading skyscan reconstructed image header\n";
    exit(1);
  }

  is_valid_ = false;

  vcl_string tmp;
  char junk[256];

  // read magic words from line 5
  vcl_string check;
  vcl_string sub_check;
  for(int i=0; i<5; i++)
    getline(fs, check);
  sub_check = check.substr(0,31);

  if( sub_check == "Software=Version 2. 5 (build 9)" 
      || sub_check == "Software=Version 1. 3 (build 2)"
      || sub_check == "Software=Version 1. 2 (build 1)" )
  {
    is_valid_ = true;

    vcl_istringstream iss;

    // get camera pixel size
    for(int i=0; i<4; i++)
      getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> camera_pixel_size_u_;

    
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
   

   
    double u_div_v ;
   
    iss >> u_div_v;
    
    camera_pixel_size_v_ = camera_pixel_size_u_ / u_div_v;

    // get source rotation center distance
    for(int i=0; i<11; i++)
      getline(fs, tmp);

    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> src_rot_dist_;
   

    // get source sensor distance
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(265, '=');
    iss >> src_snsr_dist_;
    
    // get principle point
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> v0_;
    
    // get starting position in z axis
    for(int i=0; i<26; i++)
      getline(fs, tmp);


    iss.str(tmp);

    iss.ignore(256, '=');

    iss >> start_slice_;

    // get end slice number
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> end_slice_;

    // get slice step
    if(sub_check == "Software=Version 1. 3 (build 2)")
    {
      getline(fs, tmp); //skip reconstruction time
      getline(fs, tmp); // skip post alignment
    }
    
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> slice_step_;


    //
    for(int i=0; i<4; i++)
      getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> size_x_;

    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> size_y_;

    // going to voxel size
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> voxel_size_;

    //correcting the effect of merging sensor pixels
    double multiplier = 1;
    if(voxel_size_ > 9. && voxel_size_ < 27 )
      multiplier = 2;
    
    if(voxel_size_ >= 27)
      multiplier = 4;

    camera_pixel_size_u_ *= multiplier;
    camera_pixel_size_v_ *= multiplier;
  }


}

    #endif

# if 0

imgr_skyscan_reconlog::imgr_skyscan_reconlog(char const* fname,xscan_scan & scan)
{
  vcl_ifstream fs(fname);
  if(!fs)
  {
    vcl_cerr << "file open failed for reading skyscan reconstructed image header\n";
    exit(1);
  }

  is_valid_ = false;

  vcl_string tmp;
  char junk[256];

  // read magic words from line 5
  vcl_string check;

  for(int i=0; i<5; i++)
    getline(fs, check);

  // in order to work for both windows & Linux
  vcl_string sub_check = check.substr(0,31);

  //if( check == "Software=Version 2. 5 (build 9)\r" 
  //    || check == "Software=Version 1. 3 (build 2)\r"
  //    || check == "Software=Version 1. 2 (build 1)\r" )
    if( sub_check == "Software=Version 2. 5 (build 9)" 
      || sub_check == "Software=Version 1. 3 (build 2)"
      || sub_check == "Software=Version 1. 2 (build 1)" )
  {
    is_valid_ = true;

    vcl_istringstream iss;

//first reading all the values from the log file 

    // get camera pixel size
    for(int i=0; i<4; i++)
      getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> camera_pixel_size_u_;

    
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    double u_div_v = 1;
    iss >> u_div_v;
    camera_pixel_size_v_ = camera_pixel_size_u_ / u_div_v;

    // get source rotation center distance
    for(int i=0; i<11; i++)
      getline(fs, tmp);

    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> src_rot_dist_;

    // get source sensor distance
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(265, '=');
    iss >> src_snsr_dist_;
    
    // get principle point
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> v0_;
    
    // get starting position in z axis
    for(int i=0; i<26; i++)
      getline(fs, tmp);


    iss.str(tmp);

    iss.ignore(256, '=');

    iss >> start_slice_;

    // get end slice number
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> end_slice_;

    // get slice step
    if(sub_check == "Software=Version 1. 3 (build 2)")
    {
      getline(fs, tmp); //skip reconstruction time
      getline(fs, tmp); // skip post alignment
    }
    
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> slice_step_;


    //
    for(int i=0; i<4; i++)
      getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> size_x_;

    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> size_y_;

    // going to voxel size
    getline(fs, tmp);
    iss.str(tmp);
    iss.ignore(256, '=');
    iss >> voxel_size_;

    //correcting the effect of merging sensor pixels
    double multiplier = 1;
    if(voxel_size_ > 9. && voxel_size_ < 27 )
      multiplier = 2;
    
    if(voxel_size_ >= 27)
      multiplier = 4;

    camera_pixel_size_u_ *= multiplier;
    camera_pixel_size_v_ *= multiplier;

    // changing some of the values based on the scan file values


   vpgl_calibration_matrix<double> kk(scan.kk());

    xscan_orbit_base_sptr orbit_base = scan.orbit();
  xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
  
   vnl_quaternion<double>turn_table_tr(orbit.t0());
  src_rot_dist_ = turn_table_tr.z();

 vgl_point_2d<double> princp_pt(kk.principal_point());
 v0_ = princp_pt.y();

u_div_v = kk.x_scale()/kk.y_scale();
camera_pixel_size_v_ = camera_pixel_size_u_/u_div_v;



  }


}

    #endif 
