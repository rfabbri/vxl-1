
#include "dbrl_warp_image_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vsol/vsol_point_2d.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <georegister/dbrl_feature_sptr.h>
#include <georegister/dbrl_feature_point.h>
#include <vnl/vnl_vector_fixed.h>
#include <georegister/dbrl_rpm_tps.h>
#include "dbrl_match_set_storage.h"
#include "dbrl_match_set_storage_sptr.h"
#include <georegister/dbrl_id_point_2d.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include <georegister/dbrl_rpm_affine.h>
#include <georegister/dbrl_rpm_tps.h>
#include <georegister/dbrl_match_set.h>
#include <georegister/dbrl_match_set_sptr.h>
#include <georegister/dbrl_feature_point_sptr.h>
#include <georegister/dbrl_estimator_sptr.h>
#include "dbrl_id_point_2d_storage.h"
#include "dbrl_id_point_2d_storage_sptr.h"
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <brip/brip_vil_float_ops.h>

#include <vil/vil_image_view.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_bilin_interp.h>
#include <vnl/vnl_matlab_filewrite.h>
#include <vnl/vnl_transpose.h>

//: Constructor
dbrl_warp_image_process::dbrl_warp_image_process(void): bpro1_process()
    {


    }

//: Destructor
dbrl_warp_image_process::~dbrl_warp_image_process()
    {

    }


//: Return the name of this process
vcl_string
dbrl_warp_image_process::name()
    {
    return "Warp Image";
    }


//: Return the number of input frame for this process
int
dbrl_warp_image_process::input_frames()
    {
    return 1;
    }


//: Return the number of output frames for this process
int
dbrl_warp_image_process::output_frames()
    {
    return 1;

    }


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_warp_image_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    to_return.push_back( "dbrl_match_set" );
    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_warp_image_process::get_output_type()
    {  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "image" );

    return to_return;
    }


//: Execute the process
bool
dbrl_warp_image_process::execute()
    {
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbrl_warp_image_process::execute() - "
            << "not exactly two input images \n";
        return false;
        }
clear_output(1);
// get image from the storage class
vidpro1_image_storage_sptr frame_image;
frame_image.vertical_cast(input_data_[0][0]);

// convert the image to float
 vil_image_resource_sptr image_sptr = frame_image->get_image();
 image_list_.push_back(image_sptr);


 dbrl_match_set_storage_sptr match_storage;
 match_storage.vertical_cast(input_data_[0][1]);
 dbrl_match_set_sptr match_set=match_storage->matchset();

 match_set_list_.push_back(match_set);
 framenums_.push_back(match_storage->frame());
 return true;  
    }
//: Clone the process
bpro1_process*
dbrl_warp_image_process::clone() const
    {
    return new dbrl_warp_image_process(*this);
    }

bool
dbrl_warp_image_process::finish()
    {
    static int pivot_frame_no=(int)(image_list_.size()/2);

     //: outer loop for computing TPS from each frame to pivot frame
    for(int imgi=0;imgi<image_list_.size();imgi++)
      {
          //: approximation alignment of point-sets 
      if(imgi!=pivot_frame_no)
          {   
          vil_image_view< unsigned char > imview1 = image_list_[pivot_frame_no]->get_view(0, image_list_[pivot_frame_no]->ni(), 0, image_list_[pivot_frame_no]->nj() );
          vil_image_view< unsigned char > imview2 = image_list_[imgi]->get_view(0, image_list_[imgi]->ni(), 0, image_list_[imgi]->nj() );
          vil_image_view< unsigned char > greyview1;
          vil_image_view< unsigned char > greyview2;
          if( imview1.nplanes() == 3 ) {
              vil_convert_planes_to_grey( imview1 , greyview1 );
              vil_convert_planes_to_grey( imview2 , greyview2 );
              }
          else if ( imview1.nplanes() == 1 ) {
              greyview1 = imview1;
              greyview2 = imview2;
              } 
          else {
              vcl_cerr << "Returning false. nplanes(): " << imview1.nplanes() << vcl_endl;
              return false;
              }
          vil_image_view<float> float_curr_view1= brip_vil_float_ops::convert_to_float(greyview1);
          vil_image_view<float> float_curr_view2= brip_vil_float_ops::convert_to_float(greyview2);
          

      dbrl_match_set_sptr msp1to2=match_set_list_[imgi];
      
      dbrl_correspondence M1to2=msp1to2->get_correspondence();
      vnl_transpose Mt(M1to2.M());
      dbrl_correspondence *M2to1=new dbrl_correspondence(Mt.asMatrix());
      dbrl_estimator_point_thin_plate_spline * esti2to1= new dbrl_estimator_point_thin_plate_spline();
      esti2to1->set_lambda1(0.0);
      esti2to1->set_lambda2(0.0);
      vcl_vector<dbrl_feature_sptr> f2=msp1to2->get_feature_set2();
      vcl_vector<dbrl_feature_sptr> f1=msp1to2->get_feature_set1();
      dbrl_rpm_tps *tpsrpm=new dbrl_rpm_tps();
      tpsrpm->normalize_point_set(M2to1->M(),f2 );
      dbrl_transformation_sptr tform2to1=esti2to1->estimate(f2,f1,*M2to1);
      dbrl_thin_plate_spline_transformation *tpstform2to1=dynamic_cast<dbrl_thin_plate_spline_transformation *>(tform2to1.ptr());

      //: setting grid
      vcl_vector<dbrl_feature_sptr> pointset;
      int nj=imview2.nj();
      int ni=imview2.ni();

      for(unsigned i=0;i<ni;i++)
          for(unsigned j=0;j<nj;j++)
              {
              dbrl_feature_point * p=new dbrl_feature_point((double)i/(double)ni,(double)j/(double)nj);
              pointset.push_back(p);
              }
          //: build K for the grid
          vcl_vector<dbrl_feature_sptr> warped_grid_pts=warp_grid(tpstform2to1,pointset);
          vil_image_view<unsigned char> warpimg(ni,nj,1);
          for(unsigned i=0;i<ni;i++)
              for(unsigned j=0;j<nj;j++)
                  {
                  if(dbrl_feature_point *p=dynamic_cast<dbrl_feature_point*>(warped_grid_pts[i*nj+j].ptr()))
                      { warpimg(i,j)=(int)vcl_floor(vil_bilin_interp_safe<unsigned char>(greyview2,p->location()[0]*160,p->location()[1]*160));//,greyview2.top_left_ptr(),ni,nj,nj,1));
                        
                      }
                  }

              vidpro1_image_storage_sptr img_storage = vidpro1_image_storage_new();
              img_storage->set_image(vil_new_image_resource_of_view(warpimg));
              output_data_[framenums_[imgi]].push_back(img_storage);
        }
      }

    

     return true;
    }

vcl_vector<dbrl_feature_sptr> dbrl_warp_image_process::warp_grid(dbrl_thin_plate_spline_transformation * tpstform,vcl_vector<dbrl_feature_sptr> &f)
    {
    tpstform->build_K(f);
    tpstform->set_from_features(f);
    tpstform->transform();
    return tpstform->get_to_features();
    }




