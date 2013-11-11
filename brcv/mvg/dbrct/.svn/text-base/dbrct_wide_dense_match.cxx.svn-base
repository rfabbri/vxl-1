#include <dbrct/dbrct_wide_dense_match.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_interpolate.h>
#include <vsrl/vsrl_image_correlation.h>
#include <vsrl/vsrl_raster_dp_setup.h>
#include <vnl/vnl_vector.h>
//: constructor
dbrct_wide_dense_match::dbrct_wide_dense_match():params(),f_assignments()
{
}

//: constructor with images initialized
dbrct_wide_dense_match::dbrct_wide_dense_match(vil1_image imgl,vil1_image imgr):f_assignments()
{
    imgl_=imgl;
    imgr_=imgr;

}
//: constructor with images initialized
dbrct_wide_dense_match::dbrct_wide_dense_match(vil1_image imgl,vil1_image imgr, int no_of_levels,int samplingrate):params(no_of_levels,samplingrate),f_assignments()
{
    imgl_=imgl;
    imgr_=imgr;

    orig_w_=imgl_.width();
    orig_h_=imgl_.height();

   
}

//: destructor
dbrct_wide_dense_match::~dbrct_wide_dense_match()
{
    
}

bool dbrct_wide_dense_match::scale_img(vil1_image img_in,int scale,vil1_image & img_out)
{
    if(!img_in)
        return false;

    if(scale<=0)
     return false;

    int width=(int)((double)img_in.width()/(double)scale);
    int height=(int)((double)img_in.height()/(double)scale);

    vil1_memory_image_of<unsigned char> img(img_in);
    vil1_memory_image_of<unsigned char> buffer(width,height);
        // Zero out the buffer
     for (int x=0,i=0;i<buffer.width();i++)
     {
         for (int y=0,j=0;j<buffer.height();j++)
        {
             buffer(i,j)=  img(x,y);     
            y+=scale;
        }
        x+=scale;
     }

     img_out=buffer;

     return true;

}


//: scale both left and right  images 
bool dbrct_wide_dense_match::downsample_images()
{
 if(!imgl_ || !imgr_)
     return false;
  vil1_image origl=imgl_;
 vil1_image origr=imgr_;
 scaledimgsl.push_back(origl);
 scaledimgsr.push_back(origr);

 if(params.levels<=1)
     return true;


 for (int i=0;i<params.levels-1;i++)
 {
         vil1_image templ;
         vil1_image tempr;

         if(scale_img(origl,params.coarsestscale,templ) 
                 && scale_img(origr,params.coarsestscale,tempr))
         {
                scaledimgsl.push_back(templ);
                scaledimgsr.push_back(tempr);
         }
    origl=templ;
    origr=tempr;
 }
 return true;
}

//: dense matching for pyramid
bool dbrct_wide_dense_match::do_dense_matching()
{
        vnl_matrix<int> assignforlevel;
    vnl_matrix<int> levelassignment;
    for(int i=params.levels-1;i>=0;i--)
        {
                //: special case to handle the first low res img
        if(i==params.levels-1)
        {
            assignforlevel.set_size(scaledimgsl[i].height(),scaledimgsl[i].width());
            for( int j=0;j<scaledimgsl[i].height();j++)
            {
                for( int k=0;k<scaledimgsl[i].width();k++)
                {
                    assignforlevel(j,k)=k;
                }
            }
        }
        else
        {
            assignforlevel.set_size(scaledimgsl[i].height(),scaledimgsl[i].width());
            assignforlevel.fill(-1);
            interp_correspondence(levelassignment,assignforlevel);
          
        }
        vcl_cout<<" densematching for level no "<<i<<"\n";
      
        vsrl_image_correlation i_corr(scaledimgsl[i],scaledimgsr[i] );
                i_corr.set_correlation_range(params.paramslist[i].correlation_range);
                i_corr.set_window_width(params.paramslist[i].correlation_window_width);
                i_corr.set_window_height(params.paramslist[i].correlation_window_height);
                i_corr.initial_calculations();

        //: setting the dp raster
                int num_raster_ = i_corr.get_image1_height();
                typedef vsrl_raster_dp_setup* raster_ptr;
                vsrl_raster_dp_setup **raster_array_ = new raster_ptr[num_raster_];
                for (int k=0;k<num_raster_;k++)
                        raster_array_[k]=0;
        
                for (int k=0;k<num_raster_;k++)
                {
                vsrl_raster_dp_setup *raster = new vsrl_raster_dp_setup(k, &i_corr);
                raster->set_inner_cost(params.paramslist[i].inner_cost);
                raster->set_outer_cost(params.paramslist[i].outer_cost);

                if (k>0)
                        if (raster_array_[k-1])
                    raster->set_prior_raster(raster_array_[k-1]);
            if (k<num_raster_-1)
                    if (raster_array_[k+1])
                        raster->set_prior_raster(raster_array_[k+1]);
                // set the correlation range for the raster
                raster->set_search_range(int(params.paramslist[i].correlation_range/2));
                // performing the dynamic program
            vnl_vector<int> ri=assignforlevel.get_row(k);
                raster->execute(ri);
                raster_array_[k]=raster;
                vcl_cout<<"\n row number "<<k;
        }
        
                //: get the assignment
                levelassignment.set_size(i_corr.get_image1_height(),i_corr.get_image1_width());
        vil1_memory_image_of<double> buffer(scaledimgsl[i].width(),scaledimgsl[i].height());
        for(int j=0;j<num_raster_;j++)
        {     
            for(int k=0;k<i_corr.get_image1_width();k++)
          {
              buffer(k,j)=raster_array_[j]->get_assignment(k);
              //vcl_cout<<buffer(k,j)<<" ";
          }
         // vcl_cout<<"\n";
        }

        for(int j=0;j<buffer.height();j++)
        {
            int prior_i_=0;
            int post_i_=0;
            for(int l=0;l<buffer.width();l++)
            {
                if(buffer(l,j)<0)
                   {
                    for(int k=l;k<buffer.width();k++)
                    {
                        if(buffer(k,j)>=0)
                        {
                        post_i_=(int)buffer(k,j);
                        break;
                        }
                     }
                    buffer(l,j)=prior_i_;
                   }
                 else
                    prior_i_=(int)buffer(l,j);
             }
        }
        for(int j=0;j<buffer.height();j++)
            for(int k=0;k<buffer.width();k++)
                levelassignment(j,k)=(int)buffer(k,j);

        if(i==0)
            f_assignments=levelassignment;

        }
    return true;
}
bool dbrct_wide_dense_match::interp_correspondence(vnl_matrix<int> iassignment,vnl_matrix<int> & fassignment)
{
    //fassignment.set_size(orig_h_,orig_w_);
    //fassignment.fill(-1);
    unsigned int h=fassignment.rows();
    unsigned int w=fassignment.cols();
    vil1_memory_image_of<double> buffer(iassignment.cols(),iassignment.rows());
    for(unsigned int j=0;j<iassignment.rows();j++) 
        for(unsigned int i=0;i<iassignment.cols();i++)
               buffer(i,j)=iassignment(j,i) ;
    unsigned int scale_=params.coarsestscale;
    
    for (int j=0;j<buffer.height()-1;j++)   
     {
         for (int i=0;i<buffer.width()-1;i++)
         {
            double step=1/(double)scale_;
            for(unsigned int x=0;x<scale_;x++)
            {
                for(unsigned int y=0;y<scale_;y++)
                {
                    double out=0;
                    if(vil1_interpolate_bilinear<double,double>(buffer,(double)i+x*step,(double)j+y*step,&out)) 
                        if(j*scale_+y<h && j*scale_+y>=0 && i*scale_+x<w && i*scale_+x>=0)
                         fassignment(j*scale_+y,i*scale_+x)= (int)(out*scale_ +0.5);
                }
            }
        }
     }
 
      
      h=buffer.height()-1;
      w=buffer.width()-1;

      for(unsigned int j=0;j<fassignment.rows();j++)
          for(unsigned int i=0;i<fassignment.cols();i++)
          {
            if(fassignment(j,i)<0 && j<h*scale_)
                fassignment(j,i)=fassignment(j,i-1);
            else if(fassignment(j,i)<0 && i<w*scale_)
                fassignment(j,i)=fassignment(j-1,i);
            else if(fassignment(j,i)<0)
                fassignment(j,i)=fassignment(j-1,i-1);
          }

 return true;
}
//: to display any vnl matrix
 void dbrct_wide_dense_match::display_matrices(vnl_matrix<int> p)
 {
    vcl_ostringstream os;
    p.print(os);
    vcl_cout<<os.str();
 }
//: function to get the final corrrespondence in stereo images
 vnl_matrix<int> dbrct_wide_dense_match::get_correspondence()
 {
    return f_assignments;
 }
