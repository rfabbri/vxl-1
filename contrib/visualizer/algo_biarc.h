#ifndef       BI_ARC_H
#define       BI_ARC_H


#include <vcl_vector.h>
#include <vcl_iostream.h>

#include <basegui_xpmath.h>

#include <base_geometry_functions.h>


#define EPSILON                      1.0e-5
#define INF                          1e8     
#define MIN_DENOMINATOR              1e-7
#define K_LARGE                      100000


class BiArcParams
  {
    private: 
        double _firstTurningAngle;
        double _secondTurningAngle;
        double _computeFirstTurningAngle();
        double _computeSecondTurningAngle();
    
    public:
        Point2D<double> start_pt;
        Point2D<double> end_pt;

        double start_angle;
        double end_angle;
        double join_angle;
        
        
        
        double K1;
        int    k1Sign;
        double Length1;
        
        double K2;
        int    k2Sign;
        double Length2;

        double angle_diff;
  

        /**
         * Actual Values (NOT Normalized)
         * */
        Point2D<double> circle1Center;
        Point2D<double> circle2Center;
        
        Point2D<double> circle1Start;
        Point2D<double> circle2Start; 

        Point2D<double> circle1End;
        Point2D<double> circle2End;

        int circle1Dir;
        int circle2Dir;
       
        double circle1Radius;
        double circle2Radius;

        
        BiArcParams();
        ~BiArcParams();
        
        BiArcParams( const BiArcParams &rhs);
        BiArcParams& operator=(const BiArcParams &rhs);
        double getFirstTurningAngle();
        double getSecondTurningAngle();
        double compute_angle_diff();
        double computeTurningAngle();
        int    scale (double factor);
        int    computeBiArcInfo(void);
        Point2D<double>  getCenter(Point2D<double>  start, double angle, double radius);
   };

class BiArc
  {
    private:
        Point2D<double> _start_pt;
        Point2D<double> _end_pt;
        
        double          _start_angle;
        double          _end_angle;

    public:
        vcl_vector<BiArcParams> bi_arc_params;

        BiArc()
          {
            // Points are already intialized.
            _start_angle = 0;
            _end_angle   = 0;
           };

        BiArc(Point2D<double> start_pt, double start_angle, Point2D<double> end_pt, double end_angle)
          {
            _start_pt     = start_pt;
            _start_angle  = start_angle;
            
            _end_pt       = end_pt;
            _end_angle    = end_angle;
           };

        int     compute_biarc_params (void);
        double  compute_join_theta   (double k1, double k2);
        double  compute_arclength    (double theta0,double theta2,double k);


        ~BiArc()
          {
            bi_arc_params.clear();
           }

        const vcl_vector<BiArcParams> &get_bi_arc_params();
        int set_start_params(Point2D<double> start_pt, double start_angle);
        int set_end_params(Point2D<double> end_pt, double end_angle);
        
        void set_params(Point2D<double> start_pt, double start_angle,Point2D<double> end_pt, double end_angle)
          {
            set_start_params(start_pt, start_angle);
            set_end_params(end_pt, end_angle);
           }

        int getParams(vcl_vector<BiArcParams> &params);
        BiArcParams getParams(void);
   };

vcl_ostream& operator<<(vcl_ostream &os, BiArcParams &params);
int getSinSign(double angle);
int getCosSign(double angle);



#endif       /*-   BI_ARC_H  -*/
