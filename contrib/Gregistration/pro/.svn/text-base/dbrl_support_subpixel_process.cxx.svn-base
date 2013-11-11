
#include "dbrl_support_subpixel_process.h"
#include <bpro1/bpro1_parameters.h>
#include <brip/brip_vil_float_ops.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <pro/dbrl_id_point_2d_storage_sptr.h>
#include <pro/dbrl_id_point_2d_storage.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vnl/vnl_math.h>
#include <vul/vul_sprintf.h>

//: Constructor
dbrl_support_subpixel_process::dbrl_support_subpixel_process(void): bpro1_process()
    {
        if( !parameters()->add( "Radius of Neighborhood" , "-radius" , (float)5)||
            !parameters()->add( "Std Dev for proximity (spatial)" , "-ssigd" , (float)5)||
            !parameters()->add( "Threshold (spatial)" , "-sthresh" , (float)0.5)||
            !parameters()->add( "Sigma Distance (temporal)" , "-tsigd" , (float)0.5)||
            !parameters()->add( "Sigma Angle (temporal)" , "-tsiga" , (float)0.3)||
            !parameters()->add( "Threshold (temporal)" , "-tthresh" , (float)0.5)||
            !parameters()->add( "No of Iterations" , "-numiter" , (int)3)||
            !parameters()->add( "Is Cocirc?" , "-iscocirc" , (bool)true))
            {
                
            }
        image_list_.clear();
        points_list_.clear();
        framenums_.clear();
    }

//: Destructor
dbrl_support_subpixel_process::~dbrl_support_subpixel_process()
    {

    }


//: Return the name of this process
vcl_string
dbrl_support_subpixel_process::name()
    {
    return "Estimating Support Subpixel";
    }


//: Return the number of input frame for this process
int
dbrl_support_subpixel_process::input_frames()
    {
    return 1;
    }


//: Return the number of output frames for this process
int
dbrl_support_subpixel_process::output_frames()
    {
    return 1;

    }


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_support_subpixel_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_support_subpixel_process::get_output_type()
    {  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "vsol2D" );
        to_return.push_back( "vsol2D" );

    return to_return;
    }


//: Execute the process
bool
dbrl_support_subpixel_process::execute()
    {
        if ( input_data_.size() != 1 ){
            vcl_cout << "In dbrl_support_subpixel_process::execute() - "
                << "not exactly two input images \n";
            return false;
        }
        // get image from the storage class
        vidpro1_vsol2D_storage_sptr input_vsol;
        input_vsol.vertical_cast(input_data_[0][0]);

        static float rad_=10;
        static float std=5;
        static int numiter=3;
        bool iscocirc_=true;
        static float sthresh=0.3;

        parameters()->get_value("-radius",rad_);
        parameters()->get_value("-numiter",numiter);
        parameters()->get_value("-ssigd",std);
        parameters()->get_value("-sthresh",sthresh);

        static float tsigd=0.5;
        static float tsiga=0.3;
        static float tthresh=0.3;

        parameters()->get_value("-tsigd",tsigd);
        parameters()->get_value("-tsiga",tsiga);
        parameters()->get_value("-tthresh",tthresh);

        parameters()->get_value("-iscocirc",iscocirc_);
        vcl_vector<vcl_string> groups=input_vsol->groups();
        vcl_vector< vcl_vector< dbrl_id_point_2d_sptr > > allpts;
        int count=0;
        for (unsigned c=0;c<groups.size();c++)
        {
            vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->data_named(groups[c]);
                    vcl_vector< dbrl_id_point_2d_sptr > pts;

            for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) {
                if( vsol_list[b]->cast_to_curve()){
                    //LINE
                    if( vsol_list[b]->cast_to_curve()->cast_to_line() ){
                        vsol_line_2d_sptr eline = vsol_list[b]->cast_to_curve()->cast_to_line();
                        vgl_point_2d<double> spt(eline->p0()->x(), eline->p0()->y());
                        vgl_point_2d<double> ept(eline->p1()->x(), eline->p1()->y());
                        vgl_point_2d<double> pt(eline->middle()->x(), eline->middle()->y());
                        double tan = vcl_atan2 (ept.y() - spt.y(), ept.x() - spt.x()) ;
                        pts.push_back(new dbrl_id_point_2d(pt.x(),pt.y(), tan,count));
                        //pts[count]->set_weight(0.0);
                        count++;
                    }
                }
            }
           allpts.push_back(pts);
        }
        
        compute_temporal_support(tsigd,tsiga,tthresh,allpts);
        compute_spatial_support(std,sthresh,allpts);

        return true;  
}
//: Clone the process
bpro1_process*
dbrl_support_subpixel_process::clone() const
    {
    return new dbrl_support_subpixel_process(*this);
    }

bool
dbrl_support_subpixel_process::finish()
    {
    return true;
    }
bool
dbrl_support_subpixel_process::compute_temporal_support(double sigma_d, double sigma_a,double thresh,vcl_vector< vcl_vector< dbrl_id_point_2d_sptr > >pts)
{
    vcl_vector< dbrl_id_point_2d_sptr > allpts;
    for(unsigned i=0;i<pts.size();i++)
        allpts.insert(allpts.end(),pts[i].begin(),pts[i].end());

    
    double *weightst=new double[allpts.size()];
    for(unsigned j=0;j<allpts.size();j++)
        weightst[j]=0.0;

    for(unsigned j=0;j<allpts.size()-1;j++)
    {
        double xj=allpts[j]->x();
        double yj=allpts[j]->y();
        double tj=allpts[j]->tangent();
        for(unsigned i=j+1;i<allpts.size();i++)
        {
            double xi=allpts[i]->x();
            double yi=allpts[i]->y();
            double ti=allpts[i]->tangent();

            double st=vcl_exp(-((xi-xj)*(xi-xj)+(yi-yj)*(yi-yj))/(sigma_d*sigma_d) -(tj-ti)*(tj-ti)/(sigma_a*sigma_a));
            weightst[i]+=st;
            weightst[j]+=st;
        }
    }

    vcl_vector<vsol_spatial_object_2d_sptr> vsol_out;
    for(unsigned j=0;j<allpts.size();j++)
    {
        if(weightst[j] >thresh)
        {
            vsol_point_2d_sptr p=new vsol_point_2d(allpts[j]->x(),allpts[j]->y());
            vsol_point_2d_sptr ps=new vsol_point_2d(p->x()+0.5*vcl_cos(allpts[j]->tangent()),p->y()+0.5*vcl_sin(allpts[j]->tangent()));
            vsol_point_2d_sptr pe=new vsol_point_2d(p->x()-0.5*vcl_cos(allpts[j]->tangent()),p->y()-0.5*vcl_sin(allpts[j]->tangent()));
            vsol_line_2d_sptr l=new vsol_line_2d(ps,pe);
            vsol_out.push_back(p->cast_to_spatial_object());
            vsol_out.push_back(l->cast_to_spatial_object());
        }  
    }

  vidpro1_vsol2D_storage_sptr output_vsol_temporal = vidpro1_vsol2D_storage_new();
  output_vsol_temporal->add_objects(vsol_out);
  output_data_[0].push_back(output_vsol_temporal);

  return true;

}

bool
dbrl_support_subpixel_process::compute_spatial_support(double sigma_d,double thresh,vcl_vector< vcl_vector< dbrl_id_point_2d_sptr > > pts)
{
    unsigned totsize=0;

    vcl_vector< dbrl_id_point_2d_sptr > allpts;
    for(unsigned i=0;i<pts.size();i++)
    {
        totsize+=pts[i].size();
        allpts.insert(allpts.end(),pts[i].begin(),pts[i].end());
    }
    double *weightst=new double[totsize];

    for(unsigned j=0;j<totsize;j++)
        weightst[j]=0.0;

    for(unsigned j=0;j<totsize;j++) {
        double xj=allpts[j]->x();
        double yj=allpts[j]->y();
        double tj=allpts[j]->tangent();

        for(unsigned i=0;i<pts.size();i++){
            double st=0.0;
            for(unsigned k=0;k<pts[i].size();k++){
               
                    double xi=pts[i][k]->x();
                    double yi=pts[i][k]->y();
                    double ti=pts[i][k]->tangent();


                    st+=compute_support(xj,yj,tj,xi,yi,ti,sigma_d);//vcl_exp(-((xi-xj)*(xi-xj)+(yi-yj)*(yi-yj))/(sigma_d*sigma_d) -(tj-ti)*(tj-ti)/(sigma_a*sigma_a));

                
            }
            if(weightst[j]<st)
                weightst[j]=st;

        }
    }

    vcl_vector<vsol_spatial_object_2d_sptr> vsol_out;
    for(unsigned j=0;j<totsize;j++)
    {
        if(weightst[j] >thresh)
        {
            vsol_point_2d_sptr p=new vsol_point_2d(allpts[j]->x(),allpts[j]->y());
            vsol_point_2d_sptr ps=new vsol_point_2d(p->x()+0.2*vcl_cos(allpts[j]->tangent()),p->y()+0.2*vcl_sin(allpts[j]->tangent()));
            vsol_point_2d_sptr pe=new vsol_point_2d(p->x()-0.2*vcl_cos(allpts[j]->tangent()),p->y()-0.2*vcl_sin(allpts[j]->tangent()));
            vsol_line_2d_sptr l=new vsol_line_2d(ps,pe);
            vsol_out.push_back(p->cast_to_spatial_object());
            vsol_out.push_back(l->cast_to_spatial_object());
        }  
    }

  vidpro1_vsol2D_storage_sptr output_vsol_temporal = vidpro1_vsol2D_storage_new();
  output_vsol_temporal->add_objects(vsol_out);
  output_data_[0].push_back(output_vsol_temporal);

  return true;

}
