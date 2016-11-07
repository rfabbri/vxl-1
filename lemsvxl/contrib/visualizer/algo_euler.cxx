#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_string.h>

#include "base_points.h"
#include "base_geometry_functions.h"
#include "algo_biarc.h"
#include "algo_euler.h" 

/**
 * Changes : 
 *
 * 31-Jul-2002 :  Fixed a bug in computing Winding Angles.
 * 1-Aug-2002  :  Fixed : Angle==2*M_PI in getSinSign and getCosSign.      
 *                        set k1Sign and k2Sign in case where bi-arc 
 *                        degenerates to single arc.
 *
 *
 **/ 


/*----------------- ES functions -----------------------*/

int EulerSpiral::find_bi_arc_params(void)
  {
    _intial_bi_arc_estimates.set_start_params (Point2D<double>(0,0), _normalized_start_angle);
    _intial_bi_arc_estimates.set_end_params   (_normalized_end_pt,   _normalized_end_angle);
    _intial_bi_arc_estimates.compute_biarc_params();

    return 0;
   }


/* 
 * To compute the fresnel solution given the specified parameters
 * ie: to compute the point x,y of a segment of the euler spiral specified by said params at a given
 * arc-length
 */
Point2D<double> EulerSpiral::compute_end_pt(Point2D<double> start_pt, double K, double gamma, double arclength, bool use_normalized)
  {
    /* 
     * start       = location of start point 
     * theta       = orientation of start point in RADIANS 
     * K           = curvature of start point 
     * gamma       = rate of change of curvature
     * arclength   = in the set of: [0,L] 
     *
     * */
    if (arclength<0)
      {
        vcl_cout<< " Error:<compute_end_pt> Arc Length value is negative!! : "<<arclength<<vcl_endl;
        return _start_pt;
       }


    Point2D<double> fresnel_result;
    Point2D<double> first;
    Point2D<double> second;

    float a=0,b=0,cos_term=0,sin_term=0,const_term=0;

    double theta;

    if (use_normalized)
        theta = _normalized_start_angle;
    else
        theta = _start_angle;


    /* 
     * consider all cases for gamma 
     * compute corresponding fresnel integral C(x),S(x) 
     *
     * */

    /*
     * NOTE: tiny gamma evaluates to 0
     * */


    //NOTE : Fixed BUG. Reduced the threshold.
    if (vcl_fabs(gamma)<GAMMA_MIN)
        gamma =0;

    if (gamma>0) 
      {
        first  = get_fresnel_integral((K+(gamma*arclength))/sqrt(M_PI*gamma));
        second = get_fresnel_integral((K/sqrt(M_PI*gamma)));

        a = (first.getX())-(second.getX());
        b = (first.getY())-(second.getY());

        cos_term   = (float)cos(theta-((K*K)/(2.0*gamma)));
        sin_term   = (float)sin(theta-((K*K)/(2.0*gamma))); 
        const_term = (float)(sqrt(M_PI/gamma));

        fresnel_result.setX(start_pt.getX()+(const_term)*(cos_term*a-sin_term*b));
        fresnel_result.setY(start_pt.getY()+(const_term)*(sin_term*a+cos_term*b));
       }

    if (gamma<0) 
      { 

        first  = get_fresnel_integral((-K+(-gamma*arclength))/sqrt(-M_PI*gamma));
        second = get_fresnel_integral((-K/sqrt(-M_PI*gamma)));

        a =   (first.getX())-(second.getX());
        b = -((first.getY())-(second.getY()));

        cos_term   = (float)cos(theta-((K*K)/(2.0*gamma)));
        sin_term   = (float)sin(theta-((K*K)/(2.0*gamma))); 
        const_term = (float)(sqrt(M_PI/-gamma));

        fresnel_result.setX(start_pt.getX()+(const_term)*(cos_term*a-sin_term*b));
        fresnel_result.setY(start_pt.getY()+(const_term)*(sin_term*a+cos_term*b));
       }

    if (gamma==0) 
      { 
        if (vcl_fabs(K)<K_MIN)
          {
            /* straight line */

            fresnel_result.setX(start_pt.getX()+arclength*cos(theta));
            fresnel_result.setY(start_pt.getY()+arclength*sin(theta));
           }
        else 
          { 
            /* circle */
            const_term = 1.0/K;
            fresnel_result.setX(start_pt.getX()+(const_term)*(sin(K*arclength+theta)-sin(theta)));
            fresnel_result.setY(start_pt.getY()+(-const_term)*(cos(K*arclength+theta)-cos(theta))); 
           }
       }


    return fresnel_result;
   }


/* 
 * To compute the error between the computed and the intended final coordinates 
 * */
inline double EulerSpiral::compute_error(Point2D<double> start_pt, Point2D<double> end_pt, double K0, double L, double angle_diff_orig, double &K2)
  {
    /*
     *NOTE: theta0 and theta1 MUST be in RADIANS
     * 
     * (start,theta0) = location & orientation of initial point 
     * (end  ,theta1) = location & orientation of final point 
     * L              = length of the spiral piece (ESTIMATE) 
     * K0             = curvature (ESTIMATE) 
     *
     * */



    double gamma=0,error=0, new_K2=0;
    double neg_gamma=0;
    double temp_L1=0, temp_L2 = 0;
    double determinant=0;



    Point2D<double> fresnel_result, functional;

    gamma = 2.0*(angle_diff_orig-K0*L)/(L*L);
    new_K2 = gamma*L+K0;
    neg_gamma = -gamma;

    if (vcl_fabs(gamma)>1e-7)
      {
        determinant = ((K0*K0)+2*gamma*2*M_PI);
        if (determinant>0)
          {
            temp_L1 = (-(K0)-(sqrt(determinant)))/(gamma);
            temp_L2 = (-(K0)+(sqrt(determinant)))/(gamma);

            if ((temp_L1>0)&&(temp_L1<L))
              {
                return HUGE_ERROR;
               }

            if ((temp_L2>0)&&(temp_L2<L))
              {
                return HUGE_ERROR;
               }
           }


        determinant = ((K0*K0)-2*gamma*2*M_PI);
        if (determinant>0)
          {
            temp_L1 = (-(K0)-(sqrt(determinant)))/(gamma);
            temp_L2 = (-(K0)+(sqrt(determinant)))/(gamma);

            if ((temp_L1>0)&&(temp_L1<L))
              {
                return HUGE_ERROR;
               }

            if ((temp_L2>0)&&(temp_L2<L))
              {
                return HUGE_ERROR;
               }
           }


        determinant =((new_K2*new_K2)+2*neg_gamma*2*M_PI);
        if (determinant>0)
          {
            temp_L1 = (-(new_K2)-(sqrt(determinant)))/(neg_gamma);
            temp_L2 = (-(new_K2)+(sqrt(determinant)))/(neg_gamma);

            if ((temp_L1>0)&&(temp_L1<L))
              {
                return HUGE_ERROR;
               }

            if ((temp_L2>0)&&(temp_L2<L))
              {
                return HUGE_ERROR;
               }
           }


        determinant = ((new_K2*new_K2)-2*neg_gamma*2*M_PI);
        if (determinant>0)
          {
            temp_L1 = (-(new_K2)-(sqrt(determinant)))/(neg_gamma);
            temp_L2 = (-(new_K2)+(sqrt(determinant)))/(neg_gamma);

            if ((temp_L1>0)&&(temp_L1<L))
              {
                return HUGE_ERROR;
               }

            if ((temp_L2>0)&&(temp_L2<L))
              {
                return HUGE_ERROR;
               }
           }


       } 


    if (((new_K2>=POS_LIMIT)&&(K2>=POS_LIMIT))||((new_K2<=NEG_LIMIT)&&(K2<=NEG_LIMIT)))
      {
        K2 = new_K2;
        fresnel_result = compute_end_pt(start_pt, K0,gamma,L, true);

        functional=fresnel_result-end_pt;
        error=sqrt((functional.getX()*functional.getX())+(functional.getY()*functional.getY()));

        return error;
       }
    else
      {
        return HUGE_ERROR;
       }
   }



/*
 * Compute co-ordinates of Euler Spiral.
 * Spirallace results in _euler_spiral (class variable)
 * or in the function argument according to the value of useInternal_.
 * */
int EulerSpiral::computeSpiral(vcl_vector<Point2D<double> > &spiral_, double step_size_, int count_, bool useInternal_)
  {
    double gamma      = 0;
    double s          = 0;    /* initial (default) value; NOTE: s inthesetof: [0,L] */
    int num_intervals = 0;
    double k_start=0, k_end=0, length=0;
    Point2D<double> prev_pt;

    if (step_size_ <=0)
        step_size_ = DEFAULT_INTERVAL_STEP_SIZE;

    if (_es_params.size()==0)
        return -1;
    length  = _es_params[0].getLength();
    k_start = _es_params[0].getK0();
    k_end   = _es_params[0].getK2();

    num_intervals = (int)(length/step_size_); 

    if (useInternal_)
      {
        _euler_spiral.clear();
        _euler_spiral.push_back(_start_pt);
       }
    else
      {
        spiral_.push_back(_start_pt);
       }


    if (length==0)
      {
        if (useInternal_)
          {
            _euler_spiral.push_back(_start_pt);
           }
        else
          {
            spiral_.push_back(_start_pt);
           }
        return 0;
       }

    gamma=((k_end)-(k_start))/(length);

    prev_pt = _start_pt; 
    Point2D<double> temp;


    if (useInternal_)
      {
        for (int j=0; j<(num_intervals+1);j++)
          {
            temp = compute_end_pt(_start_pt, k_start, gamma, s, false);
            if (temp!=prev_pt)
              {
                _euler_spiral.push_back(temp);
               }
            prev_pt = temp;
            s+=step_size_;
           }
       }
    else
      {
        for (int j=0; j<(num_intervals+1);j++)
          {
            temp = compute_end_pt(_start_pt, k_start, gamma, s, false);
            if (temp!=prev_pt)
              {
                spiral_.push_back(temp);
               }
            prev_pt = temp;
            s+=step_size_;
           }
       }
    return 0; 
   }


/* 
 * ITERATIVELY compute euler spiral solution given the conditions in the param list 
 *
 * */
int EulerSpiral::find_es_parameters(void)
  {
    /*
     * NOTE: 
     *
     * (1) All angles MUST be in RADIANS
     *
     * (2) The end points are first normalized.(i.e., the distance between them is 
     * reduced to 1). The parameters are then computed  and scaled by the initial distance 
     * between the two points.
     * 
     * */



    _es_params.clear();
    if (_start_pt==_end_pt)
      {
        EulerSpiralParams es_params;

        es_params.setK0(K_LARGE);
        es_params.setK2(K_LARGE);
        es_params.setLength(0);

        _es_params.push_back(es_params);
        return 0;
       }


    double potential_K0=0, potential_L=0;
    double error=0, prev_error=1000.0;

    double k_step_small=0;
    double k_step_large=0;
    double l_step_small=0;
    double l_step_large=0;

    vcl_vector<double> errors(16);
    int  error_index=0; 

    double dist_bet_pts=0;
    dist_bet_pts = euc_distance(_start_pt, _end_pt);


    if (_intial_bi_arc_estimates.bi_arc_params.size()==0)
      {
        vcl_cout<<" Error : No Bi-Arcs Computed!!"<<vcl_endl;
        return 0;
       }

    EulerSpiralParams es_params, old_params;
    int ret=0;
    int discard = 0;

    double k1Sign=0, k2Sign=0;
    for (unsigned int bi_arc_count=0;bi_arc_count<_intial_bi_arc_estimates.bi_arc_params.size();bi_arc_count++)
      {
        prev_error       = 100000;


        es_params = get_initial_estimates(bi_arc_count);
        es_params.setTurningAngle(_intial_bi_arc_estimates.bi_arc_params[bi_arc_count].compute_angle_diff());

        es_params.setK0(_intial_bi_arc_estimates.bi_arc_params[bi_arc_count].K1);
        es_params.setK2(_intial_bi_arc_estimates.bi_arc_params[bi_arc_count].K2);
        es_params.setLength(_intial_bi_arc_estimates.bi_arc_params[bi_arc_count].Length1+_intial_bi_arc_estimates.bi_arc_params[bi_arc_count].Length2);

        k1Sign=_intial_bi_arc_estimates.bi_arc_params[bi_arc_count].k1Sign;
        k2Sign=_intial_bi_arc_estimates.bi_arc_params[bi_arc_count].k2Sign;


        if (es_params.getLength()>10)
          {
            if (vcl_fabs((es_params.getK0()+es_params.getK2()))>EPSILON)
              {
                double temp_length=0;

                temp_length = vcl_fabs((2*es_params.getTurningAngle())/(es_params.getK0()+es_params.getK2()));


                if (temp_length<es_params.getLength())
                    es_params.setLength(temp_length);

               }

            if (es_params.getLength()>100)
                es_params.setLength(100);
           }



        //vcl_cout<<" Angle Diff = "<<es_params.getTurningAngle()*(180/M_PI)<<" Count = "<<discard<<vcl_endl;
        if  (vcl_fabs(es_params.getTurningAngle())>2*M_PI)
          {
            discard++;
            if (discard>=2)
                vcl_cout<<" Error : Both Bi-Arcs discarded as both have winding angle >360 degrees"<<vcl_endl;

            //  vcl_cout<<" Angle Diff = "<<es_params.getTurningAngle()*(180/M_PI)<<" Count = "<<discard<<vcl_endl;
            break;
           }  

        if (bi_arc_count==1)
          {
            if (vcl_fabs(_intial_bi_arc_estimates.bi_arc_params[1].compute_angle_diff()-_intial_bi_arc_estimates.bi_arc_params[0].compute_angle_diff())<0.001)
                break;
           }


        k_step_large  =  vcl_fabs(es_params.getK0())/10;
        if (vcl_fabs(k_step_large)<K_STEP)
            k_step_large = K_STEP;

        k_step_small  =  K_STEP;

        l_step_large      = es_params.getLength()/100;
        l_step_small      = es_params.getLength()/100;


        /**
         * For Efficiency :
         * */
        double K0=0, K2=0, length = 0, turning_angle = 0;

        K0            = es_params.getK0();
        K2            = es_params.getK2();
        length        = es_params.getLength();
        turning_angle = es_params.getTurningAngle();


        /**
         * Main Loop for computing ES parameters.
         * CPU Intensive.
         * */

        prev_error=compute_error(Point2D<double> (0,0), _normalized_end_pt,K0, length, turning_angle, K2);
        for (int i=0;i<NUM_ITERATIONS;i++)
          {

            if((prev_error*dist_bet_pts<0.1)||(i==(NUM_ITERATIONS-1)))
              {
                K0      /=dist_bet_pts;
                length  *=dist_bet_pts;


                double gamma=0;
                gamma = 2.0*(turning_angle-K0*length)/(length*length);

                es_params.setK0(K0);
                es_params.setK2(gamma*length+K0);
                es_params.setLength(length);
                es_params.setError(prev_error*dist_bet_pts);
                es_params.setStartPt(_start_pt);
                es_params.setEndPt(_end_pt);
                es_params.setStartTangent(_start_angle);
                es_params.setEndTangent(_end_angle);

                _es_params.push_back(es_params);

  //              vcl_cout<<"-----------------------------"<<vcl_endl;
  //              vcl_cout<<es_params<<vcl_endl;
  //              vcl_cout<<" Error ="<<prev_error*dist_bet_pts<<vcl_endl<<vcl_endl<<vcl_endl;
                
                break;
               }
            else
              {

                potential_K0=K0+k_step_large;
                if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                    errors[0]=compute_error(Point2D<double>(0,0), _normalized_end_pt, potential_K0,length, turning_angle,K2);
                else
                    errors[0]=HUGE_ERROR;


                potential_K0=K0-k_step_large;
                if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                    errors[1]=compute_error(Point2D<double> (0,0), _normalized_end_pt, potential_K0,length, turning_angle, K2);
                else
                    errors[1]=HUGE_ERROR;

                potential_K0=K0+k_step_small;
                if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                    errors[2]=compute_error(Point2D<double>(0,0), _normalized_end_pt, potential_K0,length, turning_angle, K2);
                else
                    errors[2] = HUGE_ERROR;


                potential_K0=K0-k_step_small;
                if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                    errors[3]=compute_error(Point2D<double> (0,0), _normalized_end_pt, potential_K0,length, turning_angle, K2);
                else
                    errors[3]=HUGE_ERROR;




                potential_L=length+l_step_large;
                errors[4]=compute_error(Point2D<double> (0,0), _normalized_end_pt, K0, potential_L, turning_angle, K2);

                if (length>(1+l_step_large))
                  {
                    potential_L=length-l_step_large;
                    errors[5]=compute_error(Point2D<double> (0,0), _normalized_end_pt, K0, potential_L, turning_angle, K2);
                   }
                else
                  {
                    errors[5]=HUGE_ERROR;
                   }

                potential_L=length+l_step_small;
                errors[6]=compute_error(Point2D<double> (0,0), _normalized_end_pt, K0, potential_L, turning_angle, K2);

                if (length>(1+l_step_small))
                  {
                    potential_L=length-l_step_small;
                    errors[7]=compute_error(Point2D<double> (0,0), _normalized_end_pt, K0, potential_L, turning_angle, K2);
                   }
                else
                  {
                    errors[7]=HUGE_ERROR;
                   }




                potential_K0 = K0+k_step_large;
                potential_L  = length+l_step_large;

                if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                    errors[8]=compute_error(Point2D<double>(0,0), _normalized_end_pt, potential_K0,potential_L, turning_angle, K2);
                else
                    errors[8]=HUGE_ERROR;


                potential_K0=K0-k_step_large;
                potential_L =length+l_step_large;

                if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                    errors[9]=compute_error(Point2D<double>(0,0), _normalized_end_pt, potential_K0,potential_L, turning_angle, K2);
                else
                    errors[9]=HUGE_ERROR;

                if (length>1+l_step_large)
                  {
                    potential_K0=K0+k_step_large;
                    potential_L =length-l_step_large;

                    if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                        errors[10]=compute_error(Point2D<double>(0,0), _normalized_end_pt, potential_K0,potential_L, turning_angle, K2);
                    else
                        errors[10]=HUGE_ERROR;

                    potential_K0=K0-k_step_large;
                    potential_L =length-l_step_large;

                    if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                        errors[11]=compute_error(Point2D<double>(0,0), _normalized_end_pt, potential_K0,potential_L, turning_angle, K2);
                    else
                        errors[11]=HUGE_ERROR;

                   }
                else
                  {
                    errors[10]=errors[11]=HUGE_ERROR;
                   }

                potential_K0=K0+k_step_small;
                potential_L =length+l_step_small;


                if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                    errors[12]=compute_error(Point2D<double>(0,0), _normalized_end_pt, potential_K0,potential_L, turning_angle, K2);
                else
                    errors[12]=HUGE_ERROR;

                potential_K0=K0-k_step_small;
                potential_L =length+l_step_small;

                if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                    errors[13]=compute_error(Point2D<double>(0,0), _normalized_end_pt, potential_K0,potential_L, turning_angle, K2);
                else
                    errors[13]=HUGE_ERROR;

                if (length>1+l_step_small)
                  {
                    potential_K0=K0+k_step_small;
                    potential_L =length-l_step_small;

                    if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                        errors[14]=compute_error(Point2D<double>(0,0), _normalized_end_pt, potential_K0,potential_L, turning_angle, K2);
                    else
                        errors[14]=HUGE_ERROR;

                    potential_K0=K0-k_step_small;
                    potential_L =length-l_step_small;

                    if (((potential_K0>=POS_LIMIT)&&(K0>=POS_LIMIT))||((potential_K0<=NEG_LIMIT)&&(K0<=NEG_LIMIT)))
                        errors[15]=compute_error(Point2D<double>(0,0), _normalized_end_pt, potential_K0,potential_L, turning_angle, K2);
                    else
                        errors[15]=HUGE_ERROR;
                   }
                else
                  {
                    errors[14]=errors[15]=1000000;
                   }

                error=minimum(errors, error_index);
                //vcl_cout<<" K0="<<K0<<" Length ="<<length<<" K2="<<K2<<" Error ="<<error<<vcl_endl;

                if ((error>=prev_error)||(error==HUGE_ERROR))
                  {

                    if (l_step_large<length)
                        l_step_large *=1.2;
                    else
                        l_step_large = length/100;

                    if (k_step_large <10)
                        k_step_large *=1.5;
                    else
                      {
                        k_step_large = vcl_fabs(K0)/10;
                        if (vcl_fabs(k_step_large)<K_STEP)
                            k_step_large = K_STEP;
                        if (vcl_fabs(k_step_large)>10)
                            k_step_large = K_STEP;
                       }

                    if (i<100)
                      {
                        if (l_step_small>1e-3)
                            l_step_small *=0.5;
                        else
                            l_step_small =length/100;

                        if (k_step_small >1e-3)
                            k_step_small *=0.5;
                        else
                            k_step_small=K_STEP;

                       }

                    if ((i>100)&&(i<500))
                      {
                        if (l_step_small>1e-4)
                            l_step_small *=0.5;
                        else
                            l_step_small =length/100;

                        if (k_step_small >1e-4)
                            k_step_small *=0.5;
                        else
                            k_step_small=K_STEP;


                       }

                    if ((i>500)&&(i<1000))
                      {
                        if (l_step_small>1e-5)
                            l_step_small *=0.5;
                        else
                            l_step_small =length/100;


                        if (k_step_small >1e-5)
                            k_step_small *=0.5;
                        else
                            k_step_small=K_STEP;

                       }

                    if ((i>1000)&&(i<2000))
                      {
                        if (l_step_small>1e-6)
                            l_step_small *=0.5;
                        else
                            l_step_small =length/100;

                        if (k_step_small >1e-6)
                            k_step_small *=0.5;
                        else
                            k_step_small=K_STEP;
                       }

                    if ((i>2000)&&(i<5000))
                      {
                        if (l_step_small>1e-6)
                            l_step_small *=0.5;
                        else
                            l_step_small =length/100;

                        if (k_step_small >1e-6)
                            k_step_small *=0.5;
                        else
                            k_step_small=K_STEP;
                       }

                    if ((i>5000))
                      {
                        if (l_step_small>1e-6)
                            l_step_small *=0.5;
                        else
                            l_step_small =length/100;

                        if (k_step_small >1e-6)
                            k_step_small *=0.5;
                        else
                            k_step_small=K_STEP;
                       }


                   }
                else
                  {
                    switch(error_index)
                      {
                        case 0:  K0     += k_step_large;      break;
                        case 1:  K0     -= k_step_large;      break;
                        case 2:  K0     += k_step_small;      break;
                        case 3:  K0     -= k_step_small;      break;

                        case 4:  length += l_step_large;      break;
                        case 5:  length -= l_step_large;      break;
                        case 6:  length += l_step_small;      break;        
                        case 7:  length -= l_step_small;      break;        

                        case 8:  K0     += k_step_large; length += l_step_large; break;
                        case 9:  K0     -= k_step_large; length += l_step_large; break;
                        case 10: K0     += k_step_large; length -= l_step_large; break;
                        case 11: K0     -= k_step_large; length -= l_step_large; break;


                        case 12: K0     += k_step_small; length += l_step_small; break;
                        case 13: K0     -= k_step_small; length += l_step_small; break;
                        case 14: K0     += k_step_small; length -= l_step_small; break;
                        case 15: K0     -= k_step_small; length -= l_step_small; break;


                        default: vcl_cout<<"<find_es_parameters> No such error Index"<<vcl_endl;
                       }
                    prev_error=error;

                    switch(error_index)
                      {
                        case 0:
                        case 1:
                              {
                                k_step_small      = K_STEP;

                                l_step_large      = length/100;
                                l_step_small      = length/100;
                                break;
                               }

                        case 2:
                        case 3:
                              {

                                k_step_large = vcl_fabs(K0)/10;
                                if (vcl_fabs(k_step_large)<K_STEP)
                                    k_step_large = K_STEP;
                                if (vcl_fabs(k_step_large)>10)
                                    k_step_large = K_STEP;

                                l_step_large      = length/100;
                                l_step_small      = length/100;
                                break;
                               }

                        case 4:
                        case 5:
                              {
                                l_step_small = length/100;

                                k_step_large = vcl_fabs(K0)/10;
                                if (vcl_fabs(k_step_large)<K_STEP)
                                    k_step_large = K_STEP;
                                if (vcl_fabs(k_step_large)>10)
                                    k_step_large = K_STEP;

                                k_step_small = K_STEP;
                                break;
                               }
                        case 6:
                        case 7:
                              {

                                l_step_large      = length/100;

                                k_step_large = vcl_fabs(K0)/10;
                                if (vcl_fabs(k_step_large)<K_STEP)
                                    k_step_large = K_STEP;

                                if (vcl_fabs(k_step_large)>10)
                                    k_step_large = K_STEP;

                                k_step_small = K_STEP;
                                break;
                               }
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                              {
                                k_step_small      = K_STEP;
                                l_step_small      = length/100;
                                break;
                               }
                        case 12:
                        case 13:
                        case 14:
                        case 15:
                              {
                                k_step_large = vcl_fabs(K0)/10;
                                if (vcl_fabs(k_step_large)<K_STEP)
                                    k_step_large = K_STEP;

                                if (vcl_fabs(k_step_large)>10)
                                    k_step_large = K_STEP;
                                l_step_large      = length/100;
                               }
                        default:break;
                       }
                   }


               }
           }
        ret++;
       }
    return ret;
   }



/* auxiliary functions */
inline double EulerSpiral::minimum(const vcl_vector<double> &array,int& index) 
  { 
    vcl_vector<double>::const_iterator iter = vcl_min_element(array.begin(), array.end());

    for (unsigned int i=0; i<array.size(); i++)
      { 
        if(array[i]==*(iter))
          { 
            index=i; 
            return array[i];
           }
       }
    return 0;
   } 

/*------------ Interface Functions ----------------*/


int EulerSpiral::getDoubleParams(vcl_vector <EulerSpiralParams> &params)
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
        params = _es_params;
       }
    return 0;
   }

