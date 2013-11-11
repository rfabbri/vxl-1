#include "algo_biarc.h"


//
//BiArcParams
//
BiArcParams::BiArcParams()
  {
    K1            = 0;
    k1Sign        = 0;
    Length1       = 0;

    K2            = 0;
    k2Sign        = 0;
    Length2       = 0;

    circle1Dir    = 0;
    circle2Dir    = 0;

    circle1Radius = 0;
    circle2Radius = 0;

    angle_diff    = 0;

    start_angle   = 0;
    end_angle     = 0;
    join_angle    = 0;

    _firstTurningAngle  = 10*M_PI;    // Do NOT change. (Look at getFirstTurningAngle())
    _secondTurningAngle = 10*M_PI;
   };

BiArcParams::~BiArcParams(){};

BiArcParams::BiArcParams( const BiArcParams &rhs)
  {
    angle_diff        = rhs.angle_diff;

    start_pt          = rhs.start_pt;
    end_pt            = rhs.end_pt;

    start_angle       = rhs.start_angle;
    end_angle         = rhs.end_angle;
    join_angle        = rhs.join_angle;

    K1                = rhs.K1;
    k1Sign            = rhs.k1Sign;
    Length1           = rhs.Length1;

    K2                = rhs.K2;
    k2Sign            = rhs.k2Sign;
    Length2           = rhs.Length2;

    circle1Center     = rhs.circle1Center;
    circle2Center     = rhs.circle2Center;

    circle1Start      = rhs.circle1Start;
    circle2Start      = rhs.circle2Start;

    circle1End        = rhs.circle1End;
    circle2End        = rhs.circle2End;

    circle1Dir        = rhs.circle1Dir;
    circle2Dir        = rhs.circle2Dir;

    circle1Radius     = rhs.circle1Radius;
    circle2Radius     = rhs.circle2Radius;
   };

BiArcParams& BiArcParams::operator=(const BiArcParams &rhs)
  {
    if (this!=&rhs)
      {
        angle_diff        = rhs.angle_diff;

        start_pt          = rhs.start_pt;
        end_pt            = rhs.end_pt;

        start_angle       = rhs.start_angle;
        end_angle         = rhs.end_angle;
        join_angle        = rhs.join_angle;

        K1                = rhs.K1;
        Length1           = rhs.Length1;
        k1Sign            = rhs.k1Sign;

        K2                = rhs.K2;
        k2Sign            = rhs.k2Sign;
        Length2           = rhs.Length2;

        circle1Center     = rhs.circle1Center;
        circle2Center     = rhs.circle2Center;

        circle1Start      = rhs.circle1Start;
        circle2Start      = rhs.circle2Start;

        circle1End        = rhs.circle1End;
        circle2End        = rhs.circle2End;

        circle1Dir        = rhs.circle1Dir;
        circle2Dir        = rhs.circle2Dir;

        circle1Radius     = rhs.circle1Radius;
        circle2Radius     = rhs.circle2Radius;
       }
    return *this;
   }

double BiArcParams::getFirstTurningAngle()
  {
    if (_firstTurningAngle >=5*M_PI)
      {
        computeTurningAngle();
       }
    return _firstTurningAngle;
   }

double BiArcParams::getSecondTurningAngle()
  {
    if (_secondTurningAngle >=5*M_PI)
      {
        computeTurningAngle();
       }
    return _secondTurningAngle;
   }


double BiArcParams::compute_angle_diff()
  {
    if ((k1Sign!=0)&&(k2Sign!=0))
      {
        angle_diff=_computeFirstTurningAngle()+_computeSecondTurningAngle();
        return angle_diff;
       }
    else
      {
        if ((k1Sign==0)&&(k2Sign==0))
          {
            if ((Length1<INF)&&(Length2<INF))
              {
                angle_diff          = 0;
                _firstTurningAngle  = 0;
                _secondTurningAngle = 0;
                return angle_diff;
               }
            else
              {
                //
                //Dummy Value. Just make sure it is > 2*M_PI;
                //Not used anywhere. 
                //
                angle_diff          = 4*M_PI;
                _firstTurningAngle  = 2*M_PI;
                _secondTurningAngle = 2*M_PI;
                return angle_diff;
               } 
           }

        if (k1Sign==0)
          {
            double temp =0;
            temp = _computeSecondTurningAngle();
            if (Length1<INF)
              {
                _firstTurningAngle = 0;
                angle_diff         = temp;
                return angle_diff;
               }

            if (temp<0)
              {
                angle_diff          = 2*M_PI+temp;
                _firstTurningAngle  = 2*M_PI; 
               }
            else
              {
                angle_diff          = temp -2*M_PI;
                _firstTurningAngle  = -2*M_PI; 
               }

            return angle_diff;
           }
        else
          {
            double temp =0;
            temp = _computeFirstTurningAngle();

            if (Length2<INF)
              {
                _secondTurningAngle = 0;
                angle_diff          = temp;
                return angle_diff;
               }
            if (temp<0)
              {
                angle_diff          = 2*M_PI+temp;
                _secondTurningAngle  = 2*M_PI; 
               }
            else
              {
                angle_diff    = temp -2*M_PI;
                _secondTurningAngle  = -2*M_PI; 
               }

            return angle_diff;

           }

       }
    return angle_diff;
   }

double BiArcParams::computeTurningAngle()
  {
    return compute_angle_diff();
   }

int BiArcParams::scale (double factor)
  {
    K1      /=factor;
    Length1 *=factor;

    K2      /=factor;
    Length2 *=factor;

    return 0;
   }


double BiArcParams::_computeFirstTurningAngle() 
  {
    _firstTurningAngle = join_angle-start_angle;
    if (k1Sign>0)
      {
        while(_firstTurningAngle<=0)
          {
            _firstTurningAngle +=2*M_PI;
           }
       }
    if (k1Sign<0)
      {
        while(_firstTurningAngle>=0)
          {
            _firstTurningAngle -=2*M_PI;
           }

       }

    if (k1Sign==0)
      {
        if (Length1<INF)
          {
            _firstTurningAngle = 0;
            return _firstTurningAngle;
           }
        else
          {
            //
            //In this case the value of turning angle depends on the 
            //turning angle of the other arc. Hence this cannot be computed here.
            //
            vcl_cout<<" <_computeFirstTurningAngle> Error : Do not use _computeFirstTurningAngle to compute "
                    "the turning angle. Use computeTurningAngle() and getFirstTurningAngle instead "<<vcl_endl;

           }
       } 
    return _firstTurningAngle;
   }

double BiArcParams::_computeSecondTurningAngle()
  {
    _secondTurningAngle =end_angle-join_angle;

    if (k2Sign>0)
      {
        while(_secondTurningAngle<=0)
          {
            _secondTurningAngle +=2*M_PI;
           }
       }

    if (k2Sign<0)
      {
        while(_secondTurningAngle>=0)
          {
            _secondTurningAngle -=2*M_PI;
           }
       }

    if (k2Sign==0)
      {
        if (Length2<INF)
          {
            _secondTurningAngle = 0;
            return _secondTurningAngle;
           }
        else
          {
            //
            //In this case the value of turning angle depends on the 
            //turning angle of the other arc. Hence this cannot be computed here.
            //
            vcl_cout<<" <_computeSecondTurningAngle> Error : Do not use _computeSecondTurningAngle to compute "
                    "the turning angle. Use computeTurningAngle() and getSecondTurningAngle() instead "<<vcl_endl;
           }
       } 

    return _secondTurningAngle;
   }


#define             INF_RADIUS              1e5
int BiArcParams::computeBiArcInfo(void)
  {

    double actual_K1=0, actual_K2=0, actual_L1=0, actual_L2=0; 

    double dist_bet_pts=0;

    //dist_bet_pts = euc_distance(start_pt, end_pt);
    dist_bet_pts = 1;

    actual_K1 =K1/dist_bet_pts;
    actual_K2 =K2/dist_bet_pts;
    actual_L1 =Length1*dist_bet_pts;
    actual_L2 =Length2*dist_bet_pts;

    
    if (fabs(actual_K1)>EPSILON)
        circle1Radius = 1/actual_K1;
    else
      {
        if (k1Sign<0)
            circle1Radius = -1*INF_RADIUS;
        else
            circle1Radius = INF_RADIUS;
       }

    if (fabs(actual_K2)>=EPSILON)
        circle2Radius = 1/actual_K2;
    else
      {
        if (k2Sign<0)
            circle2Radius = -1*INF_RADIUS;
        else
            circle2Radius = INF_RADIUS;
       }


    circle1Center = getCenter(start_pt, start_angle, circle1Radius);
    circle2Center = getCenter(end_pt, end_angle, circle2Radius);


    circle1Start = start_pt;
    circle2End   = end_pt;


    compute_angle_diff();
    double circle1_span, circle2_span;

    circle1_span=(_firstTurningAngle);
    circle2_span=(_secondTurningAngle);
    
    double start=0, end=0;

    if (fabs(actual_K1)>EPSILON)
      {
        start = atan2((double)(circle1Start.getY()-circle1Center.getY()), (double)(circle1Start.getX()-circle1Center.getX()));
        if (start<0)
            start +=2*M_PI;
        end = start+circle1_span;

        end = fmod(end, 2*M_PI);
        if (end<0)
            end +=2*M_PI;

        circle1End.setX(circle1Center.getX() +fabs(circle1Radius)*cos(end));
        circle1End.setY(circle1Center.getY() +fabs(circle1Radius)*sin(end));
        circle2Start    = circle1End;
       }
    else
      {
        end = atan2((double)(circle2End.getY()-circle2Center.getY()), (double)(circle2End.getX()-circle2Center.getX()));
        if (end<0)
            end +=2*M_PI;
        start = end-circle2_span;

        start = fmod(start, 2*M_PI);
        if (start<0)
            start +=2*M_PI;

        circle2Start.setX(circle2Center.getX() +fabs(circle2Radius)*cos(start));
        circle2Start.setY(circle2Center.getY() +fabs(circle2Radius)*sin(start));
        circle1End      = circle2Start;
       }

    return 0; 
   }

Point2D<double>  BiArcParams::getCenter(Point2D<double>  start, double angle, double radius)
  {

    Point2D<double>  center; 
    center.setX(start.getX()-radius*sin(angle)); 
    center.setY(start.getY()+radius*cos(angle));

    return center;
   }



//
// Bi Arc
//


const vcl_vector<BiArcParams>& BiArc::get_bi_arc_params()
  {
    return bi_arc_params;
   }

int BiArc::set_start_params(Point2D<double> start_pt, double start_angle)
  {
    _start_pt    = start_pt;
    _start_angle = start_angle;

    _start_angle = fmod(_start_angle,2*M_PI);

    if (_start_angle<0)
        _start_angle +=2*M_PI;

    return 0;
   }

int BiArc::set_end_params(Point2D<double> end_pt, double end_angle)
  {
    _end_pt    = end_pt;
    _end_angle = end_angle;

    _end_angle = fmod(_end_angle, 2*M_PI);
    if (_end_angle<0)
        _end_angle +=2*M_PI;

    return 0;
   }

int BiArc::getParams(vcl_vector<BiArcParams> &params)
  {
    if (bi_arc_params.size()==0)
        compute_biarc_params();
    params =bi_arc_params;
    return 0;
   }

BiArcParams BiArc::getParams(void)
  {
    if (bi_arc_params.size()==0)
        compute_biarc_params();
    if (bi_arc_params.size()>0)
        return bi_arc_params[0];

    return BiArcParams();
   }

vcl_ostream& operator<<(vcl_ostream &os, BiArcParams &params)
  {
    os<<" Start Point   ="<<params.start_pt<<" End Point   ="<<params.end_pt<<vcl_endl;
    os<<" Start Tangent ="<<params.start_angle*(180/M_PI)<<" End Tangent ="<<params.end_angle*(180/M_PI)<<" Join Tangent ="<<params.join_angle*(180/M_PI)<<vcl_endl;
    os<<" K1      ="<<params.K1<<"    K2     ="<<params.K2<<vcl_endl;
    os<<" K1 Sign ="<<params.k1Sign<<" K2 Sign = "<<params.k2Sign<<vcl_endl;
    os<<" Length1 ="<<params.Length1<<"    Length2 ="<<params.Length2<<vcl_endl;

    os<<"  Net   Turning Angle ="<<params.angle_diff*(180/M_PI)<<vcl_endl;
    os<<"  First Turning Angle ="<<params.getFirstTurningAngle()*(180/M_PI)<<" Second Turning Angle = "<<params.getSecondTurningAngle()*(180/M_PI)<<vcl_endl;

    os<<" Circle 1 :"<<vcl_endl;
    os<<"     Start = "<<params.circle1Start<<" End = "<<params.circle1End<<" Radius = "<<params.circle1Radius<<" Center = "<<params.circle1Center<<vcl_endl;
    
    os<<" Circle 2 :"<<vcl_endl;
    os<<"     Start = "<<params.circle2Start<<" End = "<<params.circle2End<<" Radius = "<<params.circle2Radius<<" Center = "<<params.circle2Center<<vcl_endl;
    
    return os;
   }

/* ---------------- Bi Arc Functions --------------------- */

double BiArc::compute_join_theta(double k1, double k2)
  {
    /*
     * NOTE: Angles alpha and beta MUST be in RADIANS 
     * */

    double denomenator=0,sin_numerator=0,cos_numerator=0,sin_theta1=0,cos_theta1=0,theta_temp=0;

    denomenator=k2-k1;
    sin_numerator=k1*k2*(_end_pt.getX()-_start_pt.getX())+k2*sin(_start_angle)-k1*sin(_end_angle);

    cos_numerator=-k1*k2*(_end_pt.getY()-_start_pt.getY())+k2*cos(_start_angle)-k1*cos(_end_angle);

    if (denomenator!=0)
      {
        sin_theta1=sin_numerator/denomenator;
        cos_theta1=cos_numerator/denomenator;
        theta_temp=atan2(sin_theta1,cos_theta1);
        if(theta_temp<0)  theta_temp+=2*M_PI;  
       }
    else
      {
        //TODO :
        vcl_cout<<" Error :<compute_join_theta> Division by 0"<<vcl_endl;
       }

    return theta_temp;
   }

double BiArc::compute_arclength(double theta0,double theta1,double k)
  {
    /* 
     * NOTE : theta0 and theta1 MUST be in RADIANS
     * */

    double dif=0,L=0;

    theta0 = fmod(theta0, 2*M_PI);
    theta1 = fmod(theta1, 2*M_PI);

    if (theta0<0)
        theta0 +=2*M_PI;
    if (theta1<0)
        theta1 +=2*M_PI;



    dif=theta1-theta0;
    if((k<0)&&(dif>0)) 
      {
        dif-=2*M_PI;
       }
    else
      {
        if((k>0)&&(dif<0))
            dif+=2*M_PI;
       }

    if (k!=0)
      {
        L=dif/k;
       }
    else
      {
        //TODO:

        vcl_cout<<" Error :<compute_arclength> Division by 0"<<vcl_endl;
       }

    return L;
   }


/* 
 * To compute initial estimates for L and K (req'd by iterative solution)
 * */
int BiArc::compute_biarc_params(void)
  {
    /*
     * NOTE: _start_angle and theta1 MUST be in RADIANS.
     * */

    double distance,psi,alpha,beta,join_theta12, join_theta34;


    double mean_angle, dif_angle;
    double L1=-10;
    double L2=-10;
    double k1,k2,k3,k4,L3,L4;
    double estimate_K, estimate_L;
    double theta2, theta0;

    bi_arc_params.clear();;

    /*
     * Check for degenerate conditions.
     * */

    distance=euc_distance(_start_pt, _end_pt);
    if (distance<EPSILON)
      {

        BiArcParams params;

        params.start_pt     = _start_pt;
        params.end_pt       = _end_pt;
        params.start_angle  = _start_angle;
        params.end_angle    = _end_angle;

        params.K1      = K_LARGE;
        params.k1Sign  = 0;
        params.Length1 = 0;
        params.Length2 = 0;
        params.K2      = K_LARGE;
        params.k2Sign  = 0;
        

        bi_arc_params.push_back(params);
        return 0;
       }


    /* 
     * psi,alpha,beta=angles derived from triangles b|t x0,y0 & x1,y1 constrained [0,pi] 
     * */

    theta0 = _start_angle;
    theta2 = _end_angle;

    psi=atan2(_end_pt.getY()-_start_pt.getY(),_end_pt.getX()-_start_pt.getX());
    if(psi<0) psi+=2*M_PI;

    alpha=_start_angle-psi;
    if(alpha<=-M_PI) alpha+=2*M_PI;
    if(alpha>M_PI)   alpha-=2*M_PI;

    beta=_end_angle-psi;
    if(beta<=-M_PI) beta+=2*M_PI;
    if(beta>M_PI)   beta-=2*M_PI;


    mean_angle=(alpha+beta)/2.;


    if(fabs(mean_angle)<EPSILON)                  /* one arc       */
      {
        if(fabs(alpha)<EPSILON)                   /* straight line */
          {
            BiArcParams params;


            params.start_pt     = _start_pt;
            params.end_pt       = _end_pt;
            params.start_angle  = _start_angle;
            params.end_angle    = _end_angle;

            params.K1      = 0;
            params.K2      = 0;
            params.k1Sign  = 0;
            params.k2Sign  = 0;

            params.Length1 = distance/2;
            params.Length2 = distance/2;


            bi_arc_params.push_back(params);
            return 0;
           }
        else
          {
            dif_angle=beta-alpha;
            if(alpha>0)
              {
                estimate_K=-fabs(2/distance *sin((beta-alpha)/2));
               }
            else
              {
                estimate_K=fabs(2/distance *sin((beta-alpha)/2));
               }

            if((estimate_K>0) && (dif_angle>0))
                estimate_L=dif_angle/(estimate_K);

            else if ((estimate_K>0)&&(dif_angle<0))
                estimate_L=(dif_angle+2*M_PI)/(estimate_K);

            else if((estimate_K<0)&&(dif_angle>0))
                estimate_L=(dif_angle-2*M_PI)/(estimate_K);
            else
                estimate_L=(dif_angle)/(estimate_K);

            BiArcParams params;

            params.start_pt     = _start_pt;
            params.end_pt       = _end_pt;
            params.start_angle  = _start_angle;
            params.end_angle    = _end_angle;
            
            if (estimate_K>0)
                params.k1Sign=params.k2Sign=1;

            if (estimate_K<0)
                params.k1Sign=params.k2Sign=-1;

            params.K1      = estimate_K;
            params.K2      = estimate_K;
            params.Length1 = estimate_L/2;
            params.Length2 = estimate_L/2;

            bi_arc_params.push_back(params);
            return 0;
           }
       }

    /*
     * in all cases but the straight line: obtain initial estimate for L
     * */
    else 
      {
        int k1_sign=0, k2_sign =0, k3_sign=0, k4_sign=0;

        k1      = -(4.0*sin(((3.0*alpha+beta)/4))*cos((beta-alpha)/4))/distance;
        k1_sign = -1*getSinSign(((3.0*alpha+beta)/4))*getCosSign((beta-alpha)/4);
        
        k2      = (4.0*sin(((alpha+3.0*beta)/4))*cos((beta-alpha)/4))/distance;
        k2_sign = getSinSign(((alpha+3.0*beta)/4))*getCosSign((beta-alpha)/4);
        
        join_theta12=compute_join_theta(k1, k2);

        if(fabs(k1)<(EPSILON))
          {
            if (fabs(sin((theta0 - theta2)/2))<MIN_DENOMINATOR)
              {
                L1 = INF;
               }
            else
              {
                L1 = distance*(fabs(sin (((theta2 + theta0)/2)-psi)/sin((theta0 - theta2)/2)));
               }
           }
        else 
          {
            L1 = compute_arclength(_start_angle,join_theta12,k1);
           }

        if(fabs(k2)<EPSILON)
          {
            if (fabs(sin((theta0 - theta2)/2))<MIN_DENOMINATOR)
              {
                L2 = INF;
               }
            else
              {
                L2=distance*(fabs(sin(((theta2 + theta0)/2)-psi) / sin((theta0 - theta2)/2)));
               }
           }
        else
          {
            L2 = compute_arclength(join_theta12,_end_angle,k2);
           }


        k3      = (4.0*cos(((3.0*alpha+beta)/4))*sin((beta-alpha)/4))/distance;
        k3_sign = getCosSign((3.0*alpha+beta)/4)*getSinSign((beta-alpha)/4); 

        k4      = (4.0*cos(((alpha+3.0*beta)/4))*sin((beta-alpha)/4))/distance;
        k4_sign = getCosSign(((alpha+3.0*beta)/4))*getSinSign((beta-alpha)/4);

        if((fabs(k3)<EPSILON)&&(fabs(k4)<EPSILON))
          {
            L3=INF;
            L4=INF;
           }
        else
          {
            join_theta34=compute_join_theta(k3, k4);
            if(fabs(k3)<EPSILON)
              {
                if (fabs(sin((theta2-theta0)/2))<MIN_DENOMINATOR)
                  {
                    L3 = INF;
                   }
                else
                  {
                    L3=distance*fabs(sin(((theta2 + theta0)/2)-psi) / sin((theta2-theta0)/2));
                   }
               }
            else
              {
                L3=compute_arclength(_start_angle,join_theta34,k3);
               }

            if(fabs(k4)<EPSILON)
              {
                if (fabs(sin((theta2 - theta0)/2))<MIN_DENOMINATOR)
                  {
                    L4 = INF;
                   }
                else
                  {
                    L4=distance*fabs((sin(((theta0 + theta2)/2) -psi)/sin((theta2 - theta0)/2)));
                   }
               }
            else
              {
                L4=compute_arclength(join_theta34,_end_angle,k4);
               }

           }


        if ((L1+L2)<=(L3+L4))
          {
            BiArcParams params;

            params.start_pt     = _start_pt;
            params.end_pt       = _end_pt;
            params.start_angle  = _start_angle;
            params.end_angle    = _end_angle;


            params.K1      = k1;
            params.Length1 = L1;
            params.Length2 = L2;
            params.K2      = k2;
            params.join_angle = join_theta12;
            params.k1Sign  = k1_sign;
            params.k2Sign  = k2_sign;

            bi_arc_params.push_back(params);

            params.K1      = k3;
            params.Length1 = L3;
            params.K2      = k4;
            params.Length2 = L4;
            params.join_angle   = join_theta34;
            params.k1Sign  = k3_sign;
            params.k2Sign  = k4_sign;


            bi_arc_params.push_back(params);
           }
        else
          {
            BiArcParams params;


            params.start_pt     = _start_pt;
            params.end_pt       = _end_pt;
            params.start_angle  = _start_angle;
            params.end_angle    = _end_angle;


            params.K1      = k3;
            params.Length1 = L3;
            params.K2      = k4;
            params.Length2 = L4;
            params.join_angle   = join_theta34;
            params.k1Sign  = k3_sign;
            params.k2Sign  = k4_sign;

            bi_arc_params.push_back(params);


            params.K1      = k1;
            params.Length1 = L1;
            params.Length2 = L2;
            params.K2      = k2;
            params.join_angle = join_theta12;
            params.k1Sign  = k1_sign;
            params.k2Sign  = k2_sign;

            bi_arc_params.push_back(params);
           }

        return 0;
       }
    return 0;
   }


int getSinSign(double angle)
  {
    angle = fmod(angle, 2*M_PI);

    if (angle<0)
        angle +=2*M_PI;
    if(angle==2*M_PI)
        angle -=2*M_PI;
    
    if (((angle<=(0+EPSILON))&&(angle>=(0-EPSILON)))||(((angle<=(M_PI+EPSILON))&&((angle>=(M_PI-EPSILON))))))
        return 0;

    if ((angle>0)&&(angle<M_PI))
        return 1;
   
    if ((angle>M_PI)&&(angle<2*M_PI))
        return -1;
    vcl_cout<<" <getSinSign> : Angle out of range. Angle ="<<angle<<" (rads) =="<<angle*(180/M_PI)<<" (deg)"<<vcl_endl;
    return 0;
   }

int getCosSign(double angle)
  {
    angle = fmod(angle, 2*M_PI);

    if (angle<0)
        angle +=2*M_PI;
    
    if(angle==2*M_PI)
        angle -=2*M_PI;
    
    if (((angle<=(M_PI*0.5+EPSILON))&&(angle>=(M_PI*0.5-EPSILON)))||(((angle<=(M_PI*1.5+EPSILON))&&((angle>=(M_PI*1.5-EPSILON))))))
        return 0;
    


    if ((angle>=0)&&(angle<M_PI*0.5))
        return 1;
  
    if ((angle>M_PI*0.5)&&(angle<M_PI*1.5))
        return -1;
  
    if ((angle>M_PI*1.5)&&(angle<2*M_PI))
        return 1;
    
    vcl_cout<<" <getCosSign> : Angle out of range. Angle ="<<angle<<" (rads) =="<<angle*(180/M_PI)<<" (deg)"<<vcl_endl;
    return 0;
   }


