#ifndef _BIARC_SHOCK_H
#define _BIARC_SHOCK_H

//#include "BiArc.h"
#include "ishock-common.h"

#define EPSILON                      1.0e-5
//#define NUM_ITERATIONS               30000
//#define DEFAULT_INTERVAL_STEP_SIZE   10 

#define GAMMA_MIN                    1e-10

#define K_STEP                       0.1
#define K_LARGE                      100000
#define K_MIN                        1e-15

#define INF                          1e8     // This and the next MUST have different values.
#define INF_PI                       1e9



#define MIN_DENOMINATOR              1e-7

#define POS_LIMIT                    -1
#define NEG_LIMIT                     1

#define CLOCKWISE                    -1
#define COUNTER_CLOCKWISE             1

#define             INF_RADIUS              1e4

class BiArcShockParams{
    public:
        Point start_pt;
        Point end_pt;

        double start_angle;
        double end_angle;
        
        double K1;
        double Length1;
        
        double K2;
        double Length2;

        double angle_diff;
  
        Point center1;
        Point center2;
        
        Point start1;
        Point start2;

        Point end1;
        Point end2;

        int dir1;
        int dir2;
       
        double radius1;
        double radius2;

        
        BiArcShockParams(){
            K1           = 0;
            Length1      = 0;
            K2           = 0;
            Length2      = 0;

            dir1         = 0;
            dir2         = 0;

            radius1      = 0;
            radius2      = 0;

            angle_diff   = 0;
        
            start_angle  = 0;
            end_angle    = 0;
           
        };
        
        ~BiArcShockParams(){};
        
        BiArcShockParams( const BiArcShockParams &rhs){
            K1          = rhs.K1;
            Length1     = rhs.Length1;
            K2          = rhs.K2;
            Length2     = rhs.Length2;

            center1     = rhs.center1;
            center2     = rhs.center2;

            start1      = rhs.start1;
            start2      = rhs.start2;

            end1        = rhs.end1;
            end2        = rhs.end2;

            dir1        = rhs.dir1;
            dir2        = rhs.dir2;

            radius1     = rhs.radius1;
            radius2     = rhs.radius2;

            angle_diff  = rhs.angle_diff;

           
            start_pt    = rhs.start_pt;
            end_pt      = rhs.end_pt;

            start_angle = rhs.start_angle;
            end_angle   = rhs.end_angle;
        };

        BiArcShockParams& operator=(const BiArcShockParams &rhs)
          {
            if (this!=&rhs)
              {
                K1          = rhs.K1;
                Length1     = rhs.Length1;
                K2          = rhs.K2;
                Length2     = rhs.Length2;
         
                center1     = rhs.center1;
                center2     = rhs.center2;
                
                start1      = rhs.start1;
                start2      = rhs.start2;
                
                end1        = rhs.end1;
                end2        = rhs.end2;
                
                dir1        = rhs.dir1;
                dir2        = rhs.dir2;
                
                radius1     = rhs.radius1;
                radius2     = rhs.radius2;
               
                angle_diff  = rhs.angle_diff;
               
                start_pt    = rhs.start_pt;
                end_pt      = rhs.end_pt;
                
                start_angle = rhs.start_angle;
                end_angle   = rhs.end_angle;
               }
            return *this;
           }

        double get_total_length()
        {
            return (Length1+Length2);
        }
        
        int scale (double factor)
          {
            K1      /=factor;
            Length1 *=factor;

            K2      /=factor;
            Length2 *=factor;

            return 0;
           }
 
        double compute_angle_diff()
          {
            
            if ((vcl_fabs(K1)<EPSILON)||(vcl_fabs(K2)<EPSILON))
              {
               double temp_theta1=0, temp_theta2=0; 
               //TODO
               
               if (vcl_fabs(K1)<EPSILON)
                 {
                   if (Length1==INF)
                       temp_theta1 = 2*M_PI;
                   else
                     {
                       if (Length1 == INF_PI)
                         {
                           temp_theta1 = M_PI;
                          }
                       else
                           temp_theta1 = K1*Length1;
                      }
                  }
               else
                 {
                 
                   temp_theta1 = K1*Length1;
                  }

               if (vcl_fabs(K2)<EPSILON)
                 {
                   if (Length2==INF)
                       temp_theta2 = 2*M_PI;
                   else
                     { 
                       if (Length2 == INF_PI)
                         {
                           temp_theta2 = M_PI;
                          }
                       else
                           temp_theta2 = K2*Length2;
                      }
                  }
               else
                 {
                   
                   temp_theta2 = K2*Length2;
                  }

               angle_diff =temp_theta1+temp_theta2;
               }
            else
              {
                angle_diff = K1*Length1+K2*Length2;
               }
            
            return angle_diff;
           }
   };

class BiArcShock{

    private:
        Point _start_pt;
        Point _end_pt;
        
        double  _start_angle;
        double  _end_angle;

    public:
        BiArcShockParams bi_arc_params;

        BiArcShock()
          {
            // Points are already intialized.
            _start_angle = 0;
            _end_angle   = 0;
           };

        BiArcShock(Point start_pt, double start_angle, Point end_pt, double end_angle)
          {
            _start_pt     = start_pt;
            _start_angle  = start_angle;
            
            _end_pt       = end_pt;
            _end_angle    = end_angle;
           };

        int     compute_biarc_params (void);
      void    compute_other_stuff(void);
        double  compute_join_theta   (double k1, double k2);
        double  compute_arclength    (double theta0,double theta2,double k);


        ~BiArcShock(){}

        const BiArcShockParams &get_bi_arc_params()
          {
            return bi_arc_params;
           }

        int set_start_params(Point start_pt, double start_angle)
          {
            _start_pt    = start_pt;
            _start_angle = start_angle;

            _start_angle =  vcl_fmod (_start_angle,2*M_PI);
            
            if (_start_angle<0)
                _start_angle +=2*M_PI;

            return 0;
           }

        int set_end_params(Point end_pt, double end_angle)
          {
            _end_pt    = end_pt;
            _end_angle = end_angle;
           
            _end_angle =  vcl_fmod (_end_angle, 2*M_PI);
            if (_end_angle<0)
                _end_angle +=2*M_PI;
            
            return 0;
           }
   
   
   };

#endif
