#ifndef GEOMETRY_FUNCTIONS_H
#define  GEOMETRY_FUNCTIONS_H

#include <vcl_vector.h>
///#include <vcl_algorithm.h>

#include <base_points.h>

///#include <vcl_cmath.h>
#ifdef M_PI
#undef M_PI
#endif
#ifndef M_PI
#define M_PI    3.1415926535897932384626433832795
#endif

#define  TWICE_PI       2*M_PI



inline Point2D<double> rotateCCWPoint2D (Point2D<double>pt, double angle)
{
   Point2D<double> rpt;

   rpt.setX (pt.getX()*cos(angle) - pt.getY()*sin(angle));
   rpt.setY (pt.getX()*sin(angle) + pt.getY()*cos(angle));

   return rpt;
}

inline Point2D<double> rotateCCWPoint2D (double x, double y, double angle)
{
   Point2D<double> rpt;

   rpt.setX (x*cos(angle) - y*sin(angle));
   rpt.setY (x*sin(angle) + y*cos(angle));

   return rpt;
}

inline double GetVector(Point2D<double> s_pt, Point2D<double> e_pt)
{
   return atan2( e_pt.getY() - s_pt.getY(), e_pt.getX() - s_pt.getX());
}

inline Point2D<double> vectorPoint (Point2D<double> pt, double vector, double length)
{
  Point2D<double> dpt;
   dpt.setX (pt.getX() + length*cos(vector));
   dpt.setY (pt.getY() + length*sin(vector));
  return dpt;
}

template <class point_type1, class point_type2, class point_type3, class point_type4>
int intersect(const Point2D<point_type1> &line0_start, const Point2D<point_type2> &line0_end, const Point2D<point_type3> &line1_start, const  Point2D<point_type4> &line1_end);

template <class point_type1, class point_type2, class point_type3>
int area_sign(const Point2D<point_type1> &pt0, const Point2D<point_type2> &pt1, const Point2D<point_type3> &pt2);

template <class point_type1, class point_type2>
double euc_distance(Point2D<point_type1> pt1, Point2D<point_type2> pt2);


    template <class point_type1, class point_type2, class point_type3, class point_type4>
int intersect(const Point2D<point_type1> &line0_start, const Point2D<point_type2> &line0_end, const Point2D<point_type3> &line1_start, const  Point2D<point_type4> &line1_end)
  {

    int sign1, sign2, sign3, sign4;

    sign1 = area_sign(line0_start, line0_end, line1_start);
    sign2 = area_sign(line0_start, line0_end, line1_end);

    sign3 = area_sign(line1_start, line1_end, line0_start);
    sign4 = area_sign(line1_start, line1_end, line0_end);


    if (line0_start == line0_end)
      {
        if (sign3==0)
            return 1;
        else
            return 0;
       }

    if (line1_start==line1_end)
      {
        if (sign1==0)
            return 1;
        else
            return 0;
       }

    if ((sign1*sign2*sign3*sign4)==0)
      {
        return 0;

       }

    if ((sign1*sign2+sign3*sign4)==-2)
      {
        return 1;
       }
    else
      {
        return 0;
       }

   }

    template <class point_type1, class point_type2, class point_type3>
int area_sign(const Point2D<point_type1> &pt0, const Point2D<point_type2> &pt1, const Point2D<point_type3> &pt2)
  {
    double area=0;
    area = ((pt1.getX()-pt0.getX())*(pt2.getY()-pt0.getY())-(pt2.getX()-pt0.getX())*(pt1.getY()-pt0.getY()));

    if (area>0)
      {
        return 1;
       }

    if (area<0)
      {
        return -1;
       }
    if (area==0)
      {
        return 0;
       }

    cout<<" Error in calculating area_sign. Area= "<<area<<endl;
    return 0;
   }

    template <class point_type1, class point_type2>
double euc_distance(Point2D<point_type1> pt1, Point2D<point_type2> pt2)
  {
    double x_dist, y_dist,dist=0;

    x_dist = (pt1.getX()-pt2.getX())*(pt1.getX()-pt2.getX());
    y_dist = (pt1.getY()-pt2.getY())*(pt1.getY()-pt2.getY());

    dist = sqrt(x_dist+y_dist);
    return dist;
   }

    template <class point_type1, class point_type2>
double EuclideanDistance(Point2D<point_type1> pt1, Point2D<point_type2> pt2)
  {
    return  sqrt((pt1.getX()-pt2.getX())*(pt1.getX()-pt2.getX())+(pt1.getY()-pt2.getY())*(pt1.getY()-pt2.getY()));
   }


    template <class point_type1, class point_type2>
double EuclideanDistanceSquare(Point2D<point_type1> pt1, Point2D<point_type2> pt2)
  {
    return ((pt1.getX()-pt2.getX())*(pt1.getX()-pt2.getX())+ (pt1.getY()-pt2.getY())*(pt1.getY()-pt2.getY()));
   }





/*
 *
 * Returns 0 if Point is NOT in polygon.
 * 1 if it is IN the polygon.
 *
 * */
//
//ORIGINAL Function
//    template <typename lineType, typename coord_type2>
//int PointInPolygon(const vcl_vector<lineType> &polygon , const Point2D<coord_type2> pt)
//  {
//    int c = 0;
//    for (unsigned int i = 0; i<polygon.size(); i++) 
//      {
//        if ((((polygon[i].start.getY()<=pt.getY()) && (pt.getY()<polygon[i].end.getY())) || ((polygon[i].end.getY()<=pt.getY()) && (pt.getY()<polygon[i].start.getY()))) && (pt.getX() < (polygon[i].end.getX() - polygon[i].start.getX()) * (pt.getY() - polygon[i].start.getY()) / (polygon[i].end.getY() - polygon[i].start.getY()) + polygon[i].start.getX()))
//          {
//            c = !c;
//           }
//       }
//    return c;
//   }


//    template <typename coord_type1, typename coord_type2>
//int PointInPolygon(const vcl_vector<Line2D<coord_type1> > &polygon , const Point2D<coord_type2> pt)
//  {
//    int c = 0;
//    for (int i = 0; i<polygon.size(); i++) 
//      {
//        if ((((polygon[i].start.getY()<=pt.getY()) && (pt.getY()<polygon[i].end.getY())) || ((polygon[i].end.getY()<=y) && (y<polygon[i].start.getY()))) && (pt.getX() < (polygon[i].end.getX() - polygon[i].start.getX()) * (pt.getY() - polygon[i].start.getY()) / (polygon[i].end.getY() - polygon[i].start.getY()) + polygon[i].start.getX()))
//          {
//            c = !c;
//           }
//       }
//    return c;
//   }
//


    template <typename coord_type1, typename coord_type2>
int PointInPolygon(const vcl_vector<coord_type1> &polygon , const Point2D<coord_type2> pt)
  {
    int c = 0;
    int size = (int)polygon.size();
    int next;
    for (int i = 0; i<size; i++) 
      {
        
        next = (i+1);
        if (i==(size-1))
            next = 0;
                
        if ((((polygon[i].getY()<=pt.getY()) && (pt.getY()<polygon[next].getY())) || ((polygon[next].getY()<=pt.getY()) && (pt.getY()<polygon[i].getY()))) && (pt.getX() < (polygon[next].getX() - polygon[i].getX()) * (pt.getY() - polygon[i].getY()) / (polygon[next].getY() - polygon[i].getY()) + polygon[i].getX()))
          {
            c = !c;
           }
       }
    return c;
   }


/**
 * \brief Tangent Averaging function
 * 
 * This function computes the average of the tangent values input.
 *
 * \warning The total "span" of the tangents must not exceed 180 degrees.
 * If it does, the answer would be meaningless. In fact, the "average"
 * is not even defined in this case.
 * */

#if 0

inline double  ComputeTangentAverage(vcl_vector<double> tangentList_)
  {
    unsigned int i;
    assert(tangentList_.size()>=1);
    
    //
    ///Make sure the tangent values lie between 0 and 2PI.
    //
    for (i=0;i<tangentList_.size();i++)
      {
        tangentList_[i]=fmod(tangentList_[i], 2*M_PI);
        if (tangentList_[i]<0)
            tangentList_[i] +=2*M_PI;
       }
    sort(tangentList_.begin(), tangentList_.end()); 
    
    double tangent=0, prev_tangent=0, tangent_sum=0, av_tangent=0;
    for (i=0;i<tangentList_.size();i++)
      {
        tangent = tangentList_[i];
        if (i==0)
            prev_tangent = tangent;
        
        if (fabs(tangent-prev_tangent)>M_PI)
          {
            if (tangent>0)
                tangent = tangent-2*M_PI;
            else
                tangent = tangent+2*M_PI;
           }
        
        if (fabs(tangent-prev_tangent)>M_PI)
          {
            cout<<" Warning : Angle Difference >M_PI"<<"  Prev = "<<prev_tangent*(180/M_PI)<<" Tangent = "<<tangent*(180/M_PI)<<" Diff = "<<fabs(tangent-prev_tangent)*(180/M_PI)<<endl;
           }
   
        
        tangent_sum  += tangent;
        prev_tangent  = tangent;
       }
    
    
    av_tangent = (tangent_sum/(tangentList_.size()*(1.0)));
    av_tangent = fmod(av_tangent, 2*M_PI);
    if (av_tangent<0)
        av_tangent +=2*M_PI;

    return (av_tangent);
   }

#endif

template<typename point1_type, typename point2_type>
double GetSlope(Point2D<point1_type> start, Point2D<point2_type> end)
  {
    double slope = atan2(end.getY()-start.getY(), end.getX()-start.getX());
    
    if (slope<0)
        slope +=2*M_PI;

    return slope;
   };


inline double AngleDiff(double start, double end)
  {
    double diff = end-start;

    diff = fmod(diff, TWICE_PI);
    if (diff<0)
        diff+=TWICE_PI;

    return diff;
   }


#if 0

class ArcFitter
  {
    private:
        double _span;
        vcl_vector<Point2D<double> > _boundary;
        vcl_vector<double>           _tangents;
        double _startAngle;
        double _endAngle;
        double _radius;
        
        
        double _stepSize;      //(Arc length)
        bool _counterClockwise;
        double _maxSpan;

    public:
        ArcFitter(double stepSize_, bool counterClockwise_):_stepSize(stepSize_), _counterClockwise(counterClockwise_)
          {
            assert(stepSize_>0);

            _span       = 0;
            _startAngle = 0;
            _endAngle   = 0;
            _radius     = 0;
            _maxSpan    = 0;
           };

        void operator()(Point2D<double> start, Point2D<double> end, Point2D<double> center)
          {
            //cout<<" Error = "<<(euc_distance(start, center) - euc_distance(end, center))<<endl; 
            _boundary.clear();
            _tangents.clear();
            _span       = 0;
            _startAngle = 0;
            _endAngle   = 0;

            _radius = euc_distance(start, center);
            assert(_radius>0);
            double angle_step = (_stepSize/_radius);
            
            _startAngle = GetSlope(center, start);
            _endAngle   = GetSlope(center, end);
            _span       = AngleDiff(_startAngle, _endAngle);
            

            //
            //Tangents always point in  the counter-clockwise direction.
            //
            double tangent_add=M_PI/2.0;
            
            
            if (!_counterClockwise)
              {
                _span          = -(TWICE_PI-_span);
                angle_step   *= -1.0;
               }
            
            double angle=0;
            int num_steps = (int)fabs(_span/angle_step);
            
            
            for (int i=1;i<(num_steps-1);i++)
              {
                angle = _startAngle+(angle_step)*i;
                _boundary.push_back(center+_radius*(Point2D<double>(cos(angle), sin(angle))));
                if (_maxSpan==0)
                    _tangents.push_back(angle+tangent_add);
                else
                  {
                    if (fabs(angle_step*i)<_maxSpan)
                        _tangents.push_back(angle+tangent_add);
                   }
               }
            
            //cout<<" Start = "<<start<<" End = "<<end<<" Center = "<<center<<" Radius ="<<_radius<<" Error ="<<error<<endl;
            return;
           }
        
        void setMaxSpan(double val)
          {
            _maxSpan=val;
           }
 
        bool checkSpan()
          {
            if (_maxSpan==0)
                return false;
            else
              {
                if (fabs(_span)<_maxSpan)
                    return false;
                else
                    return true;
               }
           }
               
        void setCounterClockwise(bool cond)
          {
            _counterClockwise = cond;
           }
        
        vcl_vector<double> getTangents()
          {
            return _tangents;
           }

        vcl_vector<Point2D<double> > getBoundary()
          {
            return _boundary;
           }
   };

#endif

/**
 * @brief Fits arc between the given points in the direction indicated.
 *
 * @warning NOTE : The tangents always point in the counter-clockwise
 * direction.
 * */

/*
    
    template<typename pointType>
vcl_vector<double> FitArc(pointType start, pointType end, pointType center, double step, bool counterClockwise, vcl_vector<pointType> &contour)
  {
    assert(step>0);
    double radius = euc_distance(start, center);
    assert(radius>0);
    double angle_step = (step/radius);
    double start_angle, end_angle, span;
    vcl_vector<double> tangents;
    
    start_angle = GetSlope(center, start);
    end_angle   = GetSlope(center, end);
    span        = AngleDiff(start_angle, end_angle);


    //
    //Tangents always point in  the counter-clockwise direction.
    //
    double tangent_add=M_PI/2.0;
   
    
    if (!counterClockwise)
      {
        span          = -(TWICE_PI-span);
        angle_step   *= -1.0;
       }

    double angle=0;
    int num_steps = (int)fabs(span/angle_step);
    
    
    for (int i=1;i<(num_steps-1);i++)
      {
        angle = start_angle+(angle_step)*i;
        contour.push_back(center+radius*(pointType(cos(angle), sin(angle))));
        tangents.push_back(angle+tangent_add);
       }
   
    return tangents;
   };

*/

template<typename pointType>
double GetTriangleArea(pointType pt0, pointType pt1, pointType pt2)
  {
    double area=0;
    area = 0.5*((pt1.getX()-pt0.getX())*(pt2.getY()-pt0.getY())-(pt2.getX()-pt0.getX())*(pt1.getY()-pt0.getY()));
    
    ///
    //Ensure that  area is not nan.
    assert((area>=0)||(area<0));
   
    return area;
   }


#endif /*  GEOMETRY_FUNCTIONS_H  */
