#ifndef EULER_H
#define EULER_H

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

#include "basegui_xpmath.h"

#include "algo_biarc.h"
#include "base_geometry_functions.h"


#define NUM_ITERATIONS               50000
#define DEFAULT_INTERVAL_STEP_SIZE   0.01 

#define GAMMA_MIN                    1e-10  //Do NOT Change

#define K_STEP                       0.1
#define K_MIN                        1e-15

#define HUGE_ERROR                   1000000

#define POS_LIMIT                    -1
#define NEG_LIMIT                     1

#define CLOCKWISE                    -1
#define COUNTER_CLOCKWISE             1


class EulerSpiralParams
  {
    private:
        Point2D<double> _startPt;
        Point2D<double> _endPt;

        double _startTangent;
        double _endTangent;
        double _K0;
        double _K2;
        double _length;
        double _turningAngle;
        double _error;

    public :
       
        /**
         * The get functions.
         * */

        Point2D<double> getStartPt() const
          {
            return _startPt;
           }

        Point2D<double> getEndPt() const
          {
            return _endPt;
           }
        
        double getStartTangent() const
          {
            return _startTangent;
           }
        
        double getEndTangent() const
          {
            return _endTangent;
           }
       
        double getK0() const
          {
            return _K0;
           }

        double getK2() const
          {
            return _K2;
           }

        double getLength() const
          {
            return _length;
           }
       
        double getError() const
          {
            return _error;
           }
       
        double getTurningAngle() const
          {
            return _turningAngle;
           }

        /**
         * Return angles in the range :
         * theta0 -->[0, PI]
         * theta2 -->[0, 2PI)
         * Needed for obtaining values from the lookup table.
         *
         * The functions assumes that the starting and ending tangents 
         * belong to [0, 2Pi).
         * */
        vcl_pair<double, double> getTableAngles() const
          {
            vcl_pair<double, double> angles;

            double start_tangent = _startTangent;
            double end_tangent   = _endTangent;

            if (start_tangent>M_PI)
              {
                if (end_tangent<M_PI)
                  {
                    vcl_swap(start_tangent, end_tangent);
                   }
                else
                  {
                    double temp   = 0;
                    temp          = 2*M_PI-end_tangent;
                    end_tangent   = 2*M_PI-start_tangent;
                    start_tangent = temp;
                   }
               }
            
            if ((start_tangent<=M_PI))
              {
                if (end_tangent<=start_tangent)
                  {
                    angles.first  = start_tangent;
                    angles.second = end_tangent;
                    
                    return angles;
                   }

                if (end_tangent<M_PI)
                  {
                    angles.first  = end_tangent;
                    angles.second = start_tangent;
                    
                    return angles;
                   }

                if (end_tangent>=(2*M_PI-start_tangent))
                  {
                    angles.first  = start_tangent;
                    angles.second = end_tangent;
                    
                    return angles;
                   }
              
                if (end_tangent<(2*M_PI-start_tangent))
                  {
                    angles.first  = 2*M_PI-end_tangent;
                    angles.second = 2*M_PI-start_tangent;
                    
                    return angles;
                   }

               }

            vcl_cout<<" <EulerSpiralParams::getTableAngles> : Weird! Angles = {"<<_startTangent*(180/M_PI)<<", "<<_endTangent*(180/M_PI)<<"} "<<vcl_endl;
            return vcl_pair<double, double>(0,0);
           }

        /*
         * The set functions ..
         * */


        int setStartPt(Point2D<double> startPt_)
          {
            _startPt=startPt_;
            return 0;
           }

        int setEndPt(Point2D<double> endPt_)
          {
            _endPt=endPt_;
            return 0;
           }
        
        int setStartTangent(double startTangent_)
          {
            _startTangent=fmod(startTangent_, 2*M_PI);
            if (_startTangent<0)
                _startTangent +=2*M_PI;
            
            return 0;
           }
        
        int setEndTangent(double endTangent_)
          {
            _endTangent=fmod(endTangent_, 2*M_PI);
            if (_endTangent<0)
                _endTangent +=2*M_PI;
            
            return 0;
           }
       
        int setK0(double K0_)
          {
            _K0=K0_;
        return 0;
           }

        int setK2(double K2_)
          {
             _K2=K2_;
         return 0;
           }

        int setLength(double length_)
          {
            _length=length_;
            return 0;
           }
       
        int setError(double error_)
          {
             _error=error_;
             return 0;
           }
       
        int setTurningAngle(double turningAngle_)
          {
            _turningAngle= turningAngle_;
            return 0;
           }


        
        EulerSpiralParams()
          {
            _startTangent  = 0;
            _endTangent    = 0;
            _K0            = 0;
            _K2            = 0;
            _length        = 0;
            _turningAngle  = 0;
            _error         = 0;
           }

        ~EulerSpiralParams(){};

        EulerSpiralParams(const EulerSpiralParams &rhs)
          {
            _K0            = rhs.getK0();
            _K2            = rhs.getK2();
            _length        = rhs.getLength();
            _turningAngle  = rhs.getTurningAngle();

            _startPt      = rhs.getStartPt();
            _endPt        = rhs.getEndPt();
            _startTangent = rhs.getStartTangent();
            _endTangent   = rhs.getEndTangent();

            _error        = rhs.getError();
           }
        
        EulerSpiralParams& operator=(const EulerSpiralParams &rhs)
          {
            if (this!=&rhs)
              {
                _K0            = rhs.getK0();
                _K2            = rhs.getK2();
                _length        = rhs.getLength();
                _turningAngle  = rhs.getTurningAngle();

                _startPt      = rhs.getStartPt();
                _endPt        = rhs.getEndPt();
                _startTangent = rhs.getStartTangent();
                _endTangent   = rhs.getEndTangent();

                _error        = rhs.getError();
               }
            return *this;
           }

        double gamma() const
          {
            if (_length!=0)
                return ((double)((_K2-_K0)/(_length*1.0)));
            else
                return 0;
           }
       
        double normalizedGamma() const
          {
            if (_length==0)
                return 0;

            double dist=0;
            double temp_K0=0, temp_K2= 0, temp_Length=0;

            dist = euc_distance(_startPt, _endPt);

            temp_K0     = _K0*dist;
            temp_K2     = _K2*dist;
            temp_Length = _length/dist;
            
            return ((temp_K2-temp_K0)/temp_Length);
           }

        double bumpFactor(void)
          {
            double dist = euc_distance(_startPt, _endPt);
            if (dist==0)
                return 0;
            double max_len = 0;
            double factor = 0;
            
            
            max_len = dist*M_PI;

            factor = (_length*2.0)/max_len;
            return factor;
           }

        double totalCurvature()
          {
            double dist = euc_distance(_startPt, _endPt);
            if (dist==0)
                return 0;
            double factor = 0;

            factor = (_length*1.0)/dist;
            return factor;
           }

   };

class EulerSpiral
  {

    private :

        Point2D<double> _start_pt;
        Point2D<double> _end_pt;

        Point2D<double> _normalized_end_pt;
        
        double          _start_angle;
        double          _end_angle;


        double          _normalized_start_angle;
        double          _normalized_end_angle;

        vcl_vector<EulerSpiralParams>  _lookup_table;
        
        vcl_vector<EulerSpiralParams> _es_params;
        vcl_vector< Point2D<double> > _euler_spiral;

        BiArc          _intial_bi_arc_estimates;

    public:

        EulerSpiral()
          {
           };

        int set_start_params(Point2D<double> start_pt, double start_angle)
          {
            _start_pt    = start_pt;
            _start_angle = start_angle;

            _start_angle = fmod(_start_angle,2*M_PI);
           
            if (_start_angle<0)
                _start_angle +=2*M_PI;
            return 0;
           }

        int set_end_params(Point2D<double> end_pt, double end_angle)
          {
            _end_pt    = end_pt;
            _end_angle = end_angle;
         
            _end_angle = fmod(_end_angle, 2*M_PI);
            if (_end_angle<0)
                _end_angle +=2*M_PI;

            return 0;
           }

        int set_params(Point2D<double> start_pt, double start_angle, Point2D<double> end_pt, double end_angle)
          {
            set_start_params(start_pt,start_angle);
            set_end_params(end_pt, end_angle);
            normalize();
            return 0;
           }
        
        vcl_vector<EulerSpiralParams> & get_es_params(void)
          {
            return _es_params;
           }

        int get_euler_spiral_params(EulerSpiralParams &params)
          {
            if (euc_distance(_start_pt, _end_pt)<0.01)
                return -1; 

            find_bi_arc_params();
            find_es_parameters();
            if (_es_params.size()<=0)
              {
                vcl_cout<<" Warning : No ES present!"<<vcl_endl;
                vcl_cout<<" Input Params = Start Pt ="<<_start_pt<<" Angle = "<<_start_angle*(180/M_PI)<<vcl_endl;
                vcl_cout<<"                End   Pt ="<<_end_pt  <<" Angle = "<<_end_angle*(180/M_PI)<<vcl_endl; 
                return -1;
               }
            else
              {
                params =  _es_params[0];
               }
            return 0;
           }

        int getDoubleParams(vcl_vector <EulerSpiralParams> &params);
        
        Point2D<double> compute_es_point(Point2D<double> start_pt, double start_tangent, double K0, double TotalLength, double K2, double arclength)
          {
            set_start_params(start_pt, start_tangent);
            double gamma=((K2)-(K0))/(TotalLength);
            return compute_end_pt(start_pt, K0, gamma, arclength, false);
           }

        Point2D<double> compute_es_point(const EulerSpiralParams &params, double arclength)
          {
            set_start_params(params.getStartPt(), params.getStartTangent());
            double gamma=((params.getK2())-(params.getK0()))/(params.getLength());
            return compute_end_pt(params.getStartPt(), params.getK0(), gamma, arclength, false);
           }
        
        int normalize()
          {
            double slope        = atan2(_end_pt.getY()-_start_pt.getY(), _end_pt.getX()-_start_pt.getX());
            if (slope<0)
                slope +=2*M_PI;
           
            _normalized_start_angle = _start_angle - slope;
            _normalized_end_angle   = _end_angle   - slope;

            _normalized_start_angle = fmod(_normalized_start_angle, 2*M_PI);
            _normalized_end_angle   = fmod(_normalized_end_angle, 2*M_PI);

//            vcl_cout<<" Start Angle = "<<_start_angle<<" Normalized = "<<_normalized_start_angle<<" End = "<<_end_angle<<" NOrmailized = "<<_normalized_end_angle<<vcl_endl;
            if (_normalized_start_angle <0)
                _normalized_start_angle +=2*M_PI;

            if (_normalized_end_angle<0)
                _normalized_end_angle +=2*M_PI;
                
            
            _normalized_end_pt      = Point2D<double>(1, 0);
            return 0;
           }
       
        int get_parameters(Point2D<double> &start_pt, double &start_angle, Point2D<double> &end_pt, double &end_angle)
          {
            start_pt    = _start_pt;
            start_angle = _start_angle;

            end_pt      = _end_pt;
            end_angle   = _end_angle;
            return 0;
           }

        int scale_bi_arc_params(void)
          {
            double factor=0;

            factor = euc_distance(_start_pt, _end_pt);

            for (unsigned int i=0;i<_intial_bi_arc_estimates.bi_arc_params.size();i++)
             {
               _intial_bi_arc_estimates.bi_arc_params[i].scale(factor);
              }
           return 0;
           }
       
        EulerSpiralParams getParams(void)
          {
            if (euc_distance(_start_pt, _end_pt)<0.01)
                return EulerSpiralParams(); 
            
            find_bi_arc_params();
            find_es_parameters();
            
            if (_es_params.size()>0)
                return _es_params[0];
            else
                return EulerSpiralParams();
           }

        Point2D<double> get_fresnel_integral      (double value);
        Point2D<double> compute_end_pt            (Point2D<double> start, double K, double gamma, double arclength, bool use_normalized);
        inline double          compute_error             (Point2D<double> start, Point2D<double> end, double K, double L, double angle_diff, double &K2);


        int  computeSpiral(vcl_vector<Point2D<double> > &spiral_, double step_size_, int count_, bool useInternal_);
        int  find_es_parameters  (void);
        int  compute_euler_spiral(void);
        int  find_bi_arc_params  (void);
        inline double   minimum  (const vcl_vector<double> &array,int& index);
       

        int get_bi_arc_info(int count, BiArcParams &bi_arc);
        int load_lookup_table();
        EulerSpiralParams get_initial_estimates(int);

        
   };

inline vcl_ostream& operator<<(vcl_ostream &os, const EulerSpiralParams &params)
  {
    os<<" Start Point   ="<<params.getStartPt()<<" End Point   ="<<params.getEndPt()<<vcl_endl;
    os<<" Start Tangent ="<<params.getStartTangent()*(180/M_PI)<<" End Tangent ="<<params.getEndTangent()*(180/M_PI)<<vcl_endl;
    os<<" K0 ="<<params.getK0()<<" K2 ="<<params.getK2()<<vcl_endl;
    os<<" Length ="<<params.getLength()<<vcl_endl;
    os<<" Turning Angle ="<<params.getTurningAngle()*(180/M_PI)<<vcl_endl;
    
    return os;
   }




#endif    /*EULER_H */
