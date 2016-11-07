// This is evaluate_sel_accuracy.h
#ifndef evaluate_sel_accuracy_h
#define evaluate_sel_accuracy_h
//:
//\file
//\brief 
//\author Amir Tamrakar
//\date 11/15/06
//
//\verbatim
//  Modifications
//\endverbatim

#include <vcl_vector.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

class curve_params
{
public:
  double theta;
  double k;
  double gamma;

  curve_params(double theta_=0.0, double k_=0.0, double gamma_=0.0): 
    theta(theta_), k(k_), gamma(gamma_){}

  ~curve_params(){}
};

dbdet_edgemap_sptr generate_circle_edgemap(double k, double dx, double dt, 
                                           vcl_vector<dbdet_edgel*>& edgels, 
                                           vcl_vector<curve_params>& GT);
dbdet_edgemap_sptr generate_ES_edgemap(double k, double gamma, double dx, double dt, 
                                       vcl_vector<dbdet_edgel*>& edgels, 
                                       vcl_vector<curve_params>& GT);
dbdet_edgemap_sptr generate_ellipse_edgemap(double ka, double kb, double dx, double dt, 
                                            vcl_vector<dbdet_edgel*>& edgels, 
                                            vcl_vector<curve_params>& GT);
dbdet_edgemap_sptr generate_circle_edgemap_by_edge_detection(double k, double noise);
dbdet_edgemap_sptr generate_ellipse_edgemap_by_edge_detection(double ka, double kb, double noise);

void compute_accuracy(vcl_vector<dbdet_edgel*>& edgels, vcl_vector<curve_params>& GT,
                      curve_params& avg, curve_params& std, 
                      curve_params& avg_spread, curve_params& std_spread, 
                      curve_params& avg_error, curve_params& std_error);

void Experiment1a();
void Experiment1b();
void Experiment1c();
void Experiment1d();

void Experiment2a();
void Experiment2b();
void Experiment2c();
void Experiment2d();
void Experiment2e();
void Experiment2f();

void Experiment3a();
//void Experiment3b();
//void Experiment3c();
//void Experiment3d();

void Experiment4();
void Experiment5();
void Experiment6();

#endif // evaluate_sel_accuracy_h
