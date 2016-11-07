#include "ishock-common.h"
#include "biarc_shock.h"
//#include "euler.h"

#include <vcl_cmath.h>

/* ---------------- Bi Arc Functions --------------------- */

double BiArcShock::compute_join_theta(double k1, double k2)
  {
    //
    //NOTE: Angles alpha and beta MUST be in RADIANS 
    //

    double denomenator=0,sin_numerator=0,cos_numerator=0,sin_theta1=0,cos_theta1=0,theta_temp=0;

    denomenator=k2-k1;
    sin_numerator=k1*k2*(_end_pt.x-_start_pt.x)+k2*vcl_sin(_start_angle)-k1*vcl_sin(_end_angle);

    cos_numerator=-k1*k2*(_end_pt.y-_start_pt.y)+k2*vcl_cos(_start_angle)-k1*vcl_cos(_end_angle);

    if (denomenator!=0)
      {
        sin_theta1=sin_numerator/denomenator;
        cos_theta1=cos_numerator/denomenator;
        theta_temp=vcl_atan2(sin_theta1,cos_theta1);
        if(theta_temp<0)  theta_temp+=2*M_PI;  
       }
    else
      {
        //TODO :
        //cout<<" Error :<compute_join_theta> Division by 0"<<endl;
       }

    return theta_temp;
   }


double BiArcShock::compute_arclength(double theta0,double theta1,double k)
  {
    // 
    // NOTE : theta0 and theta1 MUST be in RADIANS
    //

    double dif=0,L=0;

    theta0 =  vcl_fmod (theta0, 2*M_PI);
    theta1 =  vcl_fmod (theta1, 2*M_PI);

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
        //cout<<" Error :<compute_arclength> Division by 0"<<endl;
       }

    return L;
   }


// 
// To compute initial estimates for L and K (req'd by iterative solution)
//

int BiArcShock::compute_biarc_params(void)
  {
    //
    // NOTE: _start_angle and theta1 MUST be in RADIANS.
    //

    double distance,psi,alpha,beta,join_theta;


    double mean_angle, dif_angle;
    double L1=-10;
    double L2=-10;
    double k1,k2,k3,k4,L3,L4;
    double estimate_K, estimate_L;
    double theta2, theta0;

    

    //bi_arc_params.clear();;
    
    //
    // Check for degenerate conditions.
    //

    if (_start_pt==_end_pt)
      {

        BiArcShockParams params;

        params.start_pt     = _start_pt;
        params.end_pt       = _end_pt;
        params.start_angle  = _start_angle;
        params.end_angle    = _end_angle;

        params.K1      = K_LARGE;
        params.Length1 = 0;
        params.Length2 = 0;
        params.K2      = K_LARGE;

      bi_arc_params = params;
        //bi_arc_params.push_back(params);
        return 0;
       }


    // 
    // psi,alpha,beta=angles derived from triangles b|t x0,y0 & x1,y1 constrained [0,pi] 
    //

    theta0 = _start_angle;
    theta2 = _end_angle;
    
    distance= _distPointPoint(_start_pt, _end_pt);
    psi=vcl_atan2(_end_pt.y-_start_pt.y,_end_pt.x-_start_pt.x);
    if(psi<0) psi+=2*M_PI;

    alpha=_start_angle-psi;
    if(alpha<=-M_PI) alpha+=2*M_PI;
    if(alpha>M_PI)   alpha-=2*M_PI;

    beta=_end_angle-psi;
    if(beta<=-M_PI) beta+=2*M_PI;
    if(beta>M_PI)   beta-=2*M_PI;


    mean_angle=(alpha+beta)/2.;


    if(vcl_fabs(mean_angle)<EPSILON)                  // one arc
      {
        if(vcl_fabs(alpha)<EPSILON)                   // straight line
          {
            BiArcShockParams params;


            params.start_pt     = _start_pt;
            params.end_pt       = _end_pt;
            params.start_angle  = _start_angle;
            params.end_angle    = _end_angle;
            
            params.K1      = 0;
            params.K2      = 0;
            params.Length1 = distance/2;
            params.Length2 = distance/2;

        bi_arc_params = params;

            //bi_arc_params.push_back(params);
            return 0;
           }
        else
          {
            dif_angle=beta-alpha;
            if(alpha>0)
              {
                estimate_K=-vcl_fabs(2/distance *vcl_sin((beta-alpha)/2));
               }
            else
              {
                estimate_K=vcl_fabs(2/distance *vcl_sin((beta-alpha)/2));
               }

            if((estimate_K>0) && (dif_angle>0))
                estimate_L=dif_angle/(estimate_K);

            else if ((estimate_K>0)&&(dif_angle<0))
                estimate_L=(dif_angle+2*M_PI)/(estimate_K);

            else if((estimate_K<0)&&(dif_angle>0))
                estimate_L=(dif_angle-2*M_PI)/(estimate_K);
            else
                estimate_L=(dif_angle)/(estimate_K);

            BiArcShockParams params;

            params.start_pt     = _start_pt;
            params.end_pt       = _end_pt;
            params.start_angle  = _start_angle;
            params.end_angle    = _end_angle;

            
            params.K1      = estimate_K;
            params.K2      = estimate_K;
            params.Length1 = estimate_L/2;
            params.Length2 = estimate_L/2;

        bi_arc_params = params;

            //bi_arc_params.push_back(params);
            return 0;
           }
       }

    //
    // in all cases but the straight line: obtain initial estimate for L
    //
    else 
      {


        k1=-(4.0/distance)*vcl_sin(((3.0*alpha+beta)/4))*vcl_cos((beta-alpha)/4);
        k2= (4.0/distance)*vcl_sin(((alpha+3.0*beta)/4))*vcl_cos((beta-alpha)/4);


        join_theta=compute_join_theta(k1, k2);

        if(vcl_fabs(k1)<(EPSILON))
          {
            if (vcl_fabs(vcl_sin((theta0 - theta2)/2))<MIN_DENOMINATOR)
              {
                L4 = INF;
               }
            else
              {
                L1 = distance*(vcl_fabs(vcl_sin (((theta2 + theta0)/2)-psi)/vcl_sin((theta0 - theta2)/2)));
               }
           }
        else 
          {
            L1 = compute_arclength(_start_angle,join_theta,k1);
           }

        if(vcl_fabs(k2)<EPSILON)
          {
            if (vcl_fabs(vcl_sin((theta0 - theta2)/2))<MIN_DENOMINATOR)
              {
                L2 = INF;
               }
            else
              {
                L2=distance*(vcl_fabs(vcl_sin(((theta2 + theta0)/2)-psi) / vcl_sin((theta0 - theta2)/2)));
               }
           }
        else
          {
            L2 = compute_arclength(join_theta,_end_angle,k2);
           }


        k3 = (4.0/distance)*vcl_cos(((3.0*alpha+beta)/4))*vcl_sin((beta-alpha)/4);
        k4 = (4.0/distance)*vcl_cos(((alpha+3.0*beta)/4))*vcl_sin((beta-alpha)/4);

        if((vcl_fabs(k3)<EPSILON)&&(vcl_fabs(k4)<EPSILON))
          {
            L3=INF_PI;
            L4=INF_PI;
           }
        else
          {
            join_theta=compute_join_theta(k3, k4);
            if(vcl_fabs(k3)<EPSILON)
              {
                if (vcl_fabs(vcl_sin((theta0-join_theta)/2))<MIN_DENOMINATOR)
                  {
                    L3 = INF;
                   }
                else
                  {
                    L3=distance*vcl_fabs(vcl_sin(((join_theta + theta0)/2)-psi) / vcl_sin((theta0-join_theta)/2));
                   }
               }
            else
              {
                L3=compute_arclength(_start_angle,join_theta,k3);
               }

            if(vcl_fabs(k4)<EPSILON)
              {
                if (vcl_fabs(vcl_sin((join_theta - theta2)/2))<MIN_DENOMINATOR)
                  {
                    L4 = INF;
                   }
                else
                  {
                    L4=distance*vcl_fabs((vcl_sin (((join_theta + theta2)/2) -psi)/vcl_sin((join_theta - theta2)/2)));
                   }
               }
            else
              {
                L4=compute_arclength(join_theta,_end_angle,k4);
               }

           }


        if ((L1+L2)<=(L3+L4))
          {
            BiArcShockParams params;

            params.start_pt     = _start_pt;
            params.end_pt       = _end_pt;
            params.start_angle  = _start_angle;
            params.end_angle    = _end_angle;
           
            
            params.K1      = k1;
            params.Length1 = L1;
            params.Length2 = L2;
            params.K2      = k2;

        bi_arc_params = params;
            
            //bi_arc_params.push_back(params);
          
            //params.K1      = k3;
            //params.Length1 = L3;
            //params.K2      = k4;
            //params.Length2 = L4;
           
           //bi_arc_params = params;

            //bi_arc_params.push_back(params);
           }
        else
          {
            BiArcShockParams params;


            params.start_pt     = _start_pt;
            params.end_pt       = _end_pt;
            params.start_angle  = _start_angle;
            params.end_angle    = _end_angle;

           
            params.K1      = k3;
            params.Length1 = L3;
            params.K2      = k4;
            params.Length2 = L4;
            
            //bi_arc_params.push_back(params);

           //bi_arc_params = params;

            //params.K1      = k1;
            //params.Length1 = L1;
            //params.Length2 = L2;
            //params.K2      = k2;
           
        //bi_arc_params = params;

            //bi_arc_params.push_back(params);
           }
            
            return 0;
       }
    return 0;
}

Point  get_center(Point  start, double angle, double radius)
{
    double r=radius;

    Point  center; 
    center.x =  start.x-r*vcl_sin(angle); 
    center.y =  start.y+r*vcl_cos(angle);

    return center;
}

//#define          CLOCKWISE  1
//#define          COUNTER_CLOCKWISE -1

void BiArcShock::compute_other_stuff(void)
{

    double K1=0, K2=0, L1=0, L2=0;
    double R1=0, R2=0; 
    Point center1, center2;
    Point circle1_start, circle1_end, circle2_start, circle2_end;
    int   circle1_dir, circle2_dir;

    K1  = bi_arc_params.K1;
    K2  = bi_arc_params.K2;
    L1  = bi_arc_params.Length1;
    L2  = bi_arc_params.Length2;

    //double dist_bet_pts = distPointPoint(_start_pt, _end_pt);

    //K1 /=dist_bet_pts;
    //K2 /=dist_bet_pts;
    //L1 *=dist_bet_pts;
    //L2 *=dist_bet_pts;

    
    if (vcl_fabs(K1)>=1.0/INF_RADIUS)
        R1 = 1/K1;
    else
      {
        if (K1<0)
            R1 = -1*INF_RADIUS;
        else
            R1 = INF_RADIUS;
       }

    if (vcl_fabs(K2)>=1.0/INF_RADIUS)
        R2 = 1/K2;
    else
      {
        if (K2<0)
            R2 = -1*INF_RADIUS;
        else
            R2 = INF_RADIUS;
       }


    center1 = get_center(_start_pt, _start_angle, R1);
    center2 = get_center(_end_pt, _end_angle, R2);


    circle1_start = _start_pt;
    circle2_end   = _end_pt;

    if (R1<0)
      {
        //Arc is clockwise. On screen it appears Counter Clockwise.
        circle1_dir=CLOCKWISE;
       }
    else
      {
        circle1_dir=COUNTER_CLOCKWISE;
       }


    if (R2<0)
      {
        circle2_dir = CLOCKWISE;
       }
    else
      {
        circle2_dir = COUNTER_CLOCKWISE;
       }

    
    double circle1_span, circle2_span;

    circle1_span=vcl_fabs(L1/R1);
    circle2_span=vcl_fabs(L2/R2);

    //if ((circle1_span>2*M_PI)||(circle2_span>2*M_PI))
    //    cout<<" Error : Span > 2*PI :"<<circle1_span<<", "<<circle2_span<<endl;


    circle1_span =  vcl_fmod (circle1_span, 2*M_PI);
    circle2_span =  vcl_fmod (circle2_span, 2*M_PI);
   
    if (circle1_span<0)
        circle1_span +=2*M_PI;
    
    double start_pos=0, end_pos=0;

    start_pos = vcl_atan2((double)(circle1_start.y-center1.y), (double)(circle1_start.x-center1.x));
    if (start_pos<0)
        start_pos +=2*M_PI;

    if (circle1_dir==CLOCKWISE)
      {
        end_pos = start_pos-circle1_span;
       }
    else
      {
        end_pos = start_pos+circle1_span;
       }

    end_pos =  vcl_fmod (end_pos, 2*M_PI);
    if (end_pos<0)
        end_pos +=2*M_PI;
   
    circle1_end.x = center1.x +vcl_fabs(R1)*vcl_cos(end_pos);
    circle1_end.y = center1.y +vcl_fabs(R1)*vcl_sin(end_pos);
    circle2_start = circle1_end;


    bi_arc_params.K1      = K1;
    bi_arc_params.K2      = K2;
    bi_arc_params.Length1 = L1;
    bi_arc_params.Length2 = L2;
   

    bi_arc_params.center1 = center1;
    bi_arc_params.center2 = center2;

    bi_arc_params.start1  = circle1_start;
    bi_arc_params.start2  = circle2_start;

    bi_arc_params.end1    = circle1_end;
    bi_arc_params.end2    = circle2_end;


    bi_arc_params.dir1    = circle1_dir;
    bi_arc_params.dir2    = circle2_dir;
   
    bi_arc_params.radius1 = R1;
    bi_arc_params.radius2 = R2;

}
