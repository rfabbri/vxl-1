//:
// \file
#include <dbctrk/dbctrk_curve_description.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <dbctrk/dbctrk_algs.h>
#include <dbctrk/dbctrk_curveMatch.h>
#include <vgl/io/vgl_io_point_2d.h>
#include <vgl/io/vgl_io_vector_2d.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/vnl_math.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vbl/vbl_bounding_box.h>
#include<bseg/brip/brip_vil_float_ops.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <dbctrk/dbctrk_utils.h>

//: Constructor
dbctrk_curve_description::dbctrk_curve_description()
{
  center_.set(0.0, 0.0);
  spacing_on_curve=10;
  isIHS=true;

  curve_=new Curve(); 
  coarser_curve_=new Curve();

}
dbctrk_curve_description::~dbctrk_curve_description()
{
  delete curve_;
  delete coarser_curve_;
}

//: Constructor : compute description from a digital curve and a given order of description
void dbctrk_curve_description::init(vdgl_edgel_chain_sptr const& ec)
{
  if (!ec)
  {
    vcl_cout<<"In dbctrk_curve_description::dbctrk_curve_description(..) - warning, null chain\n";
    return;
  }

  
  unsigned int N = ec->size();
  
///  coarser_points_.clear();
///  points_.clear(); 
  angles_.clear(); 
  grad_.clear();
  
  // means
  center_.set(0.0, 0.0);
  ///gradient_mean_val_=0.0;
  ///gradient_mean_dir_=0.0;
  
  int j=0;
 
  vcl_vector<vgl_point_2d<double> > points;
  vcl_vector<vgl_point_2d<double> > coarser_points;

  for (unsigned int i=0; i<N; ++i)
  {
    vdgl_edgel ed = ec->edgel(i);
    center_.set(center_.x() + ed.get_x(), center_.y() + ed.get_y());

    points.push_back(vgl_point_2d<double>(ed.get_x(),ed.get_y()));
    angles_.push_back(ed.get_theta());
    grad_.push_back(ed.get_grad());
    if (j%10==0)
      coarser_points.push_back(vgl_point_2d<double>(ed.get_x(),ed.get_y()));
    j++;
  }

  

  center_.set( center_.x()/double(N) , center_.y()/double(N) );
  ///gradient_mean_val_ /= double(N);
  ///gradient_mean_dir_ /= double(N);
  
  dbctrk_algs::smooth_curve(points,1);
 
  //curve_ = new Curve();
  //coarser_curve_ = new Curve();
  
  curve_->readDataFromVector(points);
  coarser_curve_->readDataFromVector(coarser_points);

  curve_->computeProperties();
  coarser_curve_->computeProperties();

  
  
  compute_cms();

  return;
}

void dbctrk_curve_description::compute_mirror_curves(double r,vil_image_view<float> dt,vil_image_view<int> map,int id)
{
 pospoints.clear();
 negpoints.clear();


 for(int i=0;i<curve_->numPoints();i++)
 {
     double thetap=curve_->angle(i)+vnl_math::pi/2;
     double thetan=curve_->angle(i)-vnl_math::pi/2;

     vsol_point_2d_sptr  pointp=new vsol_point_2d(curve_->point(i).x()+r*vcl_cos(thetap),curve_->point(i).y()+r*vcl_sin(thetap));
     vsol_point_2d_sptr  pointn=new vsol_point_2d(curve_->point(i).x()+r*vcl_cos(thetan),curve_->point(i).y()+r*vcl_sin(thetan));

     pospoints.push_back(pointp);
     negpoints.push_back(pointn);
    
 }

 curvepos=new vsol_digital_curve_2d(pospoints);
 curveneg=new vsol_digital_curve_2d(negpoints);

 int mask_size=1;
 

 for(unsigned int k=0;k<pospoints.size();k++)
 {
   int p_x=(int)vcl_floor(pospoints[k]->x()+0.5);
   int p_y=(int)vcl_floor(pospoints[k]->y()+0.5);
   vcl_vector<vsol_point_2d_sptr> points;
   vcl_vector<bool> isinmask;
   vil_image_view<float> pmask;
   pmask.set_size(2*mask_size+1, 2*mask_size+1, 1);
   for(int j=-mask_size;j<=mask_size;j++)
   {
    for(int i=-mask_size;i<=mask_size;i++)
    {   
         int curr_x=p_x+i;
         int curr_y=p_y+j;

         //: computing the dot product of the vector joining point on the curve to the shifted point with the vector joining the curve on the point to the mask point.
         double dot_product=(curve_->point(k).x()-p_x)*(curve_->point(k).x()-curr_x/*-p_x*/)+(curve_->point(k).y()-p_y)*(curve_->point(k).y()-curr_y/*curr_y-p_y*/);
   //if(map(curr_x,curr_y)==id)
   if(curr_x>=0 && curr_x< (int)dt.ni() && curr_y >=0 && curr_y <(int)dt.nj())
     {
       if(dt(curr_x,curr_y)>1.3)
     {
       if(dot_product>=0)
         {
     vsol_point_2d_sptr point=new vsol_point_2d((double)curr_x,(double)curr_y);
     points.push_back(point);
     pmask(i+mask_size,j+mask_size)=1.0;
     isinmask.push_back(true);
         }
         else
     {
             isinmask.push_back(false);
             pmask(i+mask_size,j+mask_size)=0.0;
     }
     }
     }
   
    }
   }
   maskpos.push_back(points);
   isvalidmaskpos.push_back(isinmask);
   tpmask.push_back(pmask);
 }
 for(unsigned int k=0;k<negpoints.size();k++)

   {
     int p_x=(int)vcl_floor(negpoints[k]->x()+0.5);
     int p_y=(int)vcl_floor(negpoints[k]->y()+0.5);
     vcl_vector<vsol_point_2d_sptr> points;
     vcl_vector<bool> isinmask;
     vil_image_view<float> nmask;
     nmask.set_size(2*mask_size+1, 2*mask_size+1, 1);
     nmask.fill(0);
     for(int j=-mask_size;j<=mask_size;j++)
       {
   for(int i=-mask_size;i<=mask_size;i++)
     {   
       int curr_x=p_x+i;
       int curr_y=p_y+j;
       //: computing the dot product of the vector joining point on the curve to the shifted point with the vector joining the curve on the point to the mask point.
       double dot_product=(curve_->point(k).x()-p_x)*(curve_->point(k).x()-curr_x/*-p_x*/)+(curve_->point(k).y()-p_y)*(curve_->point(k).y()-curr_y/*curr_y-p_y*/);
       if(curr_x>=0 && curr_x< (int)dt.ni() && curr_y >=0 && curr_y <(int)dt.nj())
         {
     if(dt(curr_x,curr_y)>1.3)
       {
         if(dot_product>=0)
           {
       vsol_point_2d_sptr point=new vsol_point_2d((double)curr_x,(double)curr_y);
       points.push_back(point);
       isinmask.push_back(true);
       nmask(i+mask_size,j+mask_size)=1.0;
       
           }
         else
           {
       isinmask.push_back(false);
       nmask(i+mask_size,j+mask_size)=0.0;
           }
       }
         }
     }
       }
     maskneg.push_back(points);
     isvalidmaskneg.push_back(isinmask);
     tnmask.push_back(nmask);
   }
}

void dbctrk_curve_description::assign_rgb_values(vil_image_view<float> p0,vil_image_view<float> p1,vil_image_view<float> p2, int maskradius,vil_image_view<float> dtimg, vil_image_view<int> dtmap,int id)
{
  compute_mirror_curves(3,dtimg,dtmap,id);
  int masksize=1;
    vbl_array_2d<float> g2d(2*masksize+1,2*masksize+1,0);
    utils::gauss2d(1.5,g2d);
   //: smoothing the masked regions for each point
   for(unsigned int k=0;k<pospoints.size();k++)
   {
     int p_x=(int)vcl_floor(pospoints[k]->x()+0.5);
     int p_y=(int)vcl_floor(pospoints[k]->y()+0.5);
   

     vbl_bounding_box<double,2> p_box_temp;
     p_box_temp.update(p_x-masksize,p_y-masksize);
     p_box_temp.update(p_x+masksize,p_y+masksize);
 
     vsol_box_2d_sptr p_box= new vsol_box_2d(p_box_temp);

     vil_image_view<float> img_mask0;
     vil_image_view<float> img_mask1;
     vil_image_view<float> img_mask2;
     img_mask0.set_size(2*masksize+1, 2*masksize+1, 1);
     img_mask1.set_size(2*masksize+1, 2*masksize+1, 1);
     img_mask2.set_size(2*masksize+1, 2*masksize+1, 1);

     brip_vil_float_ops::chip(p0,p_box,img_mask0);
     brip_vil_float_ops::chip(p1,p_box,img_mask1);
     brip_vil_float_ops::chip(p2,p_box,img_mask2);


    double Ival=0,Hval=0,Sval=0;

    Ival=maskedsmooth(img_mask0,tpmask[k],g2d);
    Hval=maskedsmooth(img_mask1,tpmask[k],g2d);
    Sval=maskedsmooth(img_mask2,tpmask[k],g2d); 
  
    if(isIHS)
    {
      //vcl_cout<<"\n I = "<<Ival<<" H = "<<Hval<<" S =" <<Sval;
        Prcolor.push_back(Ival);
        Pgcolor.push_back(Hval);
        Pbcolor.push_back(Sval);

    }
    else
    {
        double L=0.0,a=0.0,b=0.0;
        utils::IHS2lab(Ival,Hval,Sval,L,a,b);
        Prcolor.push_back(L);
        Pgcolor.push_back(a);
        Pbcolor.push_back(b);
    }

  }
   for(unsigned int k=0;k<negpoints.size();k++)
   {
     int p_x=(int)(negpoints[k]->x());
     int p_y=(int)(negpoints[k]->y());
   

     vbl_bounding_box<double,2> p_box_temp;
     p_box_temp.update(p_x-masksize,p_y-masksize);
     p_box_temp.update(p_x+masksize,p_y+masksize);
 
     vsol_box_2d_sptr p_box= new vsol_box_2d(p_box_temp);

     vil_image_view<float> img_mask0;
     vil_image_view<float> img_mask1;
     vil_image_view<float> img_mask2;
     img_mask0.set_size(2*masksize+1, 2*masksize+1, 1);
     img_mask1.set_size(2*masksize+1, 2*masksize+1, 1);
     img_mask2.set_size(2*masksize+1, 2*masksize+1, 1);

     brip_vil_float_ops::chip(p0,p_box,img_mask0);
     brip_vil_float_ops::chip(p1,p_box,img_mask1);
     brip_vil_float_ops::chip(p2,p_box,img_mask2);

     double Ival=0,Hval=0,Sval=0;

     Ival=maskedsmooth(img_mask0,tpmask[k],g2d);
     Hval=maskedsmooth(img_mask1,tpmask[k],g2d);
     Sval=maskedsmooth(img_mask2,tpmask[k],g2d); 
     
     if(isIHS)
        {
            Nrcolor.push_back(Ival);
            Ngcolor.push_back(Hval);
            Nbcolor.push_back(Sval);
        }
     else
     {
        double L=0.0,a=0.0,b=0.0;
        utils::IHS2lab(Ival,Hval,Sval,L,a,b);
        Nrcolor.push_back(L);
        Ngcolor.push_back(a);
        Nbcolor.push_back(b);
     }
 }
   meanp.push_back(utils::mean(Prcolor));
   meanp.push_back(utils::mean(Pgcolor));
   meanp.push_back(utils::mean(Pbcolor));

   meann.push_back(utils::mean(Nrcolor));
   meann.push_back(utils::mean(Ngcolor));
   meann.push_back(utils::mean(Nbcolor));

   stdp.push_back(utils::std(Prcolor));
   stdp.push_back(utils::std(Pgcolor));
   stdp.push_back(utils::std(Pbcolor));

   stdn.push_back(utils::std(Nrcolor));
   stdn.push_back(utils::std(Ngcolor));
   stdn.push_back(utils::std(Nbcolor));
   
 }
// text description
void dbctrk_curve_description::info()
{
  vcl_cout<<"curve: c ("<<center_.x()<<", "<<center_.y()<<")"; 
  return;
}

void dbctrk_curve_description::compute_bounding_box(vdgl_edgel_chain_sptr const& ec)
{
  vdgl_digital_curve_sptr dc = new vdgl_digital_curve(new vdgl_interpolator_linear(ec));
  dc->compute_bounding_box();
  cbox_= dc->get_bounding_box();
  
  return;
}
void dbctrk_curve_description::compute_cms()
{
 double cmx=0;
 double cmy=0;

 for(int i=1;i<curve_->numPoints();i++)
 {
  cmx+=curve_->point(i).x();
  cmy+=curve_->point(i).y();
  spacing_on_curve=10;
  if(i%spacing_on_curve==0 )
  {
    vgl_point_2d<double> cm(cmx/(double)spacing_on_curve,cmy/(double)spacing_on_curve);
    cms_.push_back(cm);
    cmx=0;
    cmy=0;
  }

 }
}

short dbctrk_curve_description::version() const
{
  return 1;
}
   

//: Binary save self to stream.
void dbctrk_curve_description::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->center_);
  vsl_b_write(os, this->spacing_on_curve);
  vsl_b_write(os, this->angles_);
  vsl_b_write(os, this->grad_);
  vsl_b_write(os, this->cbox_);
  vsl_b_write(os, this->Prcolor);
  vsl_b_write(os, this->Pgcolor);
  vsl_b_write(os, this->Pbcolor);
  vsl_b_write(os, this->Nrcolor);
  vsl_b_write(os, this->Ngcolor);
  vsl_b_write(os, this->Nbcolor);

  vsl_b_write(os, (int)this->maskpos.size());
  for(unsigned int i=0;i<this->maskpos.size();i++)
    vsl_b_write(os, this->maskpos[i]);
  vsl_b_write(os, (int)this->maskneg.size());
  for(unsigned int i=0;i<this->maskneg.size();i++)
    vsl_b_write(os, this->maskneg[i]);

  vsl_b_write(os, (int)this->isvalidmaskpos.size());
  for(unsigned int i=0;i<this->isvalidmaskpos.size();i++)
    vsl_b_write(os, this->isvalidmaskpos[i]);
  vsl_b_write(os, (int)this->isvalidmaskneg.size());
  for(unsigned int i=0;i<this->isvalidmaskneg.size();i++)
    vsl_b_write(os, this->isvalidmaskneg[i]);


  vsl_b_write(os, this->curve_);
  vsl_b_write(os, this->coarser_curve_);
 
}
void dbctrk_curve_description::compute_IHS_histograms(int thetabins,double r1, double v1)
{
    int satbins=(int)vcl_ceil(1/vcl_pow(r1,2));
    int valuebins=(int)vcl_ceil(1/vcl_pow(v1,3));

    chistp.resize(thetabins,satbins,valuebins);
    chistn.resize(thetabins,satbins,valuebins);


    utils::norm3dpolar(Prcolor,Pgcolor,Pbcolor,chistp,thetabins,satbins,valuebins,r1,v1);
    utils::norm3dpolar(Nrcolor,Ngcolor,Nbcolor,chistn,thetabins,satbins,valuebins,r1,v1);
   
}
//: Binary load self from stream.
void dbctrk_curve_description::b_read(vsl_b_istream &is)
{
  if (!is) return;
      int maskpossize;
      int masknegsize;
      vcl_vector<vsol_point_2d_sptr> points;
      vcl_vector<bool> ispoints;
      int isvalidmaskpossize;
      int isvalidmasknegsize;
  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
      
      vsl_b_read(is, this->center_);
      vsl_b_read(is, this->spacing_on_curve);
      vsl_b_read(is, this->angles_);
      vsl_b_read(is, this->grad_);
      vsl_b_read(is, this->cbox_);
      vsl_b_read(is, this->Prcolor);
      vsl_b_read(is, this->Pgcolor);
      vsl_b_read(is, this->Pbcolor);
      vsl_b_read(is, this->Nrcolor);
      vsl_b_read(is, this->Ngcolor);
      vsl_b_read(is, this->Nbcolor);


      vsl_b_read(is, maskpossize);
      for(int i=0;i<maskpossize;i++)
  {
    vsl_b_read(is, points);
    this->maskpos.push_back(points);
  }
      vsl_b_read(is,masknegsize);
      for(int i=0;i<masknegsize;i++)
  {
    vsl_b_read(is,points);
    this->maskneg.push_back(points);
  }
      
      vsl_b_read(is, isvalidmaskpossize);
      for(int i=0;i<isvalidmaskpossize;i++)
  {
    vsl_b_read(is, ispoints);
    this->isvalidmaskpos.push_back(ispoints);
  }
      vsl_b_read(is, isvalidmasknegsize);
      for(int i=0;i<isvalidmasknegsize;i++)
  {
    vsl_b_read(is, ispoints);
    this->isvalidmaskneg.push_back(ispoints);
  }
            vsl_b_read(is, this->curve_);
      vsl_b_read(is, this->coarser_curve_);
      break;
      
  default:
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
vcl_string dbctrk_curve_description::is_a() const
{
  return vcl_string("dbctrk_curve_description");
}

bool dbctrk_curve_description::is_class(vcl_string const& string) const
{
  if(string==is_a())
    return true;
  else
    return false;
}



void vsl_b_write(vsl_b_ostream & os, const dbctrk_curve_description  * p)
{
  
   if (!p){
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}
void vsl_b_read(vsl_b_istream &is,dbctrk_curve_description * &p)
{
  delete(p);
   bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr){
    p = new dbctrk_curve_description();
    p->b_read(is);
  }
  else
    p = 0;
 
}
void dbctrk_curve_description::print_summary(vcl_ostream &os) const
{
  //os<<"\n the id of the curve is "<<this->id_;
}

float dbctrk_curve_description::maskedsmooth(vil_image_view<float> in,
                                           vil_image_view<float> mask ,
                                           vbl_array_2d<float> g2d)
{

    vbl_array_2d<float> temp(g2d);
    float sum=0;

     for(int j=0;j<temp.cols();j++)
        for(int i=0;i<temp.rows();i++)
        {
            temp.put(i,j,g2d(i,j)*mask(i,j));
            sum+=temp(i,j);
        }


     if(sum>0.0)
       {
      for(int j=0;j<temp.cols();j++)
        for(int i=0;i<temp.rows();i++)
            temp.put(i,j,temp(i,j)/sum);

       }

      float prodsum=0.0;
      for(unsigned int j=0;j<in.nj();j++)
          for(unsigned int i=0;i<in.ni();i++)
                prodsum+=temp(i,j)*in(i,j);

      return prodsum;
    

}
