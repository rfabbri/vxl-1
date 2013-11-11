#define DEBUG false

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_float_3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_inverse.h>

#include <dbecl/dbecl_episeg_point.h>
#include <dbecl/dbecl_episeg.h>
#include <dbecl/dbecl_episeg_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vgl/vgl_point_3d.h>

#include <bmrf/bmrf_network.h>
#include <bmrf/bmrf_curve_3d_builder.h>
#include <bmrf/bmrf_curvel_3d.h>
#include <bmrf/bmrf_curve_3d.h>
#include <sdet/sdet_detector_params.h>


//#include "../../dbcri/pro/dbcri_process.h"
#include "dbcri_whole_process.h"
#include <bmvl/brct/brct_algos.h>


void dbcri_whole_process::read_edge_map()

{
  vgl_point_2d<double> epipole(epi_x_,epi_y_);
  vcl_cout<<"epipole: "<<epipole<<vcl_endl;
  vgl_point_2d <double> uppoint(1000.0,(-1000.0f*line_upper_.a()-line_upper_.c())/line_upper_.b());
  vgl_point_2d <double> lowpoint(1000.0,(-1000.0f*line_lower_.a()-line_lower_.c())/line_lower_.b());
  vsol_line_2d_sptr newhline_upper= new vsol_line_2d(epipole,uppoint);
  //newhline_upper->vgl_hline_2d();
  vsol_line_2d_sptr newhline_lower= new vsol_line_2d(epipole,lowpoint);


  int frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();


    //add to the storage class
    output_vsol->add_object( newhline_upper->cast_to_spatial_object() ,"output_upperline" );
    output_vsol->add_object( newhline_lower->cast_to_spatial_object() ,"output_lowerline" );
    // output_data_[frame].push_back(output_vsol);


    vcl_vector< vsol_spatial_object_2d_sptr > sh_pts;

    vidpro1_vtol_storage_sptr frame_vtol;

    ////frame_image.vertical_cast(input_data_[frame][0]);
    vcl_cout<<output_data_.size()<<vcl_endl;
    vcl_cout<<output_data_[frame].size()<<vcl_endl;
    vcl_cout<<input_data_[frame][0]<<vcl_endl;
    frame_vtol.vertical_cast(input_data_[frame][2]);
    vcl_cout<<frame_vtol<<vcl_endl;
    vcl_vector <vsol_digital_curve_2d_sptr>  dclist;

    vcl_vector<vtol_edge_2d_sptr> edges;


    for ( vcl_set<vtol_topology_object_sptr>::const_iterator itr = frame_vtol->begin();
      itr != frame_vtol->end();  ++itr ) {

        vtol_edge *edge = (*itr)->cast_to_edge();
        if (edge){
          vtol_edge_2d *edge_2d = edge->cast_to_edge_2d();
          if (edge_2d){
            vsol_curve_2d_sptr c = edge_2d->curve();

            vdgl_digital_curve_sptr vdc = c->cast_to_vdgl_digital_curve();
            if(vdc){
              // Extract the points along the edgel chain and make a vsol_digital_curve_2d
              vdgl_edgel_chain_sptr ec = vdc->get_interpolator()->get_edgel_chain();
              vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d;


              ///// length >> )
              if (ec->size()<20) continue;
              for(unsigned int i=0; i<ec->size(); ++i){

                // For some reason vtol edges often contain duplicate points
                if ( dc->size() > 0 && dc->p1()->get_p() == ec->edgel(i).get_pt() )
                  continue;  

                // if outside contour ignore..
                /////if ( !veh_cons_[frame].contains((*ec)[i].get_x(),(*ec)[i].get_y()) )
                /////  continue;


                double xx=(*ec)[i].get_x();double yy=(*ec)[i].get_y();

                //vcl_cout<<veh_con_pol_x[frame][0]<<" "<<veh_con_pol_y[frame][0]<<vcl_endl;
                //vcl_cout<<veh_con_pol_x[frame][100]<<" "<<veh_con_pol_y[frame][100]<<vcl_endl;



                bool distance_measure;
                parameters()->get_value( "-edge_distance" , distance_measure );
                float distance_measure2;
                parameters()->get_value( "-edge_distance2" , distance_measure2 );

                distance_measure=false;
                if (distance_measure) //
                {
                  /////            double distance_from_contour = vgl_distance_to_closed_polygon( veh_con_pol_x[frame] ,veh_con_pol_y[frame] ,Nl[frame],xx ,yy);

                  /////            if (distance_from_contour>distance_measure2) continue;


                }
                dc->add_vertex(new vsol_point_2d(ec->edgel(i).get_pt()));
                // vcl_cout<< (*ec)[i].get_x()<<" "<<(*ec)[i].get_y()<<vcl_endl;

                sh_pts.push_back(new vsol_point_2d(ec->edgel(i).get_pt()));
              }

              dclist.push_back(dc);

            }
          }
        }
      }

      dcl_edge_.push_back(dclist);
      output_vsol->add_objects(sh_pts);
      output_data_[frame].push_back(output_vsol);

  }


}
  
///// cross ratio calculetion

double dbcri_whole_process::cameras_fast1(int fm1, int fm2, int fm3,
                                         vcl_vector <vsol_digital_curve_2d_sptr> &dcl_,
                                         vcl_vector <vcl_vector <dbecl_episeg_sptr> > &episeglist,
                                         dbecl_epipole_sptr &epipole)
{

  vnl_double_3x3 M_in;
  M_in[0][0] = 2000.0; M_in[0][1] = 0;        M_in[0][2] = 1280.0f/2.0f;//512.0;
  M_in[1][0] = 0;        M_in[1][1] = 2000.0; M_in[1][2] = 768.0f/2.0f;//384.0;
  M_in[2][0] = 0;        M_in[2][1] = 0;      M_in[2][2] = 1;

  vnl_double_3x4 E,E1,E2,E3;  


  // get translation
  //trans_dist = 125.0; // 105mm

  vnl_double_3 epi;

  double xe=epi_x_;
  double ye=epi_y_;
  double ze=1.0f;

  epi[0]=epi_x_; epi[1]=epi_y_; epi[2]=1.0f;

  double x0,y0,x1,y1,x2,y2;
  bool flag=false;
  char ch;
  vnl_double_3 T;


  vnl_double_3x4 C;

  C[0][0]= 2.71954;    C[0][1]= 12.1025;    C[0][2]=0;         C[0][3]=63.9305;
  C[1][0]=-1.00509;    C[1][1]= 0.757923;   C[1][2]=-11.6824;  C[1][3]= 159.883;
  C[2][0]=-0.00516136; C[2][1]= 0.0026911;  C[2][2]= 0;        C[2][3]= 0.33245;


  C=WC_;

  //vcl_cout<<C<<vcl_endl;
  vnl_double_3x3 M = C.extract(3,3);

  //T = vnl_inverse(M_in) * epi;
  T = vnl_inverse(M) * epi;
  double T_normal=vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
  T /= T_normal;
  //T *= trans_dist;



  E1[0][0] = 1;   E1[0][1] = 0;    E1[0][2] = 0;      E1[0][3] = 0;
  E1[1][0] = 0;   E1[1][1] = 1;    E1[1][2] = 0;      E1[1][3] = 0;
  E1[2][0] = 0;   E1[2][1] = 0;    E1[2][2] = 1;      E1[2][3] = 0;

  E3[0][0] = 1;   E3[0][1] = 0;    E3[0][2] = 0;      E3[0][3] = T[0];
  E3[1][0] = 0;   E3[1][1] = 1;    E3[1][2] = 0;      E3[1][3] = T[1];
  E3[2][0] = 0;   E3[2][1] = 0;    E3[2][2] = 1;      E3[2][3] = T[2];


  vnl_double_3x4 P1 = M_in*E1,/*P2 =M_in_*E2,*/ P3=M_in*E3;





  double ratio;
  vnl_double_3 ratio_3;
  vcl_vector <ratios> ratios_front_list;
  vcl_vector <ratios> ratios_rear_list;

  bool selected0,selected1,selected2;
  double distance0,distance1,distance2;

  vcl_vector <vgl_point_3d<double> > point_3d_front_list, point_3d_rear_list;
  dbecl_episeg_point  x0pt,x1pt,x2pt;
  //dbecl_episeg_point  x0pt(episeglist[fm1][i0], episeglist[fm1][i0]->index(ang));



  int angle_index=-1;

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_-MARGIN_ ; ang<-theta_neg_+MARGIN_ ; ang+=ang_ /*was .002*/) { 

    angle_index++;
    selected0=false; 
    selected1=false;
    selected2=false;
    x0=-100.0f;x1=-100.0f;x2=-100.f;
    //vcl_cout<<ang<<" "<<episeglist[fm1].size()<<" "<<episeglist[fm2].size()<<" "<<episeglist[fm3].size()<<vcl_endl;

    for (unsigned i0=0;i0<episeglist[fm1].size();i0++) {

      //vcl_cout<<episeglist[fm1][i0]->min_angle()<<" "<<episeglist[fm1][i0]->max_angle()<<vcl_endl;
      if (episeglist[fm1][i0]->min_angle() <=ang&&episeglist[fm1][i0]->max_angle() >=ang){
        //if (episeglist[fm1][i0]->index(ang)) {

        if (episeglist[fm1][i0]->dist(ang)>x0){
          x0=episeglist[fm1][i0]->dist(ang);
          selected0=true;
          x0pt.set_point(episeglist[fm1][i0],episeglist[fm1][i0]->index(ang));
        }
      }
    }

    for (unsigned i1=0;i1<episeglist[fm2].size();i1++) {
      //vcl_cout<<episeglist[fm2][i1]->min_angle()<<" "<<episeglist[fm2][i1]->max_angle()<<vcl_endl;
      if (episeglist[fm2][i1]->min_angle() <=ang&&episeglist[fm2][i1]->max_angle() >=ang) {
        //if (episeglist[fm2][i1]->index(ang)) {
        if (episeglist[fm2][i1]->dist(ang)>x1) {
          x1=episeglist[fm2][i1]->dist(ang);
          selected1=true;
          x1pt.set_point(episeglist[fm2][i1],episeglist[fm2][i1]->index(ang));
        }
      }
    }

    for (unsigned i2=0;i2<episeglist[fm3].size();i2++) {
      //vcl_cout<<episeglist[fm3][i2]->min_angle()<<" "<<episeglist[fm3][i2]->max_angle()<<vcl_endl;
      if (episeglist[fm3][i2]->min_angle() <=ang&&episeglist[fm3][i2]->max_angle() >=ang) {
        //if (episeglist[fm3][i2]->index(ang)) {
        if (episeglist[fm3][i2]->dist(ang)>x2) {
          x2=episeglist[fm3][i2]->dist(ang);
          selected2=true;
          x2pt.set_point(episeglist[fm3][i2],episeglist[fm3][i2]->index(ang));

          ///////////////debug
          //vcl_cout<<episeglist[fm3][i2]->min_angle()<<" "<<episeglist[fm3][i2]->max_angle()<<vcl_endl;
          //vcl_cout<<episeglist[fm3][i2]<<" "<<episeglist[fm3][i2]->index(ang)<<vcl_endl;
        }
      }
    }

    // if 3 points are not determined continue
    if (!selected0 || !selected1 || !selected2) continue;

    /////////////////////////
    /////////////////////////
    ///////////////////////// angle flag



    /////////////////////////
    /////////////////////////
    /////////////////////////

    // double c=vcl_sqrt(vcl_pow(x2-xe,2)+vcl_pow(y2-ye,2));
    // double b=vcl_sqrt(vcl_pow(x1-xe,2)+vcl_pow(y1-ye,2));
    // double a=vcl_sqrt(vcl_pow(x0-xe,2)+vcl_pow(y0-ye,2));
    double c=x2; double  b=x1;double a=x0;

    double cx=vcl_fabs(x2-xe),cy=vcl_fabs(y2-ye);
    double bx=vcl_fabs(x1-xe),by=vcl_fabs(y1-ye);
    double ax=vcl_fabs(x0-xe),ay=vcl_fabs(y0-ye);

    ratio=(b-a)/(c-a)*c/b;

    if (DEBUG) vcl_cout<<" camera ratio is: "<< ang <<" "<<ratio<<vcl_endl;
    /////  vcl_cout<< ang <<" "<<ratio<<vcl_endl;



    /// dont have camera yet.. 3d reconstruction is wrong..
    vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), P1, x2pt.pt(), P3);

    //// 9-21-2006 commented out ///// vcl_cout<<"contour point:"<<point_3d<<vcl_endl;
    point_3d_front_list.push_back(point_3d);
    ratios temp_ratios;
    temp_ratios.angle=ang;
    temp_ratios.angle_index=angle_index;
    temp_ratios.ratio=ratio;
    temp_ratios.pt3d=point_3d;
    temp_ratios.pt1=x0pt.pt();
    temp_ratios.pt2=x1pt.pt();
    temp_ratios.pt3=x2pt.pt();


    temp_ratios.fm1=x0;
    temp_ratios.fm2=x1;
    temp_ratios.fm3=x2;

    ratios_front_list.push_back(temp_ratios);

  }




  //for left side curve (rear of car)

  angle_index=-1;

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_ -MARGIN_; ang<-theta_neg_+MARGIN_ ; ang+=ang_ /*was .002*/) { ////angle goes clockwise..
    angle_index++;
    selected0=false; 
    selected1=false;
    selected2=false;
    x0=10000.0f;x1=10000.0f;x2=10000.f;
    //vcl_cout<<ang<<" "<<episeglist[fm1].size()<<" "<<episeglist[fm2].size()<<" "<<episeglist[fm3].size()<<vcl_endl;

    for (unsigned i0=0;i0<episeglist[fm1].size();i0++) {

      //vcl_cout<<episeglist[fm1][i0]->min_angle()<<" "<<episeglist[fm1][i0]->max_angle()<<vcl_endl;
      if (episeglist[fm1][i0]->min_angle() <=ang&&episeglist[fm1][i0]->max_angle() >=ang) {
        //if (episeglist[fm1][i0]->index(ang)) {

        if (episeglist[fm1][i0]->dist(ang)<x0) {
          x0=episeglist[fm1][i0]->dist(ang);
          selected0=true;

          x0pt.set_point(episeglist[fm1][i0],episeglist[fm1][i0]->index(ang));
        }

      }
    }

    for (unsigned i1=0;i1<episeglist[fm2].size();i1++) {
      //vcl_cout<<episeglist[fm2][i1]->min_angle()<<" "<<episeglist[fm2][i1]->max_angle()<<vcl_endl;
      if (episeglist[fm2][i1]->min_angle() <=ang&&episeglist[fm2][i1]->max_angle() >=ang) {
        //if (episeglist[fm2][i1]->index(ang)) {
        if (episeglist[fm2][i1]->dist(ang)<x1) {
          x1=episeglist[fm2][i1]->dist(ang);
          selected1=true;
          x1pt.set_point(episeglist[fm2][i1],episeglist[fm2][i1]->index(ang));
        }

      }
    }

    for (unsigned i2=0;i2<episeglist[fm3].size();i2++) {
      //vcl_cout<<episeglist[fm3][i2]->min_angle()<<" "<<episeglist[fm3][i2]->max_angle()<<vcl_endl;
      if (episeglist[fm3][i2]->min_angle() <=ang&&episeglist[fm3][i2]->max_angle() >=ang) {
        //if (episeglist[fm3][i2]->index(ang)) {
        if (episeglist[fm3][i2]->dist(ang)<x2) {
          x2=episeglist[fm3][i2]->dist(ang);
          selected2=true;
          x2pt.set_point(episeglist[fm3][i2],episeglist[fm3][i2]->index(ang));
        }
      }
    }

    // if 3 points are not determined continue
    if (!selected0 || !selected1 || !selected2) continue;

    // double c=vcl_sqrt(vcl_pow(x2-xe,2)+vcl_pow(y2-ye,2));
    // double b=vcl_sqrt(vcl_pow(x1-xe,2)+vcl_pow(y1-ye,2));
    // double a=vcl_sqrt(vcl_pow(x0-xe,2)+vcl_pow(y0-ye,2));
    double c=x2; double  b=x1;double a=x0;

    double cx=vcl_fabs(x2-xe),cy=vcl_fabs(y2-ye);
    double bx=vcl_fabs(x1-xe),by=vcl_fabs(y1-ye);
    double ax=vcl_fabs(x0-xe),ay=vcl_fabs(y0-ye);

    ratio=(b-a)/(c-a)*c/b;

    if (DEBUG) vcl_cout<<" camera ratio is: "<< ang <<" "<<ratio<<vcl_endl;
    //vcl_cout<< ang <<" "<<ratio<<vcl_endl;



    vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), P1, x2pt.pt(), P3);
    //vcl_cout<<point_3d<<vcl_endl;
    //vcl_cout<<"1 vs 3: "<<point_3d<<vcl_endl;
    point_3d_rear_list.push_back(point_3d);

    ratios temp_ratios;
    temp_ratios.angle=ang;

    temp_ratios.angle_index=angle_index;
    temp_ratios.ratio=ratio;
    temp_ratios.pt3d=point_3d;

    temp_ratios.pt1=x0pt.pt();
    temp_ratios.pt2=x1pt.pt();
    temp_ratios.pt3=x2pt.pt();

    temp_ratios.fm1=x0;
    temp_ratios.fm2=x1;
    temp_ratios.fm3=x2;

    ratios_rear_list.push_back(temp_ratios);
  }






  E2[0][0] = 1;  E2[0][1] = 0;   E2[0][2] = 0;   E2[0][3] =ratio*T[0];
  E2[1][0] = 0;  E2[1][1] = 1;   E2[1][2] = 0;   E2[1][3] =ratio*T[1];
  E2[2][0] = 0;  E2[2][1] = 0;   E2[2][2] = 1;   E2[2][3] =ratio*T[2];

  //vnl_double_3x4 P1 = M_in_*E1,P2 =M_in_*E2, P3=M_in_*E3;
  // vnl_double_3x4 P1 = M_in*E1,P2 =M_in*E2, P3=M_in*E3;



  //for ( vcl_map<int,vnl_double_3x4>::const_iterator C_itr = cameras_.begin();
  //  C_itr != cameras_.end();  ++C_itr )
  //vcl_cout << "Camera " << C_itr->first << "\n" << C_itr->second << vcl_endl;


  double r=histogram_method_for_finding_cross_ratio(ratios_front_list,ratios_rear_list);


  for (unsigned j=0;j<ratios_front_list.size();j++) {


    if (ratios_front_list[j].label==false) {

      angle_f_flag_[ratios_front_list[j].angle_index]=false;

    }


  }

  for (unsigned j=0;j<ratios_rear_list.size();j++) {



    if (ratios_rear_list[j].label==false) {

      angle_r_flag_[ratios_rear_list[j].angle_index]=false;

    }


  }



  //write_vrml("front.wrl",point_3d_front_list);
  //write_vrml("rear.wrl",point_3d_rear_list);
  //write_vrml("front.wrl",point_3d_front_list);

  return r;
}



