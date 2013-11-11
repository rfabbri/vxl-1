// This is 
//:
// \file

#include "evaluate_sel_accuracy.h"

#include <vnl/vnl_random.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>

#include <pdf1d/pdf1d_calc_mean_var.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/sel/dbdet_sel_base.h>
#include <dbdet/sel/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/sel/dbdet_sel.h>

#include <dbdet/edge/dbdet_gen_edge_det.h>

//global random number generator
vnl_random vrand;

int main(int argc, char** argv)
{

  // The goal of this experiment is to determine the accuracy of measurements
  // of curvature and curvature derivatives by the combinatorial grouping process
  // on synthetic and real data sets.

  //Experiment 1: Circles of various curvatures with no perturbation
  //Experiment 2: Circles of various curvatures with various degrees of perturbation
  //Experiment 3: Euler spirals of various curvature derivatives with no perturbation
  //Experiment 4: Euler spirals of various curvature derivatives with various degrees of perturbation
  //Experiment 5: Ellipses of various eccentricities with no perturbation
  //Experiment 6: Ellipses of various eccentricities with various degrees of perturbation.

  // Factors affecting the accuracy of measurements are:
  //  a) the estimated errors of edge detection
  //  b) the size of the edgel groups used
  //  c) the curve model used

  //Synthetic data sets will be generated in two ways:
  //  a) By generating edgels directly and perturbing them
  //  b) by generating synthetic images and then performing edge detection on them

  //Experiment1a();
  //Experiment1b();
  //Experiment1c();
  //Experiment1d();

  //Experiment2a();
  //Experiment2b();
  //Experiment2c();
  //Experiment2d();
  //Experiment2e();
  //Experiment2f();

  Experiment3a();
  //Experiment3b();
  //Experiment3c();
  //Experiment3d();

  return 0; 
}

dbdet_edgemap_sptr generate_circle_edgemap(double k, double dx, double dt, 
                                           vcl_vector<dbdet_edgel*>& edgels, 
                                           vcl_vector<curve_params>& GT)
{
  double R = 1/k;
  int w = (int) vcl_ceil(4*R);
  int h = (int) vcl_ceil(4*R);

  //center of the circle
  double cx = 2*R;
  double cy = 2*R;

  //number of edgels
  int N = (int) vcl_floor(2*2*vnl_math::pi*R);

  //resize this vector and the ground truth vector
  edgels.resize(N);
  GT.resize(N);

  for (int i=0; i<N; i++)
  {
    //generate random perturbations    
    double rand_dx = 2*dx*vrand.drand32(-1,1);
    double rand_dy = 2*dx*vrand.drand32(-1,1);
    double rand_dt = 2*dt*vrand.drand32(-1,1);

    double theta = (2*vnl_math::pi*i)/N;
    vgl_point_2d<double> pt(cx+R*vcl_cos(theta)+rand_dx, cy+R*vcl_sin(theta)+rand_dy);

    //add this randomly perturbed edgel to the edgemap
    edgels[i] = new dbdet_edgel(pt, theta+vnl_math::pi_over_2+rand_dt);

    //save the ground truth
    GT[i] = curve_params(theta+vnl_math::pi_over_2, k, 0.0);
  }

  //construct an edgemap from these edgels
  dbdet_edgemap_sptr edgemap = new dbdet_edgemap(w, h, edgels);

  return edgemap;
}

dbdet_edgemap_sptr generate_ES_edgemap(double k, double gamma, double dx, double dt, 
                                       vcl_vector<dbdet_edgel*>& edgels, 
                                       vcl_vector<curve_params>& GT)
{
  int w = 100;
  int h = 100;

  //starting point of the ES
  double cx = 30.0;
  double cy = 50.0;

  //generate an Euler spiral corresponding to the given parameters
  dbgl_eulerspiral ES(vgl_point_2d<double>(cx,cy), 0.0, k, gamma, 50);
 
  //number of edgels
  int N = 50;

  //resize this vector and the ground truth vector
  edgels.resize(N);
  GT.resize(N);

  double l=0;
  for (unsigned i=0; i<50; i++, l+=(1+0.2*vrand.drand32(-1,1)))
  {
    //get the GT edgel from the ES
    vgl_point_2d<double> pt = ES.point_at_length(l);
    double theta = ES.tangent_angle_at_length(l);
    double k = ES.curvature_at_length(l);

    //generate random perturbations    
    double rand_dx = 2*dx*vrand.drand32(-1,1);
    double rand_dy = 2*dx*vrand.drand32(-1,1);
    double rand_dt = 2*dt*vrand.drand32(-1,1);

    //add this randomly perturbed edgel to the edgemap
    vgl_point_2d<double> ppt(pt.x()+rand_dx, pt.y()+rand_dy);
    edgels[i] = new dbdet_edgel(ppt, theta+rand_dt);

    //save the ground truth
    GT[i] = curve_params(theta, k, gamma);
  }

  //construct an edgemap from these edgels
  dbdet_edgemap_sptr edgemap = new dbdet_edgemap(w, h, edgels);

  return edgemap;
}

dbdet_edgemap_sptr generate_ellipse_edgemap(double ka, double kb, double dx, double dt, 
                                            vcl_vector<dbdet_edgel*>& edgels, 
                                            vcl_vector<curve_params>& GT)
{
  double Ra = 1/ka;
  double Rb = 1/kb;
  int w = (int) vcl_ceil(4*Rb);
  int h = (int) vcl_ceil(4*Ra);

  dbdet_edgemap_sptr edgemap = new dbdet_edgemap(w, h);

  return edgemap;
}

dbdet_edgemap_sptr generate_circle_edgemap_by_edge_detection(double k, double noise)
{
  double R = 1/k;
  int w = (int) vcl_ceil(4*R);
  int h = (int) vcl_ceil(4*R);

  dbdet_edgemap_sptr edgemap = new dbdet_edgemap(w, h);

  return edgemap;
}

dbdet_edgemap_sptr generate_ellipse_edgemap_by_edge_detection(double ka, double kb, double noise)
{
  double Ra = 1/ka;
  double Rb = 1/kb;
  int w = (int) vcl_ceil(4*Rb);
  int h = (int) vcl_ceil(4*Ra);

  dbdet_edgemap_sptr edgemap = new dbdet_edgemap(w, h);

  return edgemap;
}

void compute_accuracy(vcl_vector<dbdet_edgel*>& edgels, vcl_vector<curve_params>& GT,
                      curve_params& avg, curve_params& std, 
                      curve_params& avg_spread, curve_params& std_spread, 
                      curve_params& avg_error, curve_params& std_error)
{
  //determine # of curvelets
  int cvlet_cnt=0;
  for (unsigned i=0; i<edgels.size(); i++)
    cvlet_cnt += edgels[i]->curvelets.size();

  //estimates
  double estimates[3], min_estimates[3], max_estimates[3];
  vnl_vector<double> theta_est(cvlet_cnt), k_est(cvlet_cnt), gamma_est(cvlet_cnt);
  vnl_vector<double> theta_spread(cvlet_cnt), k_spread(cvlet_cnt), gamma_spread(cvlet_cnt);
  vnl_vector<double> theta_error(cvlet_cnt), k_error(cvlet_cnt), gamma_error(cvlet_cnt);

  cvlet_cnt=0;

  //for each edgel, for each curvelet, get accuracy measurements from the curve_model
  for (unsigned i=0; i<edgels.size(); i++)
  {
    curvelet_list_iter it = edgels[i]->curvelets.begin();
    for (; it != edgels[i]->curvelets.end(); it++){
      dbdet_curvelet* cvlet = (*it);

      //get the measurements from the curvelets
      cvlet->curve_model->report_accuracy(estimates, min_estimates, max_estimates);

      theta_est(cvlet_cnt) = estimates[0];
      k_est(cvlet_cnt)     = estimates[1];
      gamma_est(cvlet_cnt) = estimates[2];

      theta_spread(cvlet_cnt) = vcl_min(vcl_abs(estimates[0]-min_estimates[0]), vcl_abs(estimates[0]-max_estimates[0]));
      k_spread(cvlet_cnt)     = vcl_min(vcl_abs(estimates[1]-min_estimates[1]), vcl_abs(estimates[1]-max_estimates[1]));
      gamma_spread(cvlet_cnt) = vcl_min(vcl_abs(estimates[2]-min_estimates[2]), vcl_abs(estimates[2]-max_estimates[2]));

      theta_error(cvlet_cnt) = vcl_abs(estimates[0]-GT[i].theta);
      k_error(cvlet_cnt)     = vcl_abs(estimates[1]-GT[i].k);
      gamma_error(cvlet_cnt) = vcl_abs(estimates[2]-GT[i].gamma);

      cvlet_cnt++;
    }
  }

  //calculate the meaan and variance of the measurements
  pdf1d_calc_mean_var(avg.theta, std.theta, theta_est);
  pdf1d_calc_mean_var(avg.k, std.k, k_est);
  pdf1d_calc_mean_var(avg.gamma, std.gamma, gamma_est);

  //calculate the meaan and variance of the spread
  pdf1d_calc_mean_var(avg_spread.theta, std_spread.theta, theta_est);
  pdf1d_calc_mean_var(avg_spread.k, std_spread.k, k_est);
  pdf1d_calc_mean_var(avg_spread.k, std_spread.gamma, gamma_est);

  //calculate mean and variance of the errors
  pdf1d_calc_mean_var(avg_error.theta, std_error.theta, theta_error);
  pdf1d_calc_mean_var(avg_error.k, std_error.k, k_error);
  pdf1d_calc_mean_var(avg_error.gamma, std_error.gamma, gamma_error);

}

void Experiment1a()
{
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << " Accuracy Experiment 1a: " << vcl_endl;
  vcl_cout << " k measurement accuracy without perturbations" << vcl_endl;
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << "| dx_syn | dt_syn | dx_sel | dt_sel | nrad | maxN | GT k | avg. k | std. k | avg. err. | std. err. | avg. spread | std. spread |" << vcl_endl; 
  vcl_cout.precision(3);

  //parameters
  double dx_syn = 0.0;
  double dt_syn = 0.0;

  double dx_sel = 0.1;
  double dt_sel = 0.1;

  double nrad = 2.0;
  unsigned maxN = 7;

  for (double k=0.02; k<0.4; k+=0.02)
  {
    //the ground truth and measurements for later comparison   
    vcl_vector<dbdet_edgel*> edgels(0); 
    vcl_vector<curve_params> GT(0);

    dbdet_edgemap_sptr edgemap = generate_circle_edgemap(k, dx_syn, dt_syn, edgels, GT);

    dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
    edge_linker->build_curvelets_greedy(maxN);

    //get statistics on measurements
    curve_params avg, std, avg_spread, std_spread, avg_error, std_error;
    compute_accuracy(edgels, GT, avg, std, avg_spread, std_spread, avg_error, std_error);

    //report
    vcl_cout << "| " << dx_syn << " | " << dt_syn << " | " << dx_sel << " | " << dt_sel << " | " << nrad << " | " << maxN << " | ";
    vcl_cout << k << " | " << avg.k << " | " << std.k << " | ";
    vcl_cout << avg_error.k << " | " << std_error.k << " | ";
    vcl_cout << avg_spread.k << " | " << std_spread.k << " |" << vcl_endl; 
  }

  vcl_cout << "||------------------------------------------" << vcl_endl;
}

void Experiment1b()
{
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << " Accuracy Experiment 1b: " << vcl_endl;
  vcl_cout << " k measurement accuracy as a function of dx_sel" << vcl_endl;
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << "| dx_syn | dt_syn | dx_sel | dt_sel | nrad | maxN | GT k | avg. k | std. k | avg. err. | std. err. | avg. spread | std. spread |" << vcl_endl; 
  vcl_cout.precision(3);

  //parameters
  double dx_syn = 0.0;
  double dt_syn = 0.0;

  double dt_sel = 0.1;

  double nrad = 2.0;
  unsigned maxN = 7;

  double k = 0.05;

  for (double dx_sel=0.02; dx_sel<0.4; dx_sel+=0.02)
  {
    //the ground truth and measurements for later comparison   
    vcl_vector<dbdet_edgel*> edgels(0); 
    vcl_vector<curve_params> GT(0);

    dbdet_edgemap_sptr edgemap = generate_circle_edgemap(k, dx_syn, dt_syn, edgels, GT);

    dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
    edge_linker->build_curvelets_greedy(maxN);

    //get statistics on measurements
    curve_params avg, std, avg_spread, std_spread, avg_error, std_error;
    compute_accuracy(edgels, GT, avg, std, avg_spread, std_spread, avg_error, std_error);

    //report
    vcl_cout << "| " << dx_syn << " | " << dt_syn << " | " << dx_sel << " | " << dt_sel << " | " << nrad << " | " << maxN << " | ";
    vcl_cout << k << " | " << avg.k << " | " << std.k << " | ";
    vcl_cout << avg_error.k << " | " << std_error.k << " | ";
    vcl_cout << avg_spread.k << " | " << std_spread.k << " |" << vcl_endl; 
  }

  vcl_cout << "||------------------------------------------" << vcl_endl;
}

void Experiment1c()
{
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << " Accuracy Experiment 1c: " << vcl_endl;
  vcl_cout << " k measurement accuracy as a function of dt_sel" << vcl_endl;
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << "| dx_syn | dt_syn | dx_sel | dt_sel | nrad | maxN | GT k | avg. k | std. k | avg. err. | std. err. | avg. spread | std. spread |" << vcl_endl; 
  vcl_cout.precision(3);

  //parameters
  double dx_syn = 0.0;
  double dt_syn = 0.0;

  double dx_sel = 0.1;

  double nrad = 2.0;
  unsigned maxN = 7;

  double k = 0.05;

  for (double dt_sel=0.02; dt_sel<0.4; dt_sel+=0.02)
  {
    //the ground truth and measurements for later comparison   
    vcl_vector<dbdet_edgel*> edgels(0); 
    vcl_vector<curve_params> GT(0);

    dbdet_edgemap_sptr edgemap = generate_circle_edgemap(k, dx_syn, dt_syn, edgels, GT);

    dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
    edge_linker->build_curvelets_greedy(maxN);

    //get statistics on measurements
    curve_params avg, std, avg_spread, std_spread, avg_error, std_error;
    compute_accuracy(edgels, GT, avg, std, avg_spread, std_spread, avg_error, std_error);

    //report
    vcl_cout << "| " << dx_syn << " | " << dt_syn << " | " << dx_sel << " | " << dt_sel << " | " << nrad << " | " << maxN << " | ";
    vcl_cout << k << " | " << avg.k << " | " << std.k << " | ";
    vcl_cout << avg_error.k << " | " << std_error.k << " | ";
    vcl_cout << avg_spread.k << " | " << std_spread.k << " |" << vcl_endl; 
  }

  vcl_cout << "||------------------------------------------" << vcl_endl;
}

void Experiment1d()
{
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << " Accuracy Experiment 1d: " << vcl_endl;
  vcl_cout << " k measurement accuracy as a function of maxN" << vcl_endl;
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << "| dx_syn | dt_syn | dx_sel | dt_sel | nrad | maxN | GT k | avg. k | std. k | avg. err. | std. err. | avg. spread | std. spread |" << vcl_endl; 
  vcl_cout.precision(3);

  //parameters
  double dx_syn = 0.0;
  double dt_syn = 0.0;

  double dx_sel = 0.1;
  double dt_sel = 0.1;

  double nrad = 2.0;

  double k = 0.05;

  for (unsigned maxN=3; maxN<15; maxN++)
  {
    //the ground truth and measurements for later comparison   
    vcl_vector<dbdet_edgel*> edgels(0); 
    vcl_vector<curve_params> GT(0);

    dbdet_edgemap_sptr edgemap = generate_circle_edgemap(k, dx_syn, dt_syn, edgels, GT);

    dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
    edge_linker->build_curvelets_greedy(maxN);

    //get statistics on measurements
    curve_params avg, std, avg_spread, std_spread, avg_error, std_error;
    compute_accuracy(edgels, GT, avg, std, avg_spread, std_spread, avg_error, std_error);

    //report
    vcl_cout << "| " << dx_syn << " | " << dt_syn << " | " << dx_sel << " | " << dt_sel << " | " << nrad << " | " << maxN << " | ";
    vcl_cout << k << " | " << avg.k << " | " << std.k << " | ";
    vcl_cout << avg_error.k << " | " << std_error.k << " | ";
    vcl_cout << avg_spread.k << " | " << std_spread.k << " |" << vcl_endl; 
  }

  vcl_cout << "||------------------------------------------" << vcl_endl;
}

void Experiment2a()
{
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << " Accuracy Experiment 2a: " << vcl_endl;
  vcl_cout << " k measurement accuracy with perturbations" << vcl_endl;
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << "| dx_syn | dt_syn | dx_sel | dt_sel | nrad | maxN | GT k | avg. k | std. k | avg. err. | std. err. | avg. spread | std. spread |" << vcl_endl; 
  vcl_cout.precision(3);

  //parameters
  double dx_syn = 0.1;
  double dt_syn = 0.1;

  double dx_sel = 0.1;
  double dt_sel = 0.1;

  double nrad = 2.0;
  unsigned maxN = 7;

  for (double k=0.02; k<0.4; k+=0.02)
  {
    //the ground truth and measurements for later comparison   
    vcl_vector<dbdet_edgel*> edgels(0); 
    vcl_vector<curve_params> GT(0);

    dbdet_edgemap_sptr edgemap = generate_circle_edgemap(k, dx_syn, dt_syn, edgels, GT);

    dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
    edge_linker->build_curvelets_greedy(maxN);

    //get statistics on measurements
    curve_params avg, std, avg_spread, std_spread, avg_error, std_error;
    compute_accuracy(edgels, GT, avg, std, avg_spread, std_spread, avg_error, std_error);

    //report
    vcl_cout << "| " << dx_syn << " | " << dt_syn << " | " << dx_sel << " | " << dt_sel << " | " << nrad << " | " << maxN << " | ";
    vcl_cout << k << " | " << avg.k << " | " << std.k << " | ";
    vcl_cout << avg_error.k << " | " << std_error.k << " | ";
    vcl_cout << avg_spread.k << " | " << std_spread.k << " |" << vcl_endl; 
  }

  vcl_cout << "||------------------------------------------" << vcl_endl;
}

void Experiment2b()
{
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << " Accuracy Experiment 2b: " << vcl_endl;
  vcl_cout << " k measurement accuracy as a function of dx_sel" << vcl_endl;
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << "| dx_syn | dt_syn | dx_sel | dt_sel | nrad | maxN | GT k | avg. k | std. k | avg. err. | std. err. | avg. spread | std. spread |" << vcl_endl; 
  vcl_cout.precision(3);

  //parameters
  double dx_syn = 0.1;
  double dt_syn = 0.1;

  double dt_sel = 0.1;

  double nrad = 2.0;
  unsigned maxN = 7;

  double k = 0.05;

  for (double dx_sel=0.02; dx_sel<0.4; dx_sel+=0.02)
  {
    //the ground truth and measurements for later comparison   
    vcl_vector<dbdet_edgel*> edgels(0); 
    vcl_vector<curve_params> GT(0);

    dbdet_edgemap_sptr edgemap = generate_circle_edgemap(k, dx_syn, dt_syn, edgels, GT);

    dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
    edge_linker->build_curvelets_greedy(maxN);

    //get statistics on measurements
    curve_params avg, std, avg_spread, std_spread, avg_error, std_error;
    compute_accuracy(edgels, GT, avg, std, avg_spread, std_spread, avg_error, std_error);

    //report
    vcl_cout << "| " << dx_syn << " | " << dt_syn << " | " << dx_sel << " | " << dt_sel << " | " << nrad << " | " << maxN << " | ";
    vcl_cout << k << " | " << avg.k << " | " << std.k << " | ";
    vcl_cout << avg_error.k << " | " << std_error.k << " | ";
    vcl_cout << avg_spread.k << " | " << std_spread.k << " |" << vcl_endl; 
  }

  vcl_cout << "||------------------------------------------" << vcl_endl;
}

void Experiment2c()
{
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << " Accuracy Experiment 2c: " << vcl_endl;
  vcl_cout << " k measurement accuracy as a function of dt_sel" << vcl_endl;
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << "| dx_syn | dt_syn | dx_sel | dt_sel | nrad | maxN | GT k | avg. k | std. k | avg. err. | std. err. | avg. spread | std. spread |" << vcl_endl; 
  vcl_cout.precision(3);

  //parameters
  double dx_syn = 0.1;
  double dt_syn = 0.1;

  double dx_sel = 0.1;

  double nrad = 2.0;
  unsigned maxN = 7;

  double k = 0.05;

  for (double dt_sel=0.02; dt_sel<0.4; dt_sel+=0.02)
  {
    //the ground truth and measurements for later comparison   
    vcl_vector<dbdet_edgel*> edgels(0); 
    vcl_vector<curve_params> GT(0);

    dbdet_edgemap_sptr edgemap = generate_circle_edgemap(k, dx_syn, dt_syn, edgels, GT);

    dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
    edge_linker->build_curvelets_greedy(maxN);

    //get statistics on measurements
    curve_params avg, std, avg_spread, std_spread, avg_error, std_error;
    compute_accuracy(edgels, GT, avg, std, avg_spread, std_spread, avg_error, std_error);

    //report
    vcl_cout << "| " << dx_syn << " | " << dt_syn << " | " << dx_sel << " | " << dt_sel << " | " << nrad << " | " << maxN << " | ";
    vcl_cout << k << " | " << avg.k << " | " << std.k << " | ";
    vcl_cout << avg_error.k << " | " << std_error.k << " | ";
    vcl_cout << avg_spread.k << " | " << std_spread.k << " |" << vcl_endl; 
  }

  vcl_cout << "||------------------------------------------" << vcl_endl;
}

void Experiment2d()
{
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << " Accuracy Experiment 2d: " << vcl_endl;
  vcl_cout << " k measurement accuracy as a function of maxN" << vcl_endl;
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << "| dx_syn | dt_syn | dx_sel | dt_sel | nrad | maxN | GT k | avg. k | std. k | avg. err. | std. err. | avg. spread | std. spread |" << vcl_endl; 
  vcl_cout.precision(3);

  //parameters
  double dx_syn = 0.1;
  double dt_syn = 0.1;

  double dx_sel = 0.1;
  double dt_sel = 0.1;

  double nrad = 2.0;

  double k = 0.05;

  for (unsigned maxN=3; maxN<15; maxN++)
  {
    //the ground truth and measurements for later comparison   
    vcl_vector<dbdet_edgel*> edgels(0); 
    vcl_vector<curve_params> GT(0);

    dbdet_edgemap_sptr edgemap = generate_circle_edgemap(k, dx_syn, dt_syn, edgels, GT);

    dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
    edge_linker->build_curvelets_greedy(maxN);

    //get statistics on measurements
    curve_params avg, std, avg_spread, std_spread, avg_error, std_error;
    compute_accuracy(edgels, GT, avg, std, avg_spread, std_spread, avg_error, std_error);

    //report
    vcl_cout << "| " << dx_syn << " | " << dt_syn << " | " << dx_sel << " | " << dt_sel << " | " << nrad << " | " << maxN << " | ";
    vcl_cout << k << " | " << avg.k << " | " << std.k << " | ";
    vcl_cout << avg_error.k << " | " << std_error.k << " | ";
    vcl_cout << avg_spread.k << " | " << std_spread.k << " |" << vcl_endl; 
  }

  vcl_cout << "||------------------------------------------" << vcl_endl;
}

void Experiment2e()
{
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << " Accuracy Experiment 2e: " << vcl_endl;
  vcl_cout << " k measurement accuracy with perturbations (syn vs sel)" << vcl_endl;
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << "| dx_syn | dt_syn | dx_sel | dt_sel | nrad | maxN | GT k | avg. k | std. k | avg. err. | std. err. | avg. spread | std. spread |" << vcl_endl; 
  vcl_cout.precision(3);

  //parameters
  double dt_syn = 0.1;

  double dt_sel = 0.1;

  double nrad = 2.0;
  unsigned maxN = 7;

  double k=0.05;

  for (double dx_syn=0.02; dx_syn<0.4; dx_syn+=0.02){
    for (double dx_sel=0.02; dx_sel<0.4; dx_sel+=0.02)
    {
      //the ground truth and measurements for later comparison   
      vcl_vector<dbdet_edgel*> edgels(0); 
      vcl_vector<curve_params> GT(0);

      dbdet_edgemap_sptr edgemap = generate_circle_edgemap(k, dx_syn, dt_syn, edgels, GT);

      dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
      edge_linker->build_curvelets_greedy(maxN);

      //get statistics on measurements
      curve_params avg, std, avg_spread, std_spread, avg_error, std_error;
      compute_accuracy(edgels, GT, avg, std, avg_spread, std_spread, avg_error, std_error);

      //report
      vcl_cout << "| " << dx_syn << " | " << dt_syn << " | " << dx_sel << " | " << dt_sel << " | " << nrad << " | " << maxN << " | ";
      vcl_cout << k << " | " << avg.k << " | " << std.k << " | ";
      vcl_cout << avg_error.k << " | " << std_error.k << " | ";
      vcl_cout << avg_spread.k << " | " << std_spread.k << " |" << vcl_endl; 
    }
  }

  vcl_cout << "||------------------------------------------" << vcl_endl;
}

void Experiment2f()
{
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << " Accuracy Experiment 2f: " << vcl_endl;
  vcl_cout << " k measurement accuracy with perturbations (syn vs sel)" << vcl_endl;
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << "| dx_syn | dt_syn | dx_sel | dt_sel | nrad | maxN | GT k | avg. k | std. k | avg. err. | std. err. | avg. spread | std. spread |" << vcl_endl; 
  vcl_cout.precision(3);

  //parameters
  double dx_syn = 0.1;

  double dx_sel = 0.1;

  double nrad = 2.0;
  unsigned maxN = 7;

  double k=0.05;

  for (double dt_syn=0.02; dt_syn<0.4; dt_syn+=0.02){
    for (double dt_sel=0.02; dt_sel<0.4; dt_sel+=0.02)
    {
      //the ground truth and measurements for later comparison   
      vcl_vector<dbdet_edgel*> edgels(0); 
      vcl_vector<curve_params> GT(0);

      dbdet_edgemap_sptr edgemap = generate_circle_edgemap(k, dx_syn, dt_syn, edgels, GT);

      dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
      edge_linker->build_curvelets_greedy(maxN);

      //get statistics on measurements
      curve_params avg, std, avg_spread, std_spread, avg_error, std_error;
      compute_accuracy(edgels, GT, avg, std, avg_spread, std_spread, avg_error, std_error);

      //report
      vcl_cout << "| " << dx_syn << " | " << dt_syn << " | " << dx_sel << " | " << dt_sel << " | " << nrad << " | " << maxN << " | ";
      vcl_cout << k << " | " << avg.k << " | " << std.k << " | ";
      vcl_cout << avg_error.k << " | " << std_error.k << " | ";
      vcl_cout << avg_spread.k << " | " << std_spread.k << " |" << vcl_endl; 
    }
  }

  vcl_cout << "||------------------------------------------" << vcl_endl;
}


void Experiment3a()
{
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << " Accuracy Experiment 3a: " << vcl_endl;
  vcl_cout << " k measurement accuracy without perturbations" << vcl_endl;
  vcl_cout << "------------------------------------------" << vcl_endl;
  vcl_cout << "| dx_syn | dt_syn | dx_sel | dt_sel | nrad | maxN | GT gamma | avg. gamma | std. gamma | avg. gamma err. | std. gamma err. | avg. gamma spread | std. gamma spread | avg. k err. | std. k err. | avg. k spread | std. k spread | avg. theta err. | std. theta err. | " << vcl_endl; 
  vcl_cout.precision(3);

  //parameters
  double dx_syn = 0.0;
  double dt_syn = 0.0;

  double dx_sel = 0.1;
  double dt_sel = 0.1;

  double nrad = 2.0;
  unsigned maxN = 7;

  for (double gamma=0.0; gamma<0.2; gamma+=0.01){
    for (double k=-0.3; k<0.4; k+=0.1)
    {
      //the ground truth and measurements for later comparison   
      vcl_vector<dbdet_edgel*> edgels(0); 
      vcl_vector<curve_params> GT(0);

      dbdet_edgemap_sptr edgemap = generate_ES_edgemap(k, gamma, dx_syn, dt_syn, edgels, GT);

      dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edgemap, nrad, dt_sel, dx_sel); 
      edge_linker->build_curvelets_greedy(maxN);

      //get statistics on measurements
      curve_params avg, std, avg_spread, std_spread, avg_error, std_error;
      compute_accuracy(edgels, GT, avg, std, avg_spread, std_spread, avg_error, std_error);

      //report
      vcl_cout << "| " << dx_syn << " | " << dt_syn << " | " << dx_sel << " | " << dt_sel << " | " << nrad << " | " << maxN << " | ";
      vcl_cout << gamma << " | " << avg.gamma << " | " << std.gamma << " | ";
      vcl_cout << avg_error.gamma << " | " << std_error.gamma << " | ";
      vcl_cout << avg_spread.gamma << " | " << std_spread.gamma << " |";
      vcl_cout << avg_error.k << " | " << std_error.k << " | ";
      vcl_cout << avg_spread.k << " | " << std_spread.k << " |";
      vcl_cout << avg_error.theta << " | " << std_error.theta << " | " << vcl_endl; 
    }
  }

  vcl_cout << "||------------------------------------------" << vcl_endl;
}


void Experiment4()
{
}

void Experiment5()
{
}

void Experiment6()
{
}
