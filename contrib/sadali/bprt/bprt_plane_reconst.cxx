#include <vcl_cstdio.h> 
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include "bprt_plane_reconst.h"
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>
#include <vnl/vnl_quaternion.h> 


#include <bcal/bcal_calibrate_object.h>
#include <bcal/bcal_calibrate_plane.h>
#include <bcal/bcal_camera.h>
#include <bcal/bcal_camera_graph.h>
#include <bcal/bcal_camera_node.h>
#include <bcal/bcal_euclidean_transformation.h>
#include <bcal/bcal_zhang_camera_node.h>
#include <bcal/bcal_zhang_linear_calibrate.h>


bprt_plane_reconst::bprt_plane_reconst()
{
}
bprt_plane_reconst::bprt_plane_reconst(vcl_vector <vtol_edge_2d_sptr >  curvs1,vcl_vector <vtol_edge_2d_sptr >  curvs2,int samp,double std)
{
        int samp_=samp;
        std_=std;
        
        vdgl_digital_curve_sptr curv1,curv2;
        for (int i=0; i<curvs1.size();i++)
        {
                curv1=conv_vtol_to_dc(curvs1[i]);
                for (int j=0;j<samp_;j++)
                {
                        vgl_homg_point_2d<double> point1(curv1->get_x(j/samp_),curv1->get_y(j/samp_));
        //              ptlist1_.push_back(point1);
                //      vcl_cout<<point1.x()<<" , "<<point1.y()<<" , "<<point1.w()<<"   ";
                }
        }

        for (int i=0; i<curvs2.size();i++)
        {
                curv2=conv_vtol_to_dc(curvs2[i]);
                for (int j=0;j<samp_;j++)
                {
                        vgl_homg_point_2d<double> point2(curv2->get_x(j/samp_),curv2->get_y(j/samp_));
        //              ptlist2_.push_back(point2);
                        //vcl_cout<<point2.x()<<"  "<<point2.y()<<"\n";
                }
        }

        

}
bprt_plane_reconst::~bprt_plane_reconst()
{
}


vdgl_digital_curve_sptr bprt_plane_reconst::conv_vtol_to_dc(vtol_edge_2d_sptr vtoledge)
{
        vdgl_digital_curve_sptr extcurve;
        vsol_curve_2d_sptr extcurv=vtoledge->curve();
    //    return extcurv->cast_to_digital_curve();
                return new vdgl_digital_curve;
}

bool bprt_plane_reconst::findplaneparams(HMatrix2D H,vnl_double_3x3 K1,vnl_double_3x3 K2,
                                        vnl_double_3x3 R,vnl_double_3 t)
{
        vnl_matrix_fixed<double,3,3> IR,tempH;
        tempH=H_.get_matrix();
        double a,b,c;
        double d;
        IR=R-(vnl_inverse(K1)*tempH*K2);
//      vcl_cout<<tempH[0][0]<<","<<tempH[1][0]<<","<<tempH[0][1]<<","<<tempH[2][1]<<","<<tempH[1][1]<<","<<
//              IR[1][1]<<","<<tempH[1][2]<<","<<tempH[2][2]<<","<<IR[2][0]<<","<<IR[0][0]<<","<<IR[1][2]<<"\n";
        
        a=0;
        b=0;
        c=0;

        for(int k=0;k<3;k++)
        {
                if (t[k])
                {
                        a+=(IR[k][0])/3.0/(t[k]);
                        b+=(IR[k][1])/3.0/(t[k]);
                        c+=(IR[k][2])/3.0/(t[k]);
                }
                else
                {a+=a/(k+1);
                b+=b/(k+1);
                c+=c/(k+1);
                }
        }
        vcl_cout<<a<<","<<b<<","<<c<<"\n";
        d=sqrt(1.0/((a*a)+(b*b)+(c*c)));
        this->param_.set_params(a,b,c);

        return 1;
}
void bprt_plane_reconst::set_cal_params(vnl_double_3x3 K1,vnl_double_3x3 K2,vnl_double_3x3 R,vnl_double_3 t)
{
        K1_=K1;
        K2_=K2;
        R_=R;
        t_=t;
}

void bprt_plane_reconst::comp_param()
{
        
        HMatrix2DComputeMLESAC Calc(std_);
        H_=Calc.compute(ptlist1_,ptlist2_);
        findplaneparams(H_,K1_,K2_,R_,t_);

}

vcl_vector< vsol_spatial_object_2d_sptr>  bprt_plane_reconst::get_vsol_point_2ds(int view)
{
        vcl_vector< vsol_spatial_object_2d_sptr> points;
        if (view==0)
        {
        
                for (int i=0; i<ptlist1_.size() ; i++)
                {
                        vgl_point_2d<double> newpt(ptlist1_[i].x(),ptlist1_[i].y());
                        

                        points.push_back(new vsol_point_2d(newpt ));
                }
        }
        if (view==1)
        {
                
                for (int i=1; i<ptlist2_.size() ; i++)
                {

                        vgl_point_2d<double> newpt2(ptlist2_[i].x(),ptlist2_[i].y());
                        points.push_back(new vsol_point_2d(newpt2));
                }
        }
                return points;
                
        
}
int bprt_plane_reconst::calibrate()
{
  
        bcal_camera_graph<bcal_calibrate_plane, bcal_zhang_camera_node, bcal_euclidean_transformation> cg;

         //
  // following block is used for reading the model data
  //
        vcl_string fname="corr_left.txt";
        vcl_string fname2="corr_right.txt";
  vcl_ifstream  in(fname.c_str());

  if (!in){
    vcl_cerr<<"cannot open the file: corr_left.txt" << vcl_endl;
  }

  int num_points = 0;
  if (in.eof()){
    vcl_cerr<<"wrong file! at least number of points in the calibration board are needed\n"<< vcl_endl;
  }

  in >> num_points;
  vcl_cout << "num_points = " << num_points << '\n'<< vcl_endl;
  vcl_vector<vgl_homg_point_2d<double> > pts(num_points);

  for (int i=0; i<num_points; i++) {
    double u, v; in >> u >> v;
    pts[i] = vgl_homg_point_2d<double>(u, v);
  }
  // add a camera with 5 views into a graph
  int camID = cg.add_vertex();
  int source_id = cg.get_source_id();


  // initialize the template plane
  cg.get_source()->read_data(pts);
 
  

  // add a camera with 7 views into a graph
  int nviews = 0;
  in >> nviews;
  vcl_cout << "nviews = " << nviews << '\n'<< vcl_endl;

  vcl_vector<double> t_beats(nviews);
  for (int i=0; i<nviews; i++)
    t_beats[i] = i;

  // set beats on camera node
  cg.get_vertex(camID)->set_beat(t_beats);

  // set beats on translation
  bcal_euclidean_transformation *trans = cg.get_edge(source_id, camID);
  //assert(trans);
  trans->set_beat(t_beats);

  cg.print(vcl_cout);


  //
  // reading the feature point for each view
  //
  int num=6;

  for (int i=0; i<nviews; i++){
    vcl_vector<vgl_homg_point_2d<double> > features(num_points);
    for (int j = 0; j<num_points; j++) {
      double u, v; in>>u>>v;
      features[j] = vgl_homg_point_2d<double>(u, v);
          if (i==num)
          ptlist1_.push_back(features[j]);
    }
    cg.get_vertex(camID)->read_data(features , i);
  }


  //Adding second camera


  vcl_ifstream  in2(fname2.c_str());

  if (!in2){
    vcl_cerr<<"cannot open the file: corr_right.txt"<< vcl_endl;
  }

  num_points = 0;
  if (in2.eof()){
    vcl_cerr<<"wrong file! at least number of points in the calibration board are needed\n"<< vcl_endl;
  }

  in2 >> num_points;
  vcl_cout << "num_points = " << num_points << '\n'<<vcl_endl;
  vcl_vector<vgl_homg_point_2d<double> > pts2(num_points);

  for (int i=0; i<num_points; i++) {
    double u, v; in2 >> u >> v;
 //   pts2[i] = vgl_homg_point_2d<double>(u, v);

  }
  
  int camID2 = cg.add_vertex();

 //int nviews = 0;
  in2 >> nviews;
  vcl_cout << "nviews = " << nviews << '\n'<<vcl_endl;

  //vcl_vector<double> t_beats(nviews);
 // for (int i=0; i<nviews; i++)
  //  t_beats[i] = i;

  // set beats on camera node
  cg.get_vertex(camID2)->set_beat(t_beats);

  // set beats on translation
  bcal_euclidean_transformation *trans2 = cg.get_edge(source_id, camID2);
  //assert(trans2);
  trans2->set_beat(t_beats);

 

  for (int i=0; i<nviews; i++){
    vcl_vector<vgl_homg_point_2d<double> > features2(num_points);
    for (int j = 0; j<num_points; j++) {
      double u, v; in2>>u>>v;
      features2[j] = vgl_homg_point_2d<double>(u, v);
          if (i==num)
                ptlist2_.push_back(features2[j]);
    }
    cg.get_vertex(camID2)->read_data(features2 , i);
  }


  // do the calibration
  vcl_cout<<"\n\nlinear calibration..............\n\n"<<vcl_endl;
  bcal_zhang_linear_calibrate lc;
  lc.setCameraGraph(&cg);
  lc.calibrate();

  cg.print(vcl_cout);
  K1_=cg.get_vertex(camID)->get_intrinsic();
  K2_=cg.get_vertex(camID2)->get_intrinsic();
  
  vcl_vector <vnl_double_3x3> Rot1,Rot2;
  Rot1.resize(nviews);
  Rot2.resize(nviews);
  vnl_double_3x3 Rot;
  vnl_double_3x1 tr1,tr2,tr;
  for (int m=0;m<3;m++)
  {
            for (int n=0;n<3;n++)
                        Rot[m][n]=0;
                tr[m][0]=0;

  }

//  for (int i=0;i<nviews;i++)
  int i=num;
  {
          vnl_double_4x4 M1=cg.get_edge(source_id, camID)->get_trans_matrix(i);
          vnl_double_4x4 M2=cg.get_edge(source_id, camID2)->get_trans_matrix(i);
          Rot1[i]=M1.extract(3,3,0,0);
          Rot2[i]=M2.extract(3,3,0,0);
          vcl_cout<<i<<" , " <<vnl_det(Rot1[i])<<"\n";
          vcl_cout<<i<<" , " <<vnl_det(Rot2[i])<<"\n";
          tr1=M1.extract(3,1,0,3);
          tr2=M2.extract(3,1,0,3);
           Rot=(vnl_inverse(Rot1[i])*Rot2[i]);
           vnl_quaternion<double> quat(Rot);
          // quat.print(vcl_cout);
          tr=tr2-tr1;
  }
  
  R_=Rot;
  /*for (int i=0;i<nviews;i++)
           for (int m=0;m<3;m++)
           {
                   for (int n=0;n<3;n++)
                   {vnl_double_3x3 Res=vnl_inverse(Rot1[i])*Rot2[i];
                           
                           vcl_cout<<(Rot[m][n]-Res[m][n])<<",";
                   }
                   vcl_cout<<"\n"<<vnl_det(Rot);
                   

           }
           */
  for (int i=0;i<3;i++)
  t_[i]=tr[i][0];
  
  in.close();
  in2.close();
  
  return 1;
 
   
}



