#ifndef bprt_homog_interface_h_
#define bprt_homog_interface_h_



#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_norm_trans_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vnl/vnl_double_3x3.h>
#include <rrel/rrel_homography2d_est.h>
#include <rrel/rrel_homography2d_est_aff.h>
#include "rrel_homography2d_est_epipole.h"
#include <rrel/rrel_ransac_obj.h>
#include <rrel/rrel_ran_sam_search.h>

class bprt_homog_interface
{
      
public:
        bprt_homog_interface(vcl_vector< vsol_point_2d_sptr > &storage1,
                        vcl_vector< vsol_point_2d_sptr > &storage2, 
                        unsigned int dof, int *max_num_planes, float std,
                        bool affine= false, bool normalize = false, bool use_correlate = true, 
                        bool use_epipole = true):
        method_("muset")                   
        
        {
        dof_=dof;
        prior_scale=std;
        affine_ = affine;
        normalize_ = normalize;
        use_correlate_ =use_correlate;
        max_planes_ = max_num_planes;
        num_points=storage1.size();
        use_epipole_ = use_epipole;
        if (storage2.size()<num_points)
        {
                num_points=storage2.size();
                vcl_cout<<"Warning: bprt_homog_interface::bprt_homog_interface() Not same amount of points in list of points\n";
        }
        else 
            if (storage2.size()>num_points)
            {
                    vcl_cout<<"Warning: bprt_homog_interface::bprt_homog_interface() Not same amount of points in list of points\n";
            }
        


        for (int i=0;i<num_points;i++)
                pointlist1_.push_back(vgl_homg_point_2d<double> (storage1[i]->x(),storage1[i]->y()));
        if (num_points>storage1.size())
        for (int i=0;i<num_points-storage1.size();i++)
                pointlist1_.push_back(vgl_homg_point_2d<double> (storage1[i]->x(),storage1[i]->y()));
        
        for (int i=0;i<num_points;i++)
                pointlist2_.push_back(vgl_homg_point_2d<double> (storage2[i]->x(),storage2[i]->y()));
        if (num_points>storage2.size())
        for (int i=0;i<num_points-storage2.size();i++)
                pointlist2_.push_back(vgl_homg_point_2d<double> (storage2[i]->x(),storage2[i]->y()));
        bool tr1found=normtr1_.compute_from_points(pointlist1_);
        bool tr2found=normtr2_.compute_from_points(pointlist2_);
        if (!tr1found)
        {
                normtr1_.set_identity();
                vcl_cout<<"Warning: bprt_homog_interface::bprt_homog_interface() couldn't compute normalizing transform for pointlist1 /n";
        }
        if (!tr2found)
        {
                normtr2_.set_identity();
                vcl_cout<<"Warning: bprt_homog_interface::bprt_homog_interface() couldn't compute normalizing transform for pointlist2 /n";
        }

        if (tr1found&&tr2found&&normalize_)
        {
            double cx= 0.0, cy =0.0;
            double cx2 =0.0, cy2 = 0.0;
            vcl_cout<<"norm1";
            for (int m= 0; m<3; m++)
            {
                for (int n= 0; n<3; n++)
                    vcl_cout<<normtr1_.get(m,n)<<"\t";
                    ;
                vcl_cout<<"\n";
            }
            
            for (int m= 0; m<3; m++)
            {
                for (int n= 0; n<3; n++)
                    vcl_cout<<normtr2_.get(m,n)<<"\t";
                 vcl_cout<<"\n";

            }

            for (int i=0;i<pointlist1_.size();++i)
            {
                cx += pointlist1_[i].x()/pointlist1_.size();
                cy += pointlist1_[i].y()/pointlist1_.size();
                cx2 +=  pointlist2_[i].x()/pointlist2_.size();
                cy2 +=  pointlist2_[i].y()/pointlist2_.size();
              

            }
              vcl_cout<<"Centroid 1:  "<< cx<<"   "<<cy<<"\n";
                vcl_cout<<"Centroid 2:  "<< cx2<<"   "<<cy2<<"\n";


             for (int i=0;i<pointlist1_.size();++i)
                {
                        pointlist1_[i]=normtr1_(pointlist1_[i]);
                        pointlist2_[i]=normtr2_(pointlist2_[i]);
                }
            cx= 0.0; 
            cy =0.0;
            cx2 =0.0;
            cy2 = 0.0;

              for (int i=0;i<pointlist1_.size();++i)
            {
                cx += pointlist1_[i].x()/pointlist1_.size();
                cy += pointlist1_[i].y()/pointlist1_.size();
                cx2 +=  pointlist2_[i].x()/pointlist2_.size();
                cy2 +=  pointlist2_[i].y()/pointlist2_.size();

            }
                vcl_cout<<"Centroid 1:  "<< cx<<"   "<<cy<<"\n";
                vcl_cout<<"Centroid 2:  "<< cx2<<"   "<<cy2<<"\n";

        }
        
        }

        bprt_homog_interface(
                vcl_vector< vsol_point_2d_sptr > &storage1,
                vcl_vector< vsol_point_2d_sptr > &storage2)
        {
                prior_scale=1;
                dof_=4;
                method_="4pt";
                normalize_ = true;
                num_points=storage1.size();
                max_planes_ =new int(1) ;
                if (storage2.size()<num_points)
                {
                    num_points=storage2.size();
                    vcl_cout<<"Warning: bprt_homog_interface::bprt_homog_interface() Not same amount of points in list of points\n";
                }
                else 
                    if (storage2.size()>num_points)
                    {
                    vcl_cout<<"Warning: bprt_homog_interface::bprt_homog_interface() Not same amount of points in list of points\n";
                    }
                for (int i=0;i<num_points;i++)
                        pointlist1_.push_back(vgl_homg_point_2d<double> (storage1[i]->x(),storage1[i]->y()));
                if (num_points>storage1.size())
                        for (int i=0;i<num_points-storage1.size();i++)
                pointlist1_.push_back(vgl_homg_point_2d<double> (storage1[i]->x(),storage1[i]->y()));
        
                for (int i=0;i<num_points;i++)
                        pointlist2_.push_back(vgl_homg_point_2d<double> (storage2[i]->x(),storage2[i]->y()));
                if (num_points>storage2.size())
                        for (int i=0;i<num_points-storage2.size();i++)
                        pointlist2_.push_back(vgl_homg_point_2d<double> (storage2[i]->x(),storage2[i]->y()));
                bool normalized1, normalized2;
                normalized1 = true;
                normalized2 = true;
                if (!normtr1_.compute_from_points(pointlist1_))
                {
                    normalized1= false;
                        normtr1_.set_identity();
                        vcl_cout<<"Warning: bprt_homog_interface::bprt_homog_interface() couldn't compute normalizing transform for pointlist1 /n";
                }
                if (!normtr2_.compute_from_points(pointlist2_))
                {
                        normalized2 = false; 
                        normtr2_.set_identity();
                        vcl_cout<<"Warning: bprt_homog_interface::bprt_homog_interface() couldn't compute normalizing transform for pointlist2 /n";
                }
                if (normalized1 && normalized2)
                    normalize_ = true;
                for (int i=0;i<pointlist1_.size();++i)
                {
                        
                        pointlist1_[i]=normtr1_(pointlist1_[i]);
                        pointlist2_[i]=normtr2_(pointlist2_[i]);
                }

        }

        ~bprt_homog_interface()
        {
        }

        bool compute_homog();
        void set_method(vcl_string &method_name);
        void get_output(vcl_vector< vsol_spatial_object_2d_sptr> &out,int i);
        void output(vcl_vector< vsol_spatial_object_2d_sptr > &pt_2btransf1, vcl_vector< vsol_spatial_object_2d_sptr > &pt_2btransf2);
        void transfer(vcl_vector< vsol_spatial_object_2d_sptr > &pt_2btransf1, vcl_vector< vsol_spatial_object_2d_sptr > &pt_2btransf2, int numpopul);
        vcl_vector<vcl_vector<vgl_homg_point_2d<double> > > get_outliers(double scale, double mult_scale,int popindex);
        vnl_double_3x3 get_H(int index);
        vgl_h_matrix_2d<double> get_Homog();
        vcl_vector< vsol_spatial_object_2d_sptr > get_pop(int imnum, int popnum);
        vcl_vector< vsol_spatial_object_2d_sptr > get_transf_pop(int imnum, int popnum);
        vgl_h_matrix_2d<double> get_Homog(int index);
        void get_outlier_points(vcl_vector< vsol_point_2d_sptr> &out,vcl_vector< vsol_point_2d_sptr> &out2);
        void set_epipole(vgl_homg_point_2d<double> epipole1, vgl_homg_point_2d<double> epipole2);



        private:
        rrel_homography2d_est *Homog_est;
        vcl_vector< vgl_homg_point_2d <double > > pointlist1_,pointlist2_;
        vcl_vector< vcl_vector< vsol_spatial_object_2d_sptr > > transferpopulationlist1,transferpopulationlist2;
      
        vnl_double_3x3 H_;
        vgl_h_matrix_2d_compute_4point CalcLin;
        vgl_h_matrix_2d <double> FullH_;
        vcl_vector<vsol_spatial_object_2d_sptr> output1_, output2_;

        vnl_double_3x3  NormH_;
        vgl_h_matrix_2d<double> NormHvgl_;
        vgl_norm_trans_2d<double> normtr1_;
        vgl_norm_trans_2d<double> normtr2_;
        vcl_vector< vcl_vector<  vgl_homg_point_2d <double > > > pops1_,pops2_;
        vcl_vector<vgl_h_matrix_2d<double> > Homog_matlist;
        vcl_vector<vgl_homg_point_2d<double> > leftover1, leftover2;
                vcl_vector<vsol_point_2d_sptr> outlier_points1,outlier_points2;
        double prior_scale;
        vgl_homg_point_2d<double> epi1;
        vgl_homg_point_2d<double> epi2;



        unsigned int dof_;
        
        int num_points;
        bool affine_;
        vcl_string method_;
        bool use_correlate_;
        bool normalize_;
        bool use_epipole_;
        int *max_planes_;

};


#endif
