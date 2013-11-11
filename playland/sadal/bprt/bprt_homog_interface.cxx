
#include "bprt_homog_interface.h"
#include <rrel/rrel_lms_obj.h>
#include <rrel/rrel_ransac_obj.h>
#include <rrel/rrel_trunc_quad_obj.h>
#include <rrel/rrel_mlesac_obj.h>
#include <rrel/rrel_muset_obj.h>
#include <vnl/vnl_det.h>


void 
bprt_homog_interface::set_method(vcl_string &method_name)
{
        method_ = method_name;
        
}

bool 
bprt_homog_interface::compute_homog()
{
        vcl_string method = method_;
        leftover1 = pointlist1_;
        leftover2 = pointlist2_;
        int plane_ind = -1;
        


        
        
        if (dof_>4)
        {
            while((plane_ind<(*max_planes_)-1) )
            {
            plane_ind++;
           
            if  ( (leftover1.size()<8) && (leftover2.size()<8) )
            {
                 vcl_cout<<"Number of planes assumed:   "<<*max_planes_<<"\n";
                *max_planes_ = plane_ind;
                break;
            }
            else 
                vcl_cout<<"Number of planes assumed:   "<<*max_planes_<<"\n";
                double max_outlier_frac = 0.5;
                double desired_prob_good = 0.99999;
                int max_pops = *max_planes_;
                int trace_level = 0;
                vcl_cout<<"\n"<<"bprt_homog_interface::compute_homog()-- No of pts in homography:"<<leftover1.size()<<"   "<<leftover2.size()<<"\n";

                if (use_epipole_)
                    Homog_est = new rrel_homography2d_est_epipole(leftover1,leftover2, epi1,epi2);
                else 
                if (!affine_)
                Homog_est = new rrel_homography2d_est( leftover1, leftover2 );
                    else
                        Homog_est = new rrel_homography2d_est_aff(leftover1 ,leftover2);

                rrel_ran_sam_search* ransam = new rrel_ran_sam_search;
                ransam->set_sampling_params( max_outlier_frac, desired_prob_good, max_pops);
                if (method=="lms")
                {
                        int num_dep_res = Homog_est->num_samples_to_instantiate();
                        rrel_objective* lms = new rrel_lms_obj( num_dep_res );
                
                        Homog_est->set_no_prior_scale();
                        if ( !ransam->estimate( Homog_est, lms ) )
                        {
                                vcl_cout << "LMS failed!!\n";
                                delete lms;
                                delete ransam;
                                return false;
                        }
                        else {
                                vcl_cout << "LMS succeeded.\n"
                                        << "estimate = " << ransam->params() << vcl_endl
                                        << "scale = " << ransam->scale() << vcl_endl;
                        }
                        vcl_cout << vcl_endl;
                        delete lms;
                }
                else if (method=="muset")
                {
                        Homog_est->set_prior_scale( prior_scale );
                                        
                        rrel_muset_obj* muset = new rrel_muset_obj( leftover1.size()+1 );
                        ransam->set_sampling_params( 1 - muset->min_inlier_fraction(),
                                desired_prob_good,
                                max_pops);
                        //muset->set_muse_type(RREL_MUSE_TRIMMED);
                        if ( !ransam->estimate( Homog_est, muset ) )
                        {
                                vcl_cout << "MUSE failed!!\n";
                               
                                if (plane_ind==0)
                                { 
                                    delete muset;
                               delete ransam;
                                return false;
                                }
                                else 
                                {*max_planes_= plane_ind;
                                                                vcl_cout<<"\nWarning :only found "<<plane_ind<<"  planes\n";

                                    continue;
                                }
                        }
                        else {
                                vcl_cout << "MUSE succeeded.\n"
                                        << "estimate = " << ransam->params() << vcl_endl
                                        << "scale = " << ransam->scale() << vcl_endl;
                        }
                        vcl_cout << vcl_endl;
                        delete muset;
                }
                        //
                        //      MSAC
                        //
                else if  (method=="msac")
                {
                        
                        rrel_trunc_quad_obj* msac = new rrel_trunc_quad_obj();
                        Homog_est->set_prior_scale( prior_scale );
                        if ( !ransam->estimate( Homog_est, msac ) )
                        {
                                vcl_cout << "MSAC failed!!\n";
                                delete msac;
                                delete ransam;
                                return false;
                        }
                        else {
                                vcl_cout << "MSAC succeeded.\n"
                                        << "estimate = " << ransam->params() << vcl_endl
                                        << "scale = "    << ransam->scale()  << vcl_endl;
                                delete msac;
                        }
                }
                        
                        
                        //
                        //      MLESAC
                        //
                else if  (method=="mlesac")
                {
                        int residual_dof = Homog_est->residual_dof();
                        rrel_mlesac_obj* mlesac = new rrel_mlesac_obj( residual_dof );
                        Homog_est->set_prior_scale( prior_scale );
                        
                        
                        if ( !ransam->estimate( Homog_est, mlesac ) )
                        {
                                vcl_cout << "MLESAC failed!!\n";
                                delete ransam;
                                delete mlesac;
                                return false;
                        }
                        else {
                                vcl_cout << "MLESAC succeeded.\n"
                                        << "estimate = " << ransam->params() << vcl_endl
                                        << "scale = " << ransam->scale() << vcl_endl;
                                delete mlesac;
                        }
                        vcl_cout << vcl_endl;
                        
                }
                        
                        
                else if  (method=="ransac")
                {
                        rrel_ransac_obj* ransac = new rrel_ransac_obj();
                        Homog_est->set_prior_scale( prior_scale );
                        
                        
                        ransam->set_gen_all_samples();
                        ransam->set_trace_level(trace_level);
                        
                        
                        if ( !ransam->estimate(Homog_est, ransac ) )
                        {
                                vcl_cout << "RANSAC failed!!\n";
                                delete ransam;
                                delete ransac;
                                return false;
                        }
                        else {
                                vcl_cout << "RANSAC succeeded.\n"
                                        << "estimate = " << ransam->params() << vcl_endl
                                        << "scale = " << ransam->scale() << vcl_endl;
                                delete ransac;          
                        }
                }
                vnl_matrix<double> Hmat(3,3);
                Homog_est->params_to_homog(ransam->params(),Hmat);
                NormH_=Hmat; 
                NormHvgl_ = vgl_h_matrix_2d<double> (NormH_);
                vcl_cout<<"Calc H by "<<method<<"\n";
                
                vgl_h_matrix_2d<double> invtr=normtr2_.get_inverse();
                if (normalize_)
                FullH_=vgl_h_matrix_2d<double>(invtr*NormH_*normtr1_);
               
                else 
                    FullH_ = NormH_;
                vcl_cout<<"homography"<<FullH_<<"\n";
                //IMPORTANT Make surre  the found homography is pushed_back before get_outliers function
                if (normalize_)
                    Homog_matlist.push_back(NormHvgl_);
                else
                Homog_matlist.push_back(FullH_);

                vcl_cout<<"Determinant of homography:"<<vnl_det(FullH_.get_matrix())<<"\n";
                vcl_vector<vcl_vector<vgl_homg_point_2d< double > > > outlier_vec = this->get_outliers(ransam->scale(),2.5, plane_ind);
                for (int i = 0; i<outlier_vec[0].size(); i++)
                {
                    vcl_cout<<"\nOutlier  x:"<<(normtr1_.get_inverse()*outlier_vec[0][i]).x()<<" y: "<< (normtr1_.get_inverse()*outlier_vec[0][i]).y()<<"\t";
                    vcl_cout<<"Outlier  x:"<<(normtr2_.get_inverse()*outlier_vec[1][i]).x()<<" y: "<< (normtr2_.get_inverse()*outlier_vec[1][i]).y()<<"\t";

                }
                vcl_cout<<"\n";
               leftover1 = outlier_vec[0];
               leftover2 = outlier_vec[1];
                           vcl_cout<<"Number of left points1: "<<leftover1.size()<<"\n";
                           vcl_cout<<"Number of left points2: "<<leftover2.size()<<"\n";

                delete ransam;
                      
                
        }
        }
        else
                {
                vgl_h_matrix_2d<double> NormH_temp;
                CalcLin.compute(pointlist1_,pointlist2_,NormH_temp);
                NormH_ = NormH_temp.get_matrix();
                vgl_h_matrix_2d<double> invtr=normtr2_.get_inverse();
                if (normalize_)
                    FullH_=vgl_h_matrix_2d<double>(invtr*NormH_*normtr1_);
                else 
                    FullH_ = NormH_;
                vcl_cout<<"Calc H by 4points\n";
                vcl_cout<<FullH_;
                
                }

        
        return true;
        }
void 
bprt_homog_interface::transfer(vcl_vector< vsol_spatial_object_2d_sptr > &pt_2btransf1, vcl_vector< vsol_spatial_object_2d_sptr > &pt_2btransf2, int populnum)
{
        int num_pt_2btransf1=pt_2btransf1.size();
        int num_pt_2btransf2=pt_2btransf2.size();
        if (dof_>4)
        {
                for(int i=0;i<num_pt_2btransf1;i++)
                        output2_.push_back(new vsol_point_2d(Homog_matlist[populnum]*(vgl_homg_point_2d<double> (pt_2btransf1[i]->cast_to_point()->x() , pt_2btransf1[i]->cast_to_point()->y()))));
                for(int i=0;i<num_pt_2btransf2;i++)
                        output1_.push_back(new vsol_point_2d(Homog_matlist[populnum].preimage(vgl_homg_point_2d<double> (pt_2btransf2[i]->cast_to_point()->x() , pt_2btransf2[i]->cast_to_point()->y()))));
                
        }
        else

                {
                for(int i=0;i<num_pt_2btransf1;i++)
                        output2_.push_back(new vsol_point_2d(FullH_*vgl_homg_point_2d<double> (pt_2btransf1[i]->cast_to_point()->x() , pt_2btransf1[i]->cast_to_point()->y() )  ));
                for(int i=0;i<num_pt_2btransf2;i++)
                        output1_.push_back(new vsol_point_2d(FullH_.preimage(vgl_homg_point_2d<double> (pt_2btransf2[i]->cast_to_point()->x() , pt_2btransf2[i]->cast_to_point()->y() ) ) ) );
                }

        
}


void
 bprt_homog_interface::get_output(vcl_vector< vsol_spatial_object_2d_sptr> &out,int i)
{
        if (i==1)
        {
                out=output1_;
        //      for (int k=0;k<num_points;k++)
                //      vcl_cout<<output1_[k]->cast_to_point()->x()<<"  "<<output1_[k]->cast_to_point()->y()<<"  ";
        }
        
        if (i==2)
        {
                out=output2_;
        //      for (int k=0;k<num_points;k++)
        //              vcl_cout<<output2_[k]->cast_to_point()->x()<<"  "<<output2_[k]->cast_to_point()->y()<<"  ";
        }
        
}

vnl_double_3x3
 bprt_homog_interface::get_H(int index)
    {
    return Homog_matlist[index].get_matrix();
    }
vgl_h_matrix_2d<double> 
bprt_homog_interface::get_Homog()
    {
    return FullH_;
    }
vgl_h_matrix_2d<double>
 bprt_homog_interface::get_Homog(int index)
{
    return Homog_matlist[index];
}

// Calculates outliers and returns them
// Also calculates inliers and puts them  in pops1_ and pops2_ as a vector of vgl_homg_2d which belong to one population

vcl_vector<vcl_vector<vgl_homg_point_2d<double> > > 
bprt_homog_interface::get_outliers(double scale, double mult_scale,int popindex)
{   
    vcl_vector<vcl_vector<vgl_homg_point_2d<double> > > outliers;
    vcl_vector<vgl_homg_point_2d<double> > outliers1, outliers2;
    vcl_vector<vgl_homg_point_2d<double> > inliers1, inliers2;
    vnl_vector<double> params(9);
    vcl_vector<double> residuals;

    Homog_est->homog_to_params(NormH_,params);
    Homog_est->compute_residuals(params , residuals);
    vcl_vector<vsol_spatial_object_2d_sptr> transferpop1,transferpop2;
        outlier_points1.clear();
        outlier_points2.clear();
    int inliers =0;

    for (int i = 0; i<leftover1.size();i++)
    {
                
        if (residuals[i]>(mult_scale*scale))
        {
            outliers1.push_back(leftover1[i]);
            outliers2.push_back(leftover2[i]);
                        outlier_points1.push_back(new vsol_point_2d(leftover1[i]));
                        outlier_points2.push_back(new vsol_point_2d(leftover2[i]));
        }
        else 
        {
            inliers1.push_back(leftover1[i]);
            inliers2.push_back(leftover2[i]);
            inliers++;
            vgl_homg_point_2d<double> tempvglpt1 = normtr2_.get_inverse()*get_Homog(popindex)*leftover1[i];
            vgl_homg_point_2d<double> tempvglpt2 = normtr1_.get_inverse()*get_Homog(popindex).preimage(leftover2[i]);
            transferpop1.push_back(new vsol_point_2d(tempvglpt2));
            transferpop2.push_back(new vsol_point_2d(tempvglpt1));
        }
        
    }
    transferpopulationlist1.push_back (transferpop1);
    transferpopulationlist2.push_back (transferpop2);
        assert(transferpopulationlist1.size() == popindex+1);
        assert(transferpopulationlist2.size() == popindex+1);
    outliers.push_back(outliers1);
    outliers.push_back(outliers2);
    pops1_.push_back(inliers1);
    pops2_.push_back(inliers2);
    vcl_cout<<"number of inliers: "<<inliers<<"\n";
        vcl_cout<<"number of outliers: "<<outliers1.size()<<"\n";

    return outliers;
}
vcl_vector<vsol_spatial_object_2d_sptr > 
bprt_homog_interface::get_pop(int imgnum, int popnum)
{
    assert((pops1_.size()>popnum)&&(pops2_.size()>popnum));
    vcl_vector<vsol_spatial_object_2d_sptr> pops1_so;
    vcl_vector<vsol_spatial_object_2d_sptr> pops2_so;
    
    if (imgnum==1)
    {
       
        for (int m = 0; m<(pops1_[popnum]).size(); m++)
        {
            assert(pops1_[popnum][m].w()!=0);
            pops1_so.push_back(new vsol_point_2d(normtr1_.get_inverse()*pops1_[popnum][m]) );
            
        }

        return  pops1_so;
    }
    else
    {
       
        for (int m = 0; m<(pops2_[popnum]).size(); m++)
        {
            assert(pops2_[popnum][m].w()!=0);
            
            
            pops2_so.push_back(new vsol_point_2d(normtr2_.get_inverse()*pops2_[popnum][m]) );
        }
 
        return  pops2_so;
    }

}

vcl_vector<vsol_spatial_object_2d_sptr> 
bprt_homog_interface::get_transf_pop(int imnum,int popnum)
{

      if (imnum==1)
    {
       return  transferpopulationlist1[popnum];
    }
    else
    {
       
        return  transferpopulationlist2[popnum];
    }
    

}
void
bprt_homog_interface::set_epipole(vgl_homg_point_2d<double> epipole1, vgl_homg_point_2d<double> epipole2)
{
    epi1 = epipole1;
    epi2 = epipole2;


}

void
bprt_homog_interface::get_outlier_points( vcl_vector<vsol_point_2d_sptr> &outliers_from_first_homog1, vcl_vector<vsol_point_2d_sptr> &outliers_from_first_homog2)
{
//Check to see if the point was  added before
        //will be a problem because of floating point errors

outliers_from_first_homog1.clear();
outliers_from_first_homog2.clear();
    bool added_before;
    for (int i =0; i<outlier_points1.size();i++)
    {
        added_before= false;
        for (int m= 0; m<i; m++)
        {
            if (outlier_points1[i]==outlier_points1[m])
                added_before= true;
        }
        if (!added_before) 
          outliers_from_first_homog1.push_back(new vsol_point_2d(normtr1_.get_inverse()*vgl_homg_point_2d<double>(outlier_points1[i]->x(),outlier_points1[i]->y())));

    }
         vcl_cout<<outliers_from_first_homog1.size()<<"  Number of outlier in first image\n";
        for (int i =0; i<outlier_points2.size();i++)
    {
        added_before= false;
        for (int m= 0; m<i; m++)
        {
            if (outlier_points2[i]==outlier_points2[m])
                added_before= true;
        }
        if (!added_before) 
          outliers_from_first_homog2.push_back(new vsol_point_2d(normtr2_.get_inverse()*vgl_homg_point_2d<double>(outlier_points2[i]->x(),outlier_points2[i]->y())));

    }
        vcl_cout<<outliers_from_first_homog2.size()<<"  Number of outlier in first image\n";
}
