#include "dbinfo_imatch_minfo_process.h"

#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>


#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_object_matcher.h>

#include <bpro1/bpro1_process_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/vidpro1_repository_sptr.h>

#define MARGIN  (10)

dbinfo_imatch_minfo_process::dbinfo_imatch_minfo_process()
{  
  if (
      !parameters()->add( "Intensity" , "-inten" , true )  ||
      !parameters()->add( "Gradient" , "-grad" , true )  ||
      !parameters()->add( "Smoothing sigma" , "-sigma" , 1.0f )  ||
      !parameters()->add( "R: " , "-R" , 10.0f ) ||
      !parameters()->add( "rigid alignment, dx:" , "-dx" , 1.0f )  ||
      !parameters()->add( "rigid alignment, dr:" , "-dr" , 0.0f )  ||
      !parameters()->add( "rigid alignment, ds:" , "-ds" , 0.0f )  ||
      !parameters()->add( "rigid alignment, ratio:" , "-ratio" , 0.1f )  ||
      !parameters()->add( "Number random iterations for initialization:" , "-Nob" , 10 ) ||
      !parameters()->add( "Forced Xform Scale:" , "-fscale" , 1.0f )
   )
  {
    vcl_cerr << "ERROR: Adding parameters in dbinfo_imatch_minfo_process::dbinfo_imatch_minfo_process()" << vcl_endl;
  }

  total_info_ = 0;
}


//: Clone the process
bpro1_process*
dbinfo_imatch_minfo_process::clone() const
{
  return new dbinfo_imatch_minfo_process(*this);
}

bool dbinfo_imatch_minfo_process::execute()
{
  float sigma, forced_scale;
  bool use_intensity;
  bool use_gradient;
  parameters()->get_value( "-inten" , use_intensity );
  parameters()->get_value( "-grad" , use_gradient );
  parameters()->get_value( "-sigma" , sigma );
  float R;
  parameters()->get_value( "-R" , R );
  float rigid_dx, rigid_dr, rigid_ds, rigid_ratio; int Nob;
  parameters()->get_value( "-dx", rigid_dx);
  parameters()->get_value( "-dr", rigid_dr);
  parameters()->get_value( "-ds", rigid_ds);
  parameters()->get_value( "-ratio", rigid_ratio);
  parameters()->get_value( "-Nob", Nob);
  parameters()->get_value( "-fscale", forced_scale);
 
  clear_output();

  //: get input images
  vidpro1_image_storage_sptr frame_image1;
  frame_image1.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr image_db_sptr = frame_image1->get_image();

  vidpro1_image_storage_sptr frame_image2;
  frame_image2.vertical_cast(input_data_[0][1]);
  vil_image_resource_sptr image_q_sptr = frame_image2->get_image();

  unsigned ni_db = image_db_sptr->ni(), nj_db = image_db_sptr->nj();
  vcl_cout << "Database image(" << ni_db << ' ' << nj_db << ")\n";

  unsigned ni_q = image_q_sptr->ni(), nj_q = image_q_sptr->nj();
  vcl_cout << "Query image(" << ni_q << ' ' << nj_q << ")\n";

  //----------------------------------
  // create the observations with intensity and gradient
  //----------------------------------
  dbinfo_observation_sptr obs_db = new dbinfo_observation(0, image_db_sptr, 0, true, true, false);

  dbinfo_observation_sptr obs_q = new dbinfo_observation(0, image_q_sptr, 0, true, true, false);


  vil_image_resource_sptr temp_sptr1, temp_sptr2, match_sptr;

  total_info_ = dbinfo_object_matcher::minfo_rigid_alignment_rand(obs_q, obs_db, rigid_dx, rigid_dr, rigid_ds, rigid_ratio, Nob, temp_sptr1,temp_sptr2, match_sptr, true, forced_scale, use_intensity, use_gradient);
  
  vcl_cout << "total_info: " << total_info_ << vcl_endl;

  //----------------------------------
  // create the output storage class
  //----------------------------------
#if 0
  if(temp_sptr1){
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(temp_sptr1);
  output_data_[0].push_back(output_storage);
  }

  if(temp_sptr2){
    vidpro1_image_storage_sptr output_storage2 = vidpro1_image_storage_new();
    output_storage2->set_image(temp_sptr2);
    output_data_[0].push_back(output_storage2);
    }
#endif
  if(match_sptr){
    vidpro1_image_storage_sptr output_storage3 = vidpro1_image_storage_new();
    output_storage3->set_image(match_sptr);
    output_data_[0].push_back(output_storage3);
  }
  return true;
}

