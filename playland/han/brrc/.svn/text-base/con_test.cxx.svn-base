#include "rc3d_windows_frame.h"
#include "probe.h"

vcl_vector <vnl_matrix <float> > rc3d_windows_frame::p_con_test(vcl_vector <vnl_matrix <float> >p_list) //{,
                                                                                                                                                //vcl_vector <bsol_intrinsic_curve_2d_sptr> curve_2d)
{

        vcl_vector <vnl_matrix <float> > p_new_list;
        float x,y,z;
        double cube_inc=1.0;
        
         vnl_matrix <double> R(BB_Rot_);
   vnl_matrix <double> t(3,1,0.0);
   vgl_point_3d<double> X;
   double shift_x=BB_shift_.x(); // bug...should be changed 6-15-2004 fixed 6-16_04
   double shift_y=BB_shift_.y();
   double shift_z=BB_shift_.z();

        for (unsigned i=0;i<p_list.size();i++)
        {
        
                
                t[0][0]=(p_list[i])[7][0];
                t[1][0]=(p_list[i])[8][0];
                t[2][0]=(p_list[i])[9][0];
                vnl_matrix<double> tt4=(R*t);
                //vcl_cout<<t<<vcl_endl;
                X.set(tt4[0][0]+shift_x,tt4[1][0]+shift_y,tt4[2][0]+shift_z);
                x=X.x();
                y=X.y();
                z=X.z();

                x=(p_list[i])[7][0];
                y=(p_list[i])[8][0];
                z=(p_list[i])[9][0];
                
                vcl_cout<<p_list[i]<<vcl_endl;
    vnl_matrix<double> c_all(3,1,0.0);
                vnl_matrix<double> c(3,3,0.0);

                for (unsigned i_frame=0;i_frame<3;i_frame++) {
                        vnl_matrix<double> temp=back_project_direct(i_frame,x,y,z);
                        probe_adjust(temp);
                        //***************
                        //vcl_cout<<temp<<vcl_endl;
                        //***************
                        /*      rec_prob_ret 
                        =recursive_probe_ret_map(d3_robe_[1],d3_robe_[2],d3_robe_[3],
                        temp[0][0],temp[1][0], 0, cube_inc, vtol_tabs_[i_frame],i_frame);
                        
                                
                                        
                                                if (i_frame==0) {x_1=rec_prob_ret(0,0); y_1=rec_prob_ret(1,0);}
                                                if (i_frame==1) {x_2=rec_prob_ret(0,0); y_2=rec_prob_ret(1,0);}
                                                if (i_frame==2) {x_3=rec_prob_ret(0,0); y_3=rec_prob_ret(1,0);}
                                                p_view+=rec_prob_ret(2,0);
                                                
                        */

                        
                
                        for (unsigned j=0;j<curve_2dl_[i_frame]->size();j++) {

                          vnl_matrix<double> c_temp=probe_con_prob(curve_2dl_[i_frame]->x(j),curve_2dl_[i_frame]->y(j),0,temp[0][0],temp[1][0]);
                                c[i_frame][0]+=c_temp[0][0];
                                c[i_frame][1]+=c_temp[1][0];
                                c[i_frame][2]+=c_temp[2][0];
                        }
                
                        //vcl_cout<<c<<vcl_endl;
                        //c_all+=c;
                }
                //////////////////
                vcl_cout<<c<<vcl_endl;
                //////////////////

                // drop no boundary probe ...

                //if (c_all(0,0)==0&&c_all(1,0)==0&&c_all(2,0)==0) ;
          //    else if (c_all(0,0)+c_all(1,0)+c_all(2,0)<30) ;
          //    else if ( c_all(0,0)+c_all(1,0)==0||
                //      c_all(1,0)+c_all(2,0)==0||
                //      c_all(0,0)+c_all(2,0)==0) ;
                //else p_new_list.push_back(p_list[i]);

                
                
                vnl_matrix <unsigned> cm(3,3,0);

                float threshold =3;
                
                for (unsigned k=0;k<3;k++) {
                        for (unsigned l=0;l<3;l++) {

                                if (c(k,l)> threshold) cm(k,l)=1;
                                else cm(k,l)=0;
                        }
                }

                int count=0;
                for (unsigned k=0;k<3;k++) {
                        for (unsigned l=0;l<3;l++) {
                                count+=cm(k,l);
                        }
                }

    
    //cm is 0 or 1 unsigned flag...

                //3-21-2005 indivisual treatment

    
                if (Current_P_==0) {
                
                        if ( (c(0,0)!=0 && c(1,0)!=0 && c(2,0)!=0) )
      {p_new_list.push_back(p_list[i]);

      vcl_cout<<"^^^^^^^^^^^con approved:"<<p_list[i]<<"============================\n"<<vcl_endl;
      }

                }
                else if (Current_P_==1) {
                
                        if ( (c(0,0)!=0 && c(1,0)!=0 && c(2,0)!=0))
      {
          p_new_list.push_back(p_list[i]);

          vcl_cout<<"^^^^^^^^^^^con approved:"<<p_list[i]<<vcl_endl;
        }
                }
                else if (Current_P_==4) 
                {
                                if (count<4) p_new_list.push_back(p_list[i]);

                }
        else if (Current_P_==7) 
                {
                                if ( (c(0,0)!=0 && c(1,0)!=0 && c(2,0)!=0)&&
           (c(0,1)!=0 && c(1,1)!=0 && c(2,1)!=0)&&
            (c(0,2)!=0 && c(1,2)!=0 && c(2,2)!=0)) {
          p_new_list.push_back(p_list[i]);

          vcl_cout<<"^^^^^^^^^^^con approved:"<<p_list[i]<<vcl_endl;
        }

                }
        else if (Current_P_==8) 
                {
                                if ( (c(0,0)!=0 && c(1,0)!=0 && c(2,0)!=0)&&
           (c(0,1)!=0 && c(1,1)!=0 && c(2,1)!=0)&&
            (c(0,2)!=0 && c(1,2)!=0 && c(2,2)!=0)) {
          p_new_list.push_back(p_list[i]);

          vcl_cout<<"^^^^^^^^^con approved:"<<p_list[i]<<vcl_endl;
        }

                }
        else if (Current_P_==9) {
                
                if ( (c(0,0)!=0 && c(1,0)!=0 && c(2,0)!=0)&&
           (c(0,1)!=0 && c(1,1)!=0 && c(2,1)!=0)&&
            (c(0,2)!=0 && c(1,2)!=0 && c(2,2)!=0))
      {
          p_new_list.push_back(p_list[i]);

          vcl_cout<<"^^^^^^^con approved:"<<p_list[i]<<vcl_endl;
        }
                }
                else if (count>4) p_new_list.push_back(p_list[i]);



                
        }
        vcl_cout<<"p_new_list.size()"<<p_new_list.size()<<vcl_endl;
  for (unsigned ia=0;ia<p_new_list.size();ia++) {
    vgl_point_3d <double> pnew(p_new_list[ia][7][0],p_new_list[ia][8][0],p_new_list[ia][9][0]);
    draw_point(PL_,pnew,1,1,0.3,12);
    draw_point(PL_,pnew,0.2,0.2,1,6);

  }
    ////
        return p_new_list;
        
}




//bool rc3d_windows_frame::probe_con_probe()
vnl_matrix<double> rc3d_windows_frame::probe_con_prob(double x,double y,
                             double theta,double x0, double y0)
{

   double c1=0,c2=0,c3=0;
   vnl_matrix<double> c_(3,1,0.0);
   //double dist=2.0;
   double vpi=vnl_math::pi;
    
    double r=vcl_sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0));
   
   //too close to center or outside probe box -> return(0);

   //// 12-26-2003 bug? 
   //if (r<CT_||r>R_) return(c_);
   ///

   if (r<CT_) return(c_);
   //vector component orthogonal to the line..
   float t_x1=(x-x0)*n_x1_, t_y1=-(y-y0)*n_y1_;
   float t_x2=(x-x0)*n_x2_, t_y2=-(y-y0)*n_y2_;
   float t_x3=(x-x0)*n_x3_, t_y3=-(y-y0)*n_y3_;

   // note y axis is reversed..
    float del_x=x-x0,del_y=-(y-y0);
    float del_sum=vcl_sqrt(del_x*del_x+del_y*del_y);
   del_x/=del_sum;del_y/=del_sum;


   // theta is each edgel's orientation...
   double sin_theta=sin(theta*vpi/180.0),cos_theta=cos(theta*vpi/180.0);

   double tt1=sin_theta*th1_x_+cos_theta*th1_y_;
   double tt2=sin_theta*th2_x_+cos_theta*th2_y_;
   double tt3=sin_theta*th3_x_+cos_theta*th3_y_;
   double udm_flag=r;
   if (udm_) udm_flag=1.0;//uniform distance measure...
   /*
   if (del_x*(th1_x_)+del_y*(th1_y_)>0.8) {
      if (vcl_sqrt(t_x1*t_x1+t_y1*t_y1)<d_*r&&vcl_fabs(tt1)>AT_)
         c1++;
   }
   if (del_x*(th2_x_)+del_y*(th2_y_)>0.8) {
      if (vcl_sqrt(t_x2*t_x2+t_y2*t_y2)<d_*r&&vcl_fabs(tt2)>AT_) 
         c2++;
   }
   if (del_x*(th3_x_)+del_y*(th3_y_)>0.8) {
      //vcl_cout<<theta<<" ";
      if (vcl_sqrt(t_x3*t_x3+t_y3*t_y3)<d_*r&&vcl_fabs(tt3)>AT_) 
         c3++;
   }*/

         
   double dist1=t_x1+t_y1;
         
   
         if (vcl_fabs(dist1)<d_*udm_flag&&r<R1_)
                 c1++;
         
         
   double dist2=t_x2+t_y2;
         
         if (vcl_fabs(dist2)<d_*udm_flag&&r<R2_) 
                 c2++;
         
         
   double dist3=t_x3+t_y3;
         
         //vcl_cout<<theta<<" ";
         if (vcl_fabs(dist3)<d_*udm_flag&&r<R3_) 
                 c3++;

   
   
   c_(0,0)=c1;c_(1,0)=c2;c_(2,0)=c3;
   //return(c1+c2+c3);
   return(c_);
}
