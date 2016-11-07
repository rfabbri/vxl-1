#include <testlib/testlib_test.h>
#include <modrec/modrec_vehicle_state.h>

void test_vehicle_state()
{
  vnl_vector<double> p(5,1.0);
  vgl_rotation_3d<double> r;
  vgl_vector_3d<double> t(1,2,0);
  double vt = 10;
  double va = 0.5;
  
  modrec_vehicle_state s1(p,r,t,vt,va);
  
  vnl_vector<double> sv = s1.state_vector();
  vcl_cout << "state = "<<sv<<vcl_endl;
  modrec_vehicle_state s2;
  s2.set_state(sv);
  
  TEST("state vector assignment", s1.params == s2.params && 
       s1.rotation.as_quaternion() == s2.rotation.as_quaternion() && 
       s1.translation == s2.translation && 
       s1.t_velocity == s2.t_velocity && 
       s1.a_velocity == s2.a_velocity, true);
}


void test_state_predict()
{
  double sd[] = {0,-10,0, 
                 0,0,0, 
                 10,0, 
                 0.02,0.1,-0.05,0,0};
  
  double t = 1.0/30;
  
  modrec_vehicle_state s1;
  s1.set_state(vnl_vector<double>(sd,13));
  s1.covar.set_size(13,13);
  s1.covar.set_identity();
  
  vnl_matrix<double> F(13,13,0.0);
  {
    F.set_identity();
    F(2,2) = F(3,3) = F(4,4) = 0.0;
    F(5,7) = t;
    double a1 = s1.rotation.as_rodrigues()[2];
    double a2 = a1+s1.a_velocity*t;
    double s = (s1.a_velocity==0.0) ? vcl_cos(a1) : (vcl_sin(a2)-vcl_sin(a1))/(s1.a_velocity*t);
    double c = (s1.a_velocity==0.0) ? -vcl_sin(a1) : (vcl_cos(a2)-vcl_cos(a1))/(s1.a_velocity*t);
    double ds = (s1.a_velocity==0.0) ? -0.5*vcl_sin(a1)*t : (vcl_cos(a2)-s)/s1.a_velocity;
    double dc = (s1.a_velocity==0.0) ? -0.5*vcl_cos(a1)*t : (-vcl_sin(a2)-c)/s1.a_velocity;
    F(0,5) = s1.t_velocity*t*c;
    F(0,6) = t*s;
    F(0,7) = s1.t_velocity*t*ds;
    F(1,5) = s1.t_velocity*t*s;
    F(1,6) = -t*c;
    F(1,7) = -s1.t_velocity*t*dc;
    
    vcl_cout << s <<' '<<c<<' '<<ds<<' '<<dc<<vcl_endl;
  }
  vnl_matrix<double> P = F * s1.covar * F.transpose();
  P(2,2) += 0.01;
  P(3,3) += 0.01;
  P(4,4) += 0.01;
  
  modrec_vehicle_state s2 = modrec_circ_motion_predict(s1,t);
  TEST_NEAR("predict covar",(s2.covar-P).array_inf_norm(), 0.0, 1e-8);
  

  for(unsigned int i=0; i<10; ++i){
    vcl_cout << s1.translation.x() <<" "<<s1.translation.y()<<" "<<s1.rotation.as_rodrigues()[2]<<vcl_endl;
    s1 = modrec_circ_motion_predict(s1,t);
  }

}


MAIN( test_vehicle_state )
{
  test_vehicle_state();
  test_state_predict();
 
  SUMMARY();
}

