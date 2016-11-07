// This is brcv/rec/dbasn/dbasn_params.h
//:
// \file
// \brief A block of parameters for the gradassign class 
// \author Ozge C. Ozcanli  <ozge@lems.brown.edu>
// \date 02/04/04
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#ifndef dbasn_params_h_
#define dbasn_params_h_

#include <vcl_iostream.h>

#define GA_COMPARE_NODE     1
#define GA_COMPARE_LINK     1
#define GA_COMPARE_CORNER   1

class dbasn_params
{
public:
  //: variables of the algorithm
  float T0_;  //initial temperature (should be high) 
  float Tf_;  //termination temperature 
  float Tr_;  //step in temperature
  int   I0_;  //# iterations in loop B
  int   I1_;  //# iterations in loop C
  int   Is_;  //# iterations in softassign

  //: Weight: should be small to avoid exp. explosion.
  float wN_;  //node cost weight for Q_ai (important!)
  float wL_;  //link cost weight for Q_ai (important!)
  float wC_;  //corner cost weight for Q_ai (important!)

  float eB_;  //criterion of convergence of M (see paper)
  float eC_;  //criterion of convergence of M_hat (see paper)
  float eS_;  //criterion of convergence of softassign

public:
  //####### Constructor/Destructor #######
  //: Default valuable for the grad. assignment variables.
  dbasn_params () {    
    T0_ = 100.0f;     //def: 100.0f, best: 1000.0f
    Tf_ =  0.01f;     //def: 0.01f, 0.001f will explode
                      //since exp (1/Tf_) = exp(100) = 2.6E43, exp(1000) = inf.
    Tr_ =   0.95f;    //def: 0.9f, best: 0.95f
    I0_ =      4;     //def: 4.0f, 5.0f
    I1_ =     30;     //def: 30.0f, 50.0f
    Is_ =    100;     //def: 100
    wN_ =   0.3f;    //def: 0.3, 0.16, 0.16, 0.2, 1, 2, 3, 10
    wL_ =   0.5f;    //def: 0.5, 0.50, 0.42, 0.5, 1.0, 3, 3, 2, 0
    wC_ =   0.4f;    //def: 0.4, 0.34, 0.42, 1.0, 0.5, 0.1f, 
    eB_ = 0.005f;     //paper 0.5, 0.005f
    eC_ = 0.0005f;    //paper 0.05, 0.05f
    eS_ = 0.00005f;   //def: 0.00005
  }
};

vcl_ostream& operator<<(vcl_ostream& os, const dbasn_params& params);

#endif
