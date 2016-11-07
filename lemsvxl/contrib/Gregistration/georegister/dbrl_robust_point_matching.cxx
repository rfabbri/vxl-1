#include "dbrl_robust_point_matching.h"
#include "dbrl_feature.h"
#include "dbrl_feature_point.h"
#include "dbrl_feature_point_tangent.h"
#include "dbrl_feature_point_tangent_curvature.h"
#include "dbrl_feature_point_tangent_curvature_groupings.h"
#include <vcl_cassert.h>
#include <vnl/vnl_transpose.h>
//#include <dbgl/algo/dbgl_circ_arc.h>
//#include <dbgl/algo/dbgl_closest_point.h>
#include <vnl/vnl_math.h>

dbrl_robust_point_matching_params::dbrl_robust_point_matching_params()
    {
    }
dbrl_robust_point_matching_params::dbrl_robust_point_matching_params(double Tstart,double Tend,
                                 double outlierval,double annealrate)
    {
        Tstart_=Tstart;
        Tend_=Tend;
        annealrate_=annealrate;
        outlierval_=outlierval;
    }
dbrl_robust_point_matching::dbrl_robust_point_matching()
    {
    }

vnl_matrix<double> dbrl_robust_point_matching::compute_correspondence_weights(double T,
                                                          vcl_vector<dbrl_feature_sptr> f1,
                                                          vcl_vector<dbrl_feature_sptr> f2)
    {
        assert(f1.size()>0);
        assert(f2.size()>0);
        assert(T>0.0);
        int m=f1.size();
        int n=f2.size();
        double *f1y=new double[f1.size()];
        double *f1x=new double[f1.size()];

        double *f2y=new double[f2.size()];
        double *f2x=new double[f2.size()];

        for(unsigned i=0;i<f1.size();i++)
            {
                if(dbrl_feature_point* p=dynamic_cast<dbrl_feature_point*>(f1[i].ptr()))
                    {
                    f1x[i]=p->location()[0];
                    f1y[i]=p->location()[1];
                    }
                else if(dbrl_feature_point_tangent * pt=dynamic_cast<dbrl_feature_point_tangent*>(f1[i].ptr()))
                    {
                    f1x[i]=pt->location()[0];
                    f1y[i]=pt->location()[1];
                    } 
                else if(dbrl_feature_point_tangent_curvature * pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f1[i].ptr()))
                {
                    f1x[i]=pt->location()[0];
                    f1y[i]=pt->location()[1];
                } 

            }

        for(unsigned i=0;i<f2.size();i++)
            {
                if(dbrl_feature_point* p=dynamic_cast<dbrl_feature_point*>(f2[i].ptr()))
                    {
                    f2x[i]=p->location()[0];
                    f2y[i]=p->location()[1];
                    }
                  else if(dbrl_feature_point_tangent * pt=dynamic_cast<dbrl_feature_point_tangent*>(f2[i].ptr()))
                    {
                    f2x[i]=pt->location()[0];
                    f2y[i]=pt->location()[1];
                    } 
                  else if(dbrl_feature_point_tangent_curvature * pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2[i].ptr()))
                  {
                      f2x[i]=pt->location()[0];
                      f2y[i]=pt->location()[1];
                  } 

            }
        vnl_matrix<double> d(m,n,0.0);//=fpt1*vnl_transpose(fpt2);
        double *d_data_block=d.data_block();
        for(unsigned i=0;i<static_cast<unsigned>(m);i++)
            for(unsigned j=0;j<static_cast<unsigned>(n);j++)
                d_data_block[i*n+j]=vcl_exp(-vcl_sqrt((f1x[i]-f2x[j])*(f1x[i]-f2x[j])+(f1y[i]-f2y[j])*(f1y[i]-f2y[j]))/T);

        delete f1x;
        delete f1y;
        delete f2x;
        delete f2y;

        return d;
    }

double dbrl_robust_point_matching::distPointLineSegment(double ptx,double pty, 
                             double lstartx,double lstarty, 
                             double lendx,double lendy)

{

    //Ken: divide is slow.
    double l = vcl_sqrt((lendy-lstarty)*(lendy-lstarty) + 
        (lendx-lstartx)*(lendx-lstartx));

    double t = ((pty-lstarty)*(lendy-lstarty) + 
        (           ptx-lstartx)*(lendx-lstartx))/l;

    if (t<0)
        return vcl_sqrt((pty-lstarty)*(pty-lstarty) +(ptx-lstartx)*(ptx-lstartx));
    else if (t>l)
        return vcl_sqrt((pty-lendy)*(pty-lendy) + (ptx-lendx)*(ptx-lendx));
    else
        return vcl_fabs((pty-lstarty)*(lendx-lstartx) - (ptx-lstartx)*(lendy-lstarty))/l;

}

//; projected distance of point1 from point2 
double dbrl_robust_point_matching:: projected_distance (double x1,double y1,double theta1,double x2,double y2,double theta2)

{

    double sigma_e=5.0;
    double sigma_p=2.0;
    double sigma_t=vnl_math::pi/6;

    double d_e=(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);

    double startx2=x2-1.0*vcl_sin(theta2);
    double starty2=y2+1.0*vcl_cos(theta2);
    double endx2=x2+1.0*vcl_sin(theta2);
    double endy2=y2-1.0*vcl_cos(theta2);

    double l = vcl_sqrt((endy2-starty2)*(endy2-starty2) + 
                        (endx2-startx2)*(endx2-startx2));

    double d_p= vcl_fabs((y1-starty2)*(endx2-startx2) - (x1-startx2)*(endy2-starty2))/l;
    double d_theta=(theta1-theta2)*(theta1-theta2);

    double d_t=(d_e/(2*sigma_e*sigma_e)+d_p*d_p/(2*sigma_p*sigma_p)+0.5*d_theta/(2*sigma_t*sigma_t));
    return d_t;

}



vnl_matrix<double> dbrl_robust_point_matching::compute_correspondence_point_tangent_weights(double T,
                                                          vcl_vector<dbrl_feature_sptr> f1,
                                                          vcl_vector<dbrl_feature_sptr> f2)
    {
        assert(f1.size()>0);
        assert(f2.size()>0);
        assert(T>0.0);

        int m=f1.size();
        int n=f2.size();
        double *f1y=new double[f1.size()];
        double *f1x=new double[f1.size()];
        double *tangent1=new double[f1.size()];

        double *lstartx=new double[f2.size()];
        double *lstarty=new double[f2.size()];

        double *lendx=new double[f2.size()];
        double *lendy=new double[f2.size()];
        double *tangent2=new double[f2.size()];

        double *f2x=new double[f2.size()];
        double *f2y=new double[f2.size()];

        for(unsigned i=0;i<f1.size();i++)
            {
                if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f1[i].ptr()))
                    {
                    f1x[i]=pt->location()[0];
                    f1y[i]=pt->location()[1];
                    tangent1[i]=pt->dir();
                    }
                    if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f1[i].ptr()))
                    {
                    f1x[i]=pt->location()[0];
                    f1y[i]=pt->location()[1];
                    tangent1[i]=pt->dir();
                    }
            }

        for(unsigned i=0;i<f2.size();i++)
            {
                if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f2[i].ptr()))
                    {
                    f2x[i]=pt->location()[0];
                    f2y[i]=pt->location()[1];
                    tangent2[i]=pt->dir();
                    lstartx[i]=f2x[i]-1.0*vcl_sin(tangent2[i]);
                    lstarty[i]=f2y[i]+1.0*vcl_cos(tangent2[i]);
                    lendx[i]=f2x[i]+1.0*vcl_sin(tangent2[i]);
                    lendy[i]=f2y[i]-1.0*vcl_cos(tangent2[i]);
                    }
                if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2[i].ptr()))
                {
                    f2x[i]=pt->location()[0];
                    f2y[i]=pt->location()[1];
                    tangent2[i]=pt->dir();
                    lstartx[i]=f2x[i]-1.0*vcl_sin(tangent2[i]);
                    lstarty[i]=f2y[i]+1.0*vcl_cos(tangent2[i]);
                    lendx[i]=f2x[i]+1.0*vcl_sin(tangent2[i]);
                    lendy[i]=f2y[i]-1.0*vcl_cos(tangent2[i]);
                }

            }
        vnl_matrix<double> d(m,n,0.0);
        double *d_data_block=d.data_block();
        for(unsigned i=0;i<m;i++)
            for(unsigned j=0;j<n;j++)
                {
                //double dist=distPointLineSegment (f1x[i],f1y[i], lstartx[j], lstarty[j], 
                //                                  lendx[j],lendy[j]);
                double dist=projected_distance(f1x[i],f1y[i], tangent1[i], 
                                                  f2x[j],f2y[j], tangent2[j]);

               //     vcl_fabs((f1x[i]-f2x[j])*vcl_sin(tangent1[i])-(f1y[i]-f2y[j])*vcl_cos(tangent1[i]));
                d_data_block[i*n+j]=vcl_exp(-dist/(T*T));

                }

        delete [] f1x;
        delete [] f1y;
        delete [] f2x;
        delete [] f2y;
        delete [] tangent2;
        return d;
    }


vnl_matrix<double> 
dbrl_robust_point_matching::compute_correspondence_point_tangent_curvature_weights(double T,
                                                                                   vcl_vector<dbrl_feature_sptr> f1,
                                                                                   vcl_vector<dbrl_feature_sptr> f2)
    {
        assert(f1.size()>0);
        assert(f2.size()>0);
        assert(T>0.0);

        int m=f1.size();
        int n=f2.size();
        double *f1y=new double[f1.size()];
        double *f1x=new double[f1.size()];

        double *lstartx=new double[f2.size()];
        double *lstarty=new double[f2.size()];

        double *lendx=new double[f2.size()];
        double *lendy=new double[f2.size()];

        double *tangent2=new double[f2.size()];

        double *f2x=new double[f2.size()];
        double *f2y=new double[f2.size()];

        for(unsigned i=0;i<f1.size();i++)
        {
            if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f1[i].ptr()))
            {
                f1x[i]=pt->location()[0];
                f1y[i]=pt->location()[1];
            }
        }

        //for(unsigned i=0;i<f2.size();i++)
        //{
        //    if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2[i].ptr()))
        //    {
        //        if(pt->k_flag())
        //        {
        //            
        //        }
        //        else
        //        {
        //        f2x[i]=pt->location()[0];
        //        f2y[i]=pt->location()[1];
        //        tangent2[i]=pt->dir();
        //        lstartx[i]=f2x[i]-1.0*vcl_sin(tangent2[i]);
        //        lstarty[i]=f2y[i]+1.0*vcl_cos(tangent2[i]);
        //        lendx[i]=f2x[i]+1.0*vcl_sin(tangent2[i]);
        //        lendy[i]=f2y[i]-1.0*vcl_cos(tangent2[i]);
        //        }
        //    }
        //}
        vnl_matrix<double> d(m,n,0.0);
        double *d_data_block=d.data_block();
        for(unsigned i=0;i<m;i++)
            for(unsigned j=0;j<n;j++)
            {
                double dist=0.0;
                if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2[j].ptr()))
                {
                    double x2=pt->location()[0];
                    double y2=pt->location()[1];
                    if(pt->k_flag())
                    {
                        //dbgl_circ_arc carc=pt->get_arc();
                        double len=0;
                        //dist=dbgl_closest_point::point_to_circular_arc(vgl_point_2d<double>(f1x[i],f1y[i]),carc.point1(),carc.point2(),carc.k(),len);
                    }
                    else
                    {
                        double tangent2=pt->dir();
                        double startx=x2-1.0*vcl_sin(tangent2);
                        double starty=y2+1.0*vcl_cos(tangent2);
                        double endx=x2+1.0*vcl_sin(tangent2);
                        double endy=y2-1.0*vcl_cos(tangent2);
                        dist=distPointLineSegment (f1x[i],f1y[i],startx, starty,endx,endy);
                    }
                }
                //double dist=distPointLineSegment (f1x[i],f1y[i], lstartx[j], lstarty[j], 
                //    lendx[j],lendy[j]);
                ////     vcl_fabs((f1x[i]-f2x[j])*vcl_sin(tangent1[i])-(f1y[i]-f2y[j])*vcl_cos(tangent1[i]));
                d_data_block[i*n+j]=vcl_exp(-dist/T);
            }
            delete [] f1x;
            delete [] f1y;
            delete [] f2x;
            delete [] f2y;
            delete [] tangent2;
            return d;
    }




vnl_matrix<double> 
dbrl_robust_point_matching::compute_neighborhood_weights(double T,
                                                         vcl_vector<dbrl_feature_sptr> f1,
                                                         vcl_vector<dbrl_feature_sptr> f2, 
                                                         vnl_matrix<double> dist)
                                                         
    {
        //if(type=="Euclidean")
        //{
        vnl_matrix<double> dnew=dist;
        //vcl_cout<<"\n Before : \n"<<d;
        for(unsigned i=0;i<dist.rows();i++)
        {
            if(dbrl_feature_point_tangent_curvature_groupings* pt1
                =dynamic_cast<dbrl_feature_point_tangent_curvature_groupings*>(f1[i].ptr()))
            {
                for(unsigned j=0;j<dist.cols();j++)
                {
                    double tempcollect=0.0;
                    if(dbrl_feature_point_tangent_curvature_groupings* pt2
                        =dynamic_cast<dbrl_feature_point_tangent_curvature_groupings*>(f2[j].ptr()))
                    {
                        for(unsigned cnti=0;cnti<pt1->feature_neighbor_map_.size();cnti++)
                        {
                            unsigned  m1=pt1->feature_neighbor_map_[cnti];
                            for(unsigned cntj=0;cntj<pt2->feature_neighbor_map_.size();cntj++)
                            {
                                unsigned  n1=pt2->feature_neighbor_map_[cntj];
                                tempcollect+=1/5*dist(m1,n1);
                            }
                        }
                    }
                    dnew(i,j)=dnew(i,j)+tempcollect;
                }
            }
        }
        //vcl_cout<<"\n After : \n"<<dnew;
        return dnew;

        //}
        //else if(type=="Projected")
        //{
        //    vnl_matrix<double> d=compute_correspondence_point_tangent_weights(T,f1,f2);
        //    for(unsigned i=0;i<d.rows();i++)
        //    {
        //        if(dbrl_feature_point_tangent_curvature_groupings* pt1=dynamic_cast<dbrl_feature_point_tangent_curvature_groupings*>(f1[i].ptr()))
        //        {
        //            for(unsigned j=0;j<d.cols();j++)
        //            {
        //                if(dbrl_feature_point_tangent_curvature_groupings* pt2
        //                    =dynamic_cast<dbrl_feature_point_tangent_curvature_groupings*>(f2[j].ptr()))
        //                {
        //                    for(unsigned cnti=0;cnti<pt1->feature_neighbor_map_.size();cnti++)
        //                    {
        //                        unsigned  m1=pt1->feature_neighbor_map_[cnti]-1;
        //                        for(unsigned cntj=0;cntj<pt2->feature_neighbor_map_.size();cntj++)
        //                        {
        //                            unsigned  n1=pt2->feature_neighbor_map_[cntj]-1;
        //                            d(i,j)=d(i,j)+0.5*d(m1,n1);

        //                        }
        //                    }

        //                }
        //            }
        //        }
        //    }
        //    return d;

        //}

        //return vnl_matrix<double>();
}


