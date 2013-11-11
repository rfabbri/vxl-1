#ifndef  ES_ENERGY_H
#define  ES_ENERGY_H

#include <algo_euler.h>

class BaseEnergy
  {
    public:
        virtual double computeEnergy(const EulerSpiralParams &params)=0;
        /*
         * Return :
         *  -1 iff params1 has lower energy.
         *   0 iff energy of params1 == energy of params2.
         *   1 iff params2 has lower energy.
         *   */
        int getOrder(const EulerSpiralParams &params1, const EulerSpiralParams &params2);
   };


class FirstQuadEnergy :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };

class FourthQuadEnergy :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };

class Gamma1_3Length :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };

class Gamma1_6Length :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };

class GammaPowLength :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };

class Gamma3Length :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };

class Gamma4Length :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };

class Gamma3_2Length :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };

class GammaGammaLength :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };


class AbsGammaLength   :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };

class SqrtGammaLength  :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };


class ExpGammaLength   :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };


class Elastica         :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };

class TurningAngle     :public BaseEnergy
  {
    public:
        double computeEnergy(const EulerSpiralParams &params);
   };

class Length           :public BaseEnergy
  {
    public:    
        double computeEnergy(const EulerSpiralParams &params);
   };



#endif  /*  ES_ENERGY_H   */
