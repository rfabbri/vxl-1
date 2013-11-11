#ifndef EULER_EQUATION_H
#define EULER_EQUATION_H

#include <vector>
#include <math.h>
#include <base/points.h>

#define      MIN_GAMMA   1e-10



class EulerEquations : public DoubleEquation
  {
    private:
        double           _theta0;
        double           _theta2;
        Point2D<double>  _startPt;
        Point2D<double>  _endPt;

        FresnelIntegral  _fresnelIntegral;

    public :
        EulerEquations(Point2D<double> startPt_, double theta0_,Point2D<double> endPt_, double theta2_)
          {
            _theta0 = fmod(theta0_, 2*M_PI);
            if (_theta0<0)
                _theta0 +=2*M_PI;
            
            _theta2 = fmod(theta2_, 2*M_PI);
            if (_theta2<0)
                _theta2 +=2*M_PI;

            _startPt = startPt_;
            _endPt   = endPt_;
           };

        ~EulerEquations(){};

        DoubleEquationResult operator()(double K0, double L)
          {
            double gamma=0;
            pair<double, double> value;
            pair<double, double> derivative;

            double upper_limit=0, lower_limit=0, const_term=0;
            double theta = 0;
            pair<double, double> fresnel_result;
           
            gamma       = (2*((_theta2-_theta0)-K0*L))/(L*L);
            theta       = _theta0-((K0*K0)/2*gamma);
            
            if (fabs(gamma)<MIN_GAMMA)
                gamma = 0;

            /**
             * Compute Value
             * */
            
            if (gamma==0)
              {
                if (fabs(K0)<K_MIN)
                  {
                    /* straight line */
                    
                    value.setX(_startPt.getX()+L*cos(_theta0));
                    value.setY(_startPt.getY()+L*sin(_theta0));
                   }
                else 
                  { 
                    /* circle */
                    const_term = 1.0/K;
                    value.setX(_startPt.getX()+( const_term)*(sin(K*L+_theta0)-sin(_theta0)));
                    value.setY(_startPt.getY()+(-const_term)*(cos(K*L+_theta0)-cos(_theta0))); 
                   }
               }
            
            if (gamma>0)
              {
                lower_limit =  K0/sqrt(M_PI*gamma);
                upper_limit = (L*gamma+K0)/sqrt(M_PI*gamma);
                const_term  = sqrt(M_PI/gamma);
               }

            if (gamma<0)
              {
                lower_limit =  -K0/sqrt(-M_PI*gamma);
                upper_limit = (L*gamma-K0)/sqrt(-M_PI*gamma);
                const_term  = sqrt(M_PI/-gamma);
               }
              
            fresnel_result = _fresnelIngtegral(upper_limit)-_fresnelIngtegral(lower_limit);

            value.setX(_startPt.getX()+const_term*(cos(theta)*fresnel_result.first-sin(theta)*fresnel_result.second));
            value.setY(_startPt.getY()+const_term*(sin(theta)*fresnel_result.first+cos(theta)*fresnel_result.second));

           
            
            /**
             * Compute Derivative.
             * */
            double x_K0=0, x_L=0,  y_K0=0, y_L=0;
            double gamma_length=0, gamma_length_sqrt_gamma=0, K2=0;
            

            if (gamma==0)
              {
               }
            if (gamma>0)
              {
                gamma_length       = gamma*L;
                gamma_gamma_length = gamma_length*gamma;
                K2                 = gamma*L+K0;
                

                x_K0 = ((const_term/gamma_length)*cos(theta)+(const_term*sin(theta)*K2*K0/(gamma*gamma_length)))*fresnel_result.first+
                        cos(theta)*(cos(K2*K2/(2*gamma))*K0/(gamma*gamma*L));
               }
           
           }

   };
























#endif          /* EULER_EQUATION_H */
