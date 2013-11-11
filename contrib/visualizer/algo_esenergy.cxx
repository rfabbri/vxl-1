#include <math.h>

#include "algo_esenergy.h"

int BaseEnergy::getOrder(const EulerSpiralParams &params1, const EulerSpiralParams &params2)
  {
    double E1=0, E2=0;

    E1 = computeEnergy(params1);
    E2 = computeEnergy(params2);

    if (E1==E2)
        return 0;

    if (E1<E2)
        return -1;
   
    return 1;
   }




double Gamma1_3Length::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return (pow(fabs(gamma), 0.3333)*params.getLength());
   }

double Gamma1_6Length::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return (pow(fabs(gamma), 1.0/6.0)*params.getLength());
   }


double Gamma3Length::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return fabs(gamma*gamma*gamma*params.getLength());
   }

double Gamma4Length::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return fabs(gamma*gamma*gamma*gamma*params.getLength());
   }

double Gamma3_2Length::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return fabs(gamma*sqrt(fabs(gamma))*params.getLength());
   }

double GammaGammaLength::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return gamma*gamma*params.getLength();
   }


double AbsGammaLength::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return fabs(gamma)*params.getLength();
   }


double SqrtGammaLength::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return sqrt(fabs(gamma))*params.getLength();
   }


double ExpGammaLength::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return exp(fabs(gamma))*params.getLength();
   }


double Elastica::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());
    
    return ((gamma*gamma*params.getLength()*params.getLength()*params.getLength())/3.0) + params.getLength()*params.getK0()*params.getK0() + gamma*params.getK0()*params.getLength()*params.getLength();
   }


double TurningAngle::computeEnergy(const EulerSpiralParams &params)
  {
    return params.getTurningAngle(); 
   }


double Length::computeEnergy(const EulerSpiralParams &params)
  {
    return params.getLength();
   }



//(0,0)   to (180,180)  : 0.20
//(0,180) to (180, 360) : 0.23

double GammaPowLength::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return (pow(fabs(gamma), 0.20)*params.getLength());
   }


double FirstQuadEnergy::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return (pow(fabs(gamma), 0.20)*params.getLength());
   }

double FourthQuadEnergy::computeEnergy(const EulerSpiralParams &params)
  {
    double gamma=0;
    gamma = ((params.getK2()-params.getK0())/params.getLength());

    return (pow(fabs(gamma), 0.23)*params.getLength());
   }


