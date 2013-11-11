#include <dbrkf/dbrkf_simplerect.h>
#include <mvl/FMatrix.h>
#include <mvl/FMatrixComputeLinear.h>
#include <vbl/vbl_bounding_box.h>
#include <vil/vil_bilin_interp.h>
#include <vnl/vnl_inverse.h>
#include <vgl/algo/vgl_h_matrix_1d.h>
#include <vgl/algo/vgl_h_matrix_1d_compute_linear.h>
#include <vgl/algo/vgl_h_matrix_1d_compute.h>
#include<vnl/vnl_math.h>

dbrkf_simplerect::dbrkf_simplerect()
{

}
dbrkf_simplerect::dbrkf_simplerect(vcl_vector<vsol_point_2d_sptr > pointsl,
               vcl_vector<vsol_point_2d_sptr > pointsr,
               vil_image_view<vxl_byte> imagel,
               vil_image_view<vxl_byte> imager)
    
{
    pointsl_=pointsl;
    pointsr_=pointsr;
    imgl=imagel;
    imgr=imager;

    compute_fundamental_matrix(pointsl_,pointsr_);

    imgl_step1=new vil_image_view<vxl_byte>;
    imgr_step1=new vil_image_view<vxl_byte>;
    imgl_step2=new vil_image_view<vxl_byte>;
    imgr_step2=new vil_image_view<vxl_byte>;

}
dbrkf_simplerect::~dbrkf_simplerect()
{

}

bool dbrkf_simplerect::compute_fundamental_matrix(vcl_vector<vsol_point_2d_sptr > pointsl,
                                        vcl_vector<vsol_point_2d_sptr > pointsr)
{
    assert(pointsl.size()==pointsr.size());
    assert(pointsl.size()>8);
    
    vcl_vector<vgl_homg_point_2d<double> > homgpl,homgpr;

    //: convert points into homogeneous points
    for(unsigned int i=0;i<pointsl.size();i++)
    {
     vgl_homg_point_2d<double> pl(pointsl[i]->x(),pointsl[i]->y());
     homgpl.push_back(pl);
 
     vgl_homg_point_2d<double> pr(pointsr[i]->x(),pointsr[i]->y());
     homgpr.push_back(pr);

    }
     //: compute the F matrix for set of correspondence
     FMatrix F;
     FMatrixComputeLinear comp;
     F=comp.compute(homgpl,homgpr);

     Fm=F.get_matrix();
     F.get_epipoles(el,er);
     return true;
}


 //: step1 is to send the epipole of each image to infinity along the horizontal direction 
bool dbrkf_simplerect::rect_step1()
{
    vnl_double_3x3 Hl,Hr;
    int img_hl = imgl.nj();
    int img_wl = imgl.ni();
    int img_hr = imgr.nj();
    int img_wr = imgr.ni();
    if(!map_epipoles_infinity(Hl,el,img_wl/2,img_hl/2)|| !map_epipoles_infinity(Hr,er,img_wr/2,img_hr/2))
        return false;
    resample(Hl_step1,Hr_step1,imgl_step1,imgr_step1,Hl,Hr,imgl,imgr);
   return true;   
}
bool dbrkf_simplerect::rect_step2()
{

    assert(pointsl_.size()==pointsr_.size());
    assert(pointsl_.size()>5);
    
    vcl_vector<vsol_point_2d_sptr >  ptsl,ptsr;
    //: transforming set of corresponding points using the Homography computed in step1
    for(unsigned int i=0;i<pointsl_.size();i++)
    {
        vnl_double_3 vecl(pointsl_[i]->x(),pointsl_[i]->y(),1);
        vnl_double_3 veclt=Hl_step1*vecl;
        vsol_point_2d_sptr tl=new vsol_point_2d(veclt[0]/veclt[2],veclt[1]/veclt[2]);
        ptsl.push_back(tl);

        vnl_double_3 vecr(pointsr_[i]->x(),pointsr_[i]->y(),1);
        vnl_double_3 vecrt=Hr_step1*vecr;
        vsol_point_2d_sptr tr=new vsol_point_2d(vecrt[0]/vecrt[2],vecrt[1]/vecrt[2]);
        ptsr.push_back(tr);
    }

    vcl_vector<vgl_homg_point_2d<double> > homgp0,homgp1;
    vcl_vector<vgl_homg_line_2d<double> > homgl0,homgl1,homgl,homglr;
    vcl_vector<vgl_homg_point_1d<double> > pl,pr,p;

    for(int i=0;i<ptsl.size();i++)
    {
     vgl_homg_point_1d<double> p0(ptsl[i]->y());
     pl.push_back(p0);
     vgl_homg_point_1d<double> p1(ptsr[i]->y());
     pr.push_back(p1);
     vgl_homg_point_1d<double> pm((ptsl[i]->y()+ptsr[i]->y())/2);
     p.push_back(pm);
    }

 vgl_h_matrix_1d<double> Hl,Hr;
 vgl_h_matrix_1d_compute_linear clinear0,clinear1;

 clinear0.compute(pl,p,Hl);
 clinear1.compute(pr,p,Hr);

 vnl_double_2x2 Hml=Hl.get_matrix();
 vnl_double_2x2 Hmr=Hr.get_matrix();

 resample(Hl_step2,Hr_step2,imgl_step2,imgr_step2,Hml,Hmr,imgl_step1,imgr_step1);
 
 return true;
}
bool dbrkf_simplerect::map_epipoles_infinity(vnl_double_3x3 &H0,vgl_homg_point_2d<double> ep,double ci, double cj)
{

  vnl_double_3 epv(ep.x()/ep.w(),ep.y()/ep.w(),1.0);
  // First of all, translate the centre pixel to 0, 0
  H0[0][0] = 1.0; H0[0][1] = 0.0; H0[0][2] = -ci;
  H0[1][0] = 0.0; H0[1][1] = 1.0; H0[1][2] = -cj;
  H0[2][0] = 0.0; H0[2][1] = 0.0; H0[2][2] = 1.0;
  // Translate the epipole as well
  epv = H0 * epv;

  // Make sure that the epipole is not at the origin
  if (epv[0] == 0.0 && epv[1] == 0.0)
  {
    vcl_cerr<<"Error : Epipole is at image center\n";
    return 0;
  }

  // Next determine a rotation that will send the epipole to (1, 0, x)
  double theta = -vcl_atan2(epv[1],epv[0]) + vnl_math::pi;
  double c = vcl_cos (theta);
  double s = vcl_sin (theta);

  vnl_double_3x3 T;

  T[0][0] =   c; T[0][1] = - s; T[0][2] = 0.0;
  T[1][0] =  s; T[1][1] =   c; T[1][2] = 0.0;
  T[2][0] = 0.0; T[2][1] = 0.0; T[2][2] = 1.0;

    // Multiply things out
  H0 =  T * H0;
  epv = T * epv;

  // Now send the epipole to infinity
  double x = epv[2]/epv[0];

  vnl_double_3x3 E;
  E[0][0] = 1.0; E[0][1] = 0.0; E[0][2] = 0.0;
  E[1][0] = 0.0; E[1][1] = 1.0; E[1][2] = 0.0;
  E[2][0] =  -x; E[2][1] = 0.0; E[2][2] = 1.0;

  // Multiply things out.  Put the result in H0
  H0 = E * H0;
  epv= E*epv;
  vcl_cout<<"\n"<<epv;
  return true;
}





void dbrkf_simplerect::resample(vnl_double_3x3 &Hfl,vnl_double_3x3 &Hfr,vil_image_view<vxl_byte> *imgfl,vil_image_view<vxl_byte> *imgfr,
                      vnl_double_3x3 Hl,vnl_double_3x3 Hr,vil_image_view<vxl_byte> imgl,vil_image_view<vxl_byte> imgr)
                      
{
  
  vbl_bounding_box<double,2> box;

  // Find the bound of the image to be resampled
  vnl_double_3 ipointL; // input and output image points
  vnl_double_3 opointL; // "input" = non-rectified image

  vnl_double_3 ipointR; // input and output image points
  vnl_double_3 opointR; // "input" = non-rectified image

  int img_h = imgl.nj();
  int img_w = imgl.ni();

  for (int i=0; i<img_h; i++)
  {
    for (int j=0; j<img_w; j++)
    {
      ipointL[0]=j; ipointL[1]=i; ipointL[2]=1.0;
      opointL = Hl*ipointL;
      opointL /= opointL[2]; // unhomogenize the point... [x,y,1.0]

      ipointR[0]=j; ipointR[1]=i; ipointR[2]=1.0;
      opointR = Hr*ipointR;
      opointR /= opointR[2]; // unhomogenize the point... [x,y,1.0]
      // save the extremes for image sizing and bounds checking
      box.update(opointL[0],opointL[1]);
      box.update(opointR[0],opointR[1]);
    }
  }

  // Find the mapping from rectified space back to the original
  // using the inverse of the transforms
  vnl_double_3x3 Hlinv = vnl_inverse(Hl);
  vnl_double_3x3 Hrinv = vnl_inverse(Hr);

  int img2_h = int(box.max()[1] - box.min()[1] + 1); // output image size
  int img2_w = int(box.max()[0] - box.min()[0] + 1); // (make them both the same)
  int planes=1;

  //re-size the images for the next iteration...
  imgfl->set_size(img2_w,img2_h,planes);
  imgfr->set_size(img2_w,img2_h,planes);
  
  for (int i=0; i<img2_h; i++)
  {
    for (int j=0; j<img2_w; j++)
    {
      // create the homogeneous point in rectified image(s)
      opointL[0]=j+box.min()[0]; opointL[1]=i+box.min()[1]; opointL[2]=1.0;
      // map it into the original image(s)
      ipointL = Hlinv * opointL; ipointL /= ipointL[2]; // unhomogenize
      // Bounds checking...
      if (ipointL[0] >= 0 && ipointL[0] < img_w-1 &&
          ipointL[1] >= 0 && ipointL[1] < img_h-1)
      {
        // interpolate the rectified image point from the original.
        (*imgfl)(j,i) = (vxl_byte)vil_bilin_interp(imgl,ipointL[0],ipointL[1],0);
      }
      else
        (*imgfl)(j,i) = vxl_byte(0);
    
     opointR[0]=j+box.min()[0]; opointR[1]=i+box.min()[1]; opointR[2]=1.0;
      // map it into the original image(s)
      ipointR = Hrinv * opointR; ipointR /= ipointR[2]; // unhomogenize
      // Bounds checking...
     if (ipointR[0] >= 0 && ipointR[0] < img_w-1 &&
          ipointR[1] >= 0 && ipointR[1] < img_h-1)
      {
        // interpolate the rectified image point from the original.
        (*imgfr)(j,i) = (vxl_byte)vil_bilin_interp(imgr,ipointR[0],ipointR[1],0);
      }
      else
        (*imgfr)(j,i) = vxl_byte(0);
    }


  }

    vnl_double_3x3 tH;
    tH[0][0]=1.0;tH[0][1]=0.0;tH[0][2]=-box.min()[0];
    tH[1][0]=0.0;tH[1][1]=1.0;tH[1][2]=-box.min()[1];
    tH[2][0]=0.0;tH[2][1]=0.0;tH[2][2]=1.0;

    Hfl=tH*Hl;
    Hfr=tH*Hr;
}


void dbrkf_simplerect::resample(vnl_double_2x2 &Hfl,vnl_double_2x2 &Hfr,vil_image_view<vxl_byte> *imgfl,vil_image_view<vxl_byte> *imgfr,
                      vnl_double_2x2 Hl,vnl_double_2x2 Hr,vil_image_view<vxl_byte> *imgl,vil_image_view<vxl_byte> *imgr)
{
  
  vbl_bounding_box<double,2> box;
  
  // Find the bound of the image to be resampled
  vnl_double_2 ipointL; // input and output image points
  vnl_double_2 opointL; // "input" = non-rectified image
  vnl_double_2 ipointR;
  vnl_double_2 opointR;

 
  int img_h = imgl->nj();
  int img_w = imgl->ni();
  for (int i=0; i<img_h; i++)
  {
    for (int j=0; j<img_w; j++)
    {
      ipointL[0]=i; ipointL[1]=1.0;
      opointL = Hl * ipointL;
      opointL /= opointL[1]; // unhomogenize the point... [x,y,1.0]
      ipointR[0]=i; ipointR[1]=1.0;
      opointR = Hr * ipointR;
      opointR /= opointR[1]; 
      // save the extremes for image sizing and bounds checking
      box.update(j,opointL[0]);
      box.update(j,opointR[0]);

    }
  } 

  // Now we know the size of the image
  // Now do the warping

  // Find the mapping from rectified space back to the original
  // using the inverse of the transforms
  vnl_double_2x2 Hlinv = vnl_inverse(Hl);
  vnl_double_2x2 Hrinv = vnl_inverse(Hr);
  
  int img2_h = int(box.max()[1] - box.min()[1] + 1); // output image size
  int img2_w = int(box.max()[0] - box.min()[0] + 1); // (make them both the same)
  int planes=1;


  //re-size the images for the next iteration...
  imgfl->set_size(img2_w,img2_h,planes);
  imgfr->set_size(img2_w,img2_h,planes);
  
  for (int i=0; i<img2_h; i++)
  {
    for (int j=0; j<img2_w; j++)
    {
      // create the homogeneous point in rectified image(s)
      opointL[0]=i+box.min()[1]; opointL[1]=1.0;
      opointR[0]=i+box.min()[1]; opointR[1]=1.0;

      // map it into the original image(s)
      ipointL = Hlinv * opointL; ipointL /= ipointL[1]; // unhomogenize
      ipointR = Hrinv * opointR; ipointR /= ipointR[1]; // unhomogenize
      
      // Bounds checking...
      if (ipointL[0] >= 0 && ipointL[0] < img_h-1)
      {
        // interpolate the rectified image point from the original.
        (*imgfl)(j,i) = (vxl_byte)vil_bilin_interp(*imgl,j,ipointL[0],0);
      }
      else
        (*imgfl)(j,i) = vxl_byte(0);
 
    if (ipointR[0] >= 0 && ipointR[0] < img_h-1)
      {
        // interpolate the rectified image point from the original.
        (*imgfr)(j,i) = (vxl_byte)vil_bilin_interp(*imgr,j,ipointR[0],0);
      }
      else
        (*imgfr)(j,i) = vxl_byte(0);
    }
  }
  vnl_double_2x2 tH;
  tH[0][0]=1.0;tH[0][1]=-box.min()[1];
  tH[1][0]=0.0;tH[1][1]=1.0;
  
  Hfl=tH*Hl;
  Hfr=tH*Hr;
}




vgl_point_2d<double> dbrkf_simplerect::undo_steps(vgl_point_2d<double> p,vcl_string tag)
{

       vnl_double_2 point(p.y(),1.0);
       if(tag=="left")
       {
           point=vnl_inverse(Hl_step2)*point;
           vnl_double_3 point2(p.x(),point[0]/point[1],1.0);
           point2=vnl_inverse(Hl_step1)*point2;

           vgl_point_2d<double> pf(point2[0]/point2[2],point2[1]/point2[2]);
           return pf;
       }
       else if(tag=="right")
       {
           point=vnl_inverse(Hr_step2)*point;
           vnl_double_3 point2(p.x(),point[0]/point[1],1.0);
           point2=vnl_inverse(Hr_step1)*point2;

           vgl_point_2d<double> pf(point2[0]/point2[2],point2[1]/point2[2]);
           return pf;
       }

       else
           return 0;
    
    
}
