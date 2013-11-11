
#include "dbrl_superimpose_2ndtier.h"
#include<bpro1/bpro1_parameters.h>
#include<vsol/vsol_point_2d.h>
#include<vidpro1/storage/vidpro1_image_storage_sptr.h>
#include<vidpro1/storage/vidpro1_image_storage.h>
#include<dbinfo/pro/dbinfo_track_storage.h>
#include<dbinfo/pro/dbinfo_track_storage_sptr.h>
#include<dbinfo/dbinfo_observation_sptr.h>
#include<dbinfo/dbinfo_observation.h>

#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_vil.h>
#include <vnl/vnl_math.h>
#include<georegister/dbrl_rpm_affine.h>
#include<georegister/dbrl_match_set_sptr.h>
#include<georegister/dbrl_feature_sptr.h>
#include<georegister/dbrl_feature_point_sptr.h>
#include<georegister/dbrl_feature_point.h>
#include<pro/dbrl_id_point_2d_storage.h>
#include<pro/dbrl_id_point_2d_storage_sptr.h>
#include <osl/osl_edge.h>
#include <osl/osl_canny_ox.h>
#include <osl/osl_canny_ox_params.h>
#include <osl/osl_edgel_chain.h>
#include<bseg/brip/brip_vil_float_ops.h>
#include <vnl/vnl_vector_fixed.h>
#include <georegister/dbrl_support.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vul/vul_sprintf.h>
#include <bsol/bsol_algs.h>
#include <vgl/vgl_polygon.h>
#include <dbbgm/dbbgm_distribution_image.h>
#include <dbbgm/dbbgm_detect.h>
#include <dbsta/dbsta_detector_mixture.h>
#include <dbsta/dbsta_detector_gaussian.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage_sptr.h>
#include <vil/vil_save.h>

//: Constructor
dbrl_superimpose_2ndtier::dbrl_superimpose_2ndtier(void): bpro1_process()
{
    
if(!parameters()->add( "No of frames on each side to register" , "-winsize" , (int)1) ||
   !parameters()->add( "Initial temperature " , "-initT" , (float)1) ||
   !parameters()->add( "Annealing Rate" , "-annealrate" , (float) 0.93 )||
   !parameters()->add( "Final Temperature" , "-finalT" , (float) 0.0001 )||
   !parameters()->add( "Initial Lambda" , "-initlambda" , (float) 100 )||
   !parameters()->add( "Covergence Tolerance" , "-mconvg" , (float) 0.1 )||
   !parameters()->add( "Outlier value" , "-moutlier" , (float) 1e-5 )||
   !parameters()->add( "Scale the points to a grid", "-upscale" , (float) 100 )||
   !parameters()->add( "Radius of Neighborhood" , "-radius" , (float)5)||
   !parameters()->add( "Std Dev for proximity" , "-std" , (float)5)||
   !parameters()->add( "Threshold" , "-thresh" , (float)0.5)||
   !parameters()->add( "No of Iterations" , "-numiter" , (int)3)||
   !parameters()->add( "Distance Theshold" ,    "-dist" ,    2.5f        ) ||
   !parameters()->add( "Minimum Weight Threshold" ,    "-minweightthresh" ,    0.3f        )||
   !parameters()->add( "Radius of uncertainity" ,    "-rad" ,    (int)2        ))
    {
        vcl_cerr << "ERROR: Adding parameters in dbrl_superimpose_2ndtier::dbrl_superimpose_2ndtier()" << vcl_endl;
     }
    
}

//: Destructor
dbrl_superimpose_2ndtier::~dbrl_superimpose_2ndtier()
{

}


//: Return the name of this process
vcl_string
dbrl_superimpose_2ndtier::name()
{
  return "2nd tier Superimpose";
}


//: Return the number of input frame for this process
int
dbrl_superimpose_2ndtier::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbrl_superimpose_2ndtier::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_superimpose_2ndtier::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbinfo_track_storage" );
  to_return.push_back( "image" );
  to_return.push_back( "dbbgm_distribution_image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_superimpose_2ndtier::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
dbrl_superimpose_2ndtier::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbrl_superimpose_2ndtier::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  clear_output();

  //: obtain params for affine rpm.
  static float Tinit=1.0;
  static float Tfinal=0.0001;
  static float annealrate=0.93;
  static float lambdainit=1000;
  static float mconvg=0.1;
  static float moutlier=1e-5;
  static float scale=100;
  static float rad=10;
  static float std=5;
  static int numiter=3;
  static bool iscocirc=true;

  static float thresh=0.0;

  parameters()->get_value("-initT",Tinit);
  parameters()->get_value("-finalT",Tfinal);
  parameters()->get_value("-annealrate",annealrate);
  parameters()->get_value("-initlambda",lambdainit);
  parameters()->get_value("-mconvg",mconvg);
  parameters()->get_value("-moutlier",moutlier);
  parameters()->get_value("-upscale",scale);
  parameters()->get_value("-radius",rad);
  parameters()->get_value("-numiter",numiter);
  parameters()->get_value("-std",std);
  parameters()->get_value("-thresh",thresh);
  parameters()->get_value("-iscocirc",iscocirc);


  //: initiating the registration process.
  dbrl_rpm_affine_params affineparams(lambdainit,mconvg,Tinit,Tfinal,moutlier,annealrate);
  int winsize=0;
  parameters()->get_value("-winsize",winsize);
  //: obtain the tracks 
  dbinfo_track_storage_sptr track_storage;
  track_storage.vertical_cast(input_data_[0][0]);

  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(input_data_[0][1]);
  vil_image_resource_sptr image_sptr = image_storage->get_image();

  dbbgm_distribution_image_storage_sptr frame_model;
  frame_model.vertical_cast(input_data_[0][2]);
  /*dbbgm_distribution_image<float> *model = 
    dynamic_cast<dbbgm_distribution_image<float>*>(frame_model->dist_image().ptr());*/

  vcl_vector<dbinfo_track_sptr> tracks=track_storage->tracks();
  unsigned  frame_no=image_storage->frame();
  vidpro1_vsol2D_storage_sptr output_vsol_subpixel = vidpro1_vsol2D_storage_new();
  vidpro1_vsol2D_storage_sptr output_vsol_superimposed = vidpro1_vsol2D_storage_new();
  dbrl_id_point_2d_storage_sptr output_id_points=dbrl_id_point_2d_storage_new();

  vcl_vector<dbrl_id_point_2d_sptr> all_pivot_pointedges;
  vcl_vector<dbrl_id_point_2d_sptr> xformed_all_pivot_pointedges;
  vcl_vector<dbrl_id_point_2d_sptr> idpointssupport;

  int cnt=0;
  for(vcl_vector<dbinfo_track_sptr>::iterator trit = tracks.begin();
      trit != tracks.end(); trit++)
      {
      dbinfo_observation_sptr ref_obs=(*trit)->observ(frame_no); //: observation in the current frame
      vcl_vector<vsol_point_2d_sptr>  pivot_pointedges;//=ref_obs->get_edges();
      vcl_vector<dbrl_feature_sptr> f1;
      vcl_vector<vsol_spatial_object_2d_sptr> f1pts;
      vcl_vector<dbrl_id_point_2d_sptr> pivot_id_points;

      for(int k=0;k<static_cast<int>(pivot_pointedges.size());k++)
          {
          vnl_vector_fixed<double,2> pt(pivot_pointedges[k]->x()/scale,pivot_pointedges[k]->y()/scale);
          dbrl_feature_point * fpt=new dbrl_feature_point(pt);
          fpt->setid(k);
          f1.push_back(fpt);
          f1pts.push_back(pivot_pointedges[k]->cast_to_spatial_object());
          pivot_id_points.push_back(new dbrl_id_point_2d(pivot_pointedges[k]->x(),pivot_pointedges[k]->y(),k));
          }
      if(ref_obs.ptr())
          {
          vcl_vector<vcl_vector< vsol_spatial_object_2d_sptr > > xpoints; 
          output_vsol_superimposed->add_objects(f1pts,"curr_points");
          for(int i=static_cast<int>(frame_no)-static_cast<int>(winsize);i<=static_cast<int>(frame_no)+static_cast<int>(winsize);i++)
              if(i!=static_cast<int>(frame_no))
                  {
                  dbinfo_observation_sptr temp_obs=(*trit)->observ(i);
                  if(temp_obs.ptr())
                      {
                      vcl_vector<vsol_point_2d_sptr>  curr_pointedges;//=temp_obs->get_edges();
                      vcl_vector<dbrl_feature_sptr> f2;
                      for(int k=0;k<static_cast<int>(curr_pointedges.size());k++)
                          {
                          vnl_vector_fixed<double,2> pt(curr_pointedges[k]->x()/scale,curr_pointedges[k]->y()/scale);
                          dbrl_feature_point * fpt=new dbrl_feature_point(pt);
                          fpt->setid(k);
                          f2.push_back(fpt);

                          }
                      //output_vsol_superimposed->add_objects(f1pts,"curr_points");
                      dbrl_rpm_affine match_affinely(affineparams,f1,f2);
                      dbrl_match_set_sptr match_set=match_affinely.rpm();

                      //: binarize the match matrix
                      dbrl_correspondence M=match_set->get_correspondence();
                      M.setinitialoutlier(affineparams.outlier());
                      //: in order to solve multiple effects.
                      M.binarize(0.51);
                      //: retreiving final transform
                      dbrl_estimator_point_affine* affine_est= new dbrl_estimator_point_affine();
                      affine_est->set_lambda(0.0);
                      dbrl_transformation_sptr tform=affine_est->estimate(match_set->get_feature_set1(),match_set->get_feature_set2(),M);
                      dbrl_affine_transformation * affinetform=dynamic_cast<dbrl_affine_transformation *> (tform.ptr());
                      affinetform->set_from_features(match_set->get_feature_set2());
                      affinetform->transform();

                      vcl_vector<dbrl_feature_sptr> f2xformed=affinetform->get_to_features();
                      vcl_vector<dbrl_id_point_2d_sptr> f2xid;
                      vcl_vector<vsol_spatial_object_2d_sptr> f2pts;
                      for(unsigned m=0;m<f2xformed.size();m++)
                          if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f2xformed[m].ptr()))
                              {
                              vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0]*scale,pt->location()[1]*scale);
                              f2pts.push_back(p->cast_to_spatial_object());
                              }
                          vcl_string namepoints=vul_sprintf("points%d",i);
                          output_vsol_superimposed->add_objects(f2pts,namepoints);
                          //xformed_all_pivot_pointedges.insert(xformed_all_pivot_pointedges.begin(),f2xid.begin(),f2xid.end());
                      }
                  }
              xformed_all_pivot_pointedges.insert(xformed_all_pivot_pointedges.begin(),pivot_id_points.begin(),pivot_id_points.end());
                    
          }
cnt++;
      }
      //}

  output_data_[0].push_back(output_vsol_superimposed);
  return true;
}
//: Clone the process
bpro1_process*
dbrl_superimpose_2ndtier::clone() const
{
  return new dbrl_superimpose_2ndtier(*this);
}
    
bool
dbrl_superimpose_2ndtier::finish()
{
  return true;
}



