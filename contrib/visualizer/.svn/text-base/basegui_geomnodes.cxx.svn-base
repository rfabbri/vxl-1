#include "basegui_geomnodes.h"
#include "basegui_visitor.h"

//#include "xp/xpmath.h"
//#include "geometry_functions.h"

const int NUM_ELLIPSE_SUBDIVISIONS = 100;

void PointGeom::draw(SceneGraphVisitor *v) {
  v->beginPoints();
  v->vertex(x, y);
  v->end();
}

void LineGeom::draw(SceneGraphVisitor *v) {
  v->beginPolyline();
  v->vertex(p0.getX(), p0.getY());
  v->vertex(p1.getX(), p1.getY());
  v->end();
}

int LineGeom::getPoints_(vcl_vector<Point2D<double> >& pts_){
  pts_.push_back(p0);
  pts_.push_back(p1);
  return 2;
}
  
bool LineGeom::setPoint_(int id_, const Point2D<double> pt_){
  if(id_>=2){
    return false;
  }else{
    if(id_==0)
      p0 = pt_;
    else if (id_==1)
      p1 = pt_;
    return true;
  }
}
  
void PolylineGeom::draw(SceneGraphVisitor *v) {
  v->beginPolyline();
  for(vcl_vector<Point2D<double> >::const_iterator
      i = _points.begin(); i != _points.end(); ++i)
    v->vertex(i->getX(), i->getY());
  v->end();
}

int PolylineGeom::getPoints_(vcl_vector<Point2D<double> >& pts_){
  pts_ = _points;
  return _points.size();
}
  
bool PolylineGeom::setPoint_(int id_, const Point2D<double> pt_){
  if(id_>=(int)_points.size()){
    return false;
  }else{
    _points[id_] = pt_;
    return true;
  }
}
  
void UnfilledPolygonGeom::draw(SceneGraphVisitor *v) {
  v->beginUnfilledPolygon();
  for(vcl_vector<Point2D<double> >::const_iterator
      i = _points.begin(); i != _points.end(); ++i)
    v->vertex(i->getX(), i->getY());
  v->end();
}

void ConvexPolygonGeom::draw(SceneGraphVisitor *v) {
  v->beginFilledPolygon();
  for(vcl_vector<Point2D<double> >::const_iterator
      i = _points.begin(); i != _points.end(); ++i)
    v->vertex(i->getX(), i->getY());
  v->end();
}

void ScaleInvariantConvexPolygonGeom::draw(SceneGraphVisitor *v) {
  v->beginFilledPolygon();
  for(vcl_vector<Point2D<double> >::const_iterator
      i = _points.begin(); i != _points.end(); ++i)
    v->vertex(i->getX(), i->getY());
  v->end();
}

void EllipseGeom::draw(SceneGraphVisitor *v) {
  int n_line_segs = int(NUM_ELLIPSE_SUBDIVISIONS*fabs(t1-t0)/(2*M_PI));
  if(n_line_segs < 4) n_line_segs = 4;
  bool wholeEllipse = fabs(t1-t0) >= 2*M_PI-1e-5;

  v->beginFilledPolygon();
  if(!wholeEllipse)
    v->vertex(x, y);
  for(int i = 0; i < n_line_segs; ++i) {
    double t = t0 + (t1-t0)*i/double(n_line_segs-1);
    v->vertex(x+a*cos(t), y+b*sin(t));
  }
  v->end();
}

void EllipseOutlineGeom::draw(SceneGraphVisitor *v) {
  int n_line_segs = int(NUM_ELLIPSE_SUBDIVISIONS*fabs(t1-t0)/(2*M_PI));
  if(n_line_segs < 4) n_line_segs = 4;

  v->beginPolyline();
  for(int i = 0; i < n_line_segs; ++i) {
    double t = t0 + (t1-t0)*i/double(n_line_segs-1);
    v->vertex(x+a*cos(t), y+b*sin(t));
  }
  v->end();
}

int EllipseOutlineGeom::getPoints_(vcl_vector<Point2D<double> >& pts_){
  pts_.push_back(Point2D<double>(x,y));
  return 1;
}
  
bool EllipseOutlineGeom::setPoint_(int id_, const Point2D<double> pt_){
  if(id_>=1){
    return false;
  }else{
    x = pt_.getX();
    y = pt_.getY();
    return true;
  }
}

ArcGeom::ArcGeom (double sx, double sy, double ex, double ey,
            double cx, double cy, double R, int newnud):
  EllipseOutlineGeom(cx, cy, R, R, 0, 0)
{
  Start.set (sx, sy);
  End.set (ex, ey);
  Center.set (cx, cy);
  r = R;
  nud = newnud;

  t0 = atan2 (sy-cy, sx-cx);
   t1 = atan2 (ey-cy, ex-cx);

  if (nud==1){
    vcl_swap(t0, t1);
  }

  if(t0 > t1) t1 += 2*M_PI;      
}

ArcGeom::ArcGeom(double start_angle, double end_angle, double cx, double cy,
            double newr, int newnud):
  EllipseOutlineGeom(cx, cy, newr, newr, 0, 0)
{
  r = newr;
  Center.set (cx, cy);
  Start = vectorPoint (Center, start_angle, r);
  End   = vectorPoint (Center, end_angle, r);
  nud = newnud;

  t0 = start_angle;
   t1 = end_angle;

  if (nud==1){
    vcl_swap(t0, t1);
  }

  if(t0 > t1) t1 += 2*M_PI;
}

void ArcGeom::setParameters (double sx, double sy, double ex, double ey,
          double cx, double cy, double R, int newnud)
{
  Start.set (sx, sy);
  End.set (ex, ey);
  Center.set (cx, cy);

  r = R;
  nud = newnud;

  t0 = atan2 (sy-cy, sx-cx);
   t1 = atan2 (ey-cy, ex-cx);

  if(t0 > t1) t1 += 2*M_PI; 

  if (nud==1){
    vcl_swap(t0, t1);
  }

  set(cx, cy, R, R, t0, t1);
}

void ScaleInvariantEllipseGeom::draw(SceneGraphVisitor *v) {
  int n_line_segs = int(NUM_ELLIPSE_SUBDIVISIONS*fabs(t1-t0)/(2*M_PI));
  if(n_line_segs < 4) n_line_segs = 4;
  bool wholeEllipse = fabs(t1-t0) >= 2*M_PI-1e-5;

  v->beginFilledPolygon();
  if(!wholeEllipse)
    v->vertex(0, 0);
  for(int i = 0; i < n_line_segs; ++i) {
    double t = t0 + (t1-t0)*i/double(n_line_segs-1);
    v->vertex(a*cos(t), b*sin(t));
  }
  v->end();
}

void ScaleInvariantEllipseOutlineGeom::draw(SceneGraphVisitor *v) {
  int n_line_segs = int(NUM_ELLIPSE_SUBDIVISIONS*fabs(t1-t0)/(2*M_PI));
  if(n_line_segs < 4) n_line_segs = 4;

  v->beginPolyline();
  for(int i = 0; i < n_line_segs; ++i) {
    double t = t0 + (t1-t0)*i/double(n_line_segs-1);
    v->vertex(a*cos(t), b*sin(t));
  }
  v->end();
}

void ScaleInvariantPolylineGeom::draw(SceneGraphVisitor *v) {
  v->beginPolyline();
  for(points_t::const_iterator
      i = _points.begin(); i != _points.end(); ++i)
    v->vertex(i->getX(), i->getY());
  v->end();
}

void FilledPolygonGeom::draw(SceneGraphVisitor *v) {
  v->beginConcaveFilledPolygon();
  for(vcl_vector<Point2D<double> >::const_iterator
      i = _points.begin(); i != _points.end(); ++i)
    v->concaveVertex(i->getX(), i->getY());
  v->endConcaveFilledPolygon();
}

void ScaleInvariantFilledPolygonGeom::draw(SceneGraphVisitor *v) {
  v->beginConcaveFilledPolygon();
  for(vcl_vector<Point2D<double> >::const_iterator
      i = _points.begin(); i != _points.end(); ++i)
    v->concaveVertex(i->getX(), i->getY());
  v->endConcaveFilledPolygon();
}

ImageGeom::ImageGeom(Image *ii, double xx1, double yy1, double w, bool flip) 
  {
    img = ii;
    x1 = xx1;
    y1 = yy1;
    width = w;
    height = w*(ii->height()/double(ii->width()));
    if(flip)
        height = -height;
   }

ImageGeom::ImageGeom(Image *ii, double xx1, double yy1, double w, double h, bool flip) 
  {
    img = ii;
    x1 = xx1;
    y1 = yy1;
    width = w;
    height = h;
    if(flip)
        height = -height;
   }

ImageGeom::ImageGeom(vcl_string file_name,  double xx1, double yy1, double w, bool flip)
  {
    //NOTE RGBImage is being used.
    img  = new RGBImage(file_name.c_str());
    x1 = xx1;
    y1 = yy1;
    width = w;
    height = w*(img->height()/double(img->width()));
    if(flip)
        height = -height;
   }

ImageGeom::ImageGeom(vcl_string file_name, double xx1, double yy1, double w, double h, bool flip) 
  {
    //NOTE RGBImage is being used.
    img  = new RGBImage(file_name.c_str());
    x1 = xx1;
    y1 = yy1;
    width = w;
    height = h;
    if(flip)
        height = -height;
   }


ImageGeom::ImageGeom(vcl_string file_name, double xx1, double yy1, bool flip) 
  {
    //NOTE RGBImage is being used.
    img    = new RGBImage(file_name.c_str());
    x1     = xx1;
    y1     = yy1;
    width  = img->width();
    height = img->height();
    if(flip)
        height = -height;
   }


int ImageGeom::getInformation(vcl_string &info, Point2D<double> pos)
  {
    vcl_ostringstream ostrm;
            ostrm<<" Image Coords = ["<<x1<<", "<<y1<<", "<<width<<", "<<height<<"] "<<vcl_endl;
    
    int x0=(int)pos.getX();
    int y0=(int)pos.getY();
    if ((x0>=0)&&(x0<img->width())&&(y0>=0)&&(y0<img->height()))
      {
        RGBImage *rgb = dynamic_cast<RGBImage *> (img);
        if (rgb)
          {
            ostrm<<" Pixel [RGB]   = ["<<(int)rgb->r(x0,y0)<<", "<<(int)rgb->g(x0,y0)<<", "<<(int)rgb->b(x0, y0)<<"] "<<vcl_endl;
           }
       }
    
    info=ostrm.str();
    return 0;
   }



void ImageGeom::draw(SceneGraphVisitor *v) {
  v->image(x1, y1, x1 + width, y1 + height, img);
}

void TextGeom::draw(SceneGraphVisitor *v) {
  v->text(x, y, txt.c_str());
}

AffineTextGeom::AffineTextGeom(float xx, float yy, vcl_string tt, bool flip) : x(xx), y(yy), txt(tt) {
  if(flip)
    pushXForm(XForm::reflect_y_axis());
}

AffineTextGeom::AffineTextGeom(Point2D<double> pos, vcl_string tt, bool flip) : x(pos.getX()), y(pos.getY()), txt(tt) 
  {
    if(flip)
        pushXForm(XForm::reflect_y_axis());
   }

void AffineTextGeom::draw(SceneGraphVisitor *v) {
  v->affineText(x, y, txt.c_str());
}


ShockArrowGeom::ShockArrowGeom (double x, double y, double dir, 
  double ARROW_SIZE, double ARROW_ANGLE) :
    ScaleInvariantFilledPolygonGeom (x, y)
{
  //compute arrow polygon
  Point2D<double> pt  (ARROW_SIZE*cos(dir),             ARROW_SIZE*sin(dir));
  Point2D<double> ar1 (-ARROW_SIZE*cos(dir-ARROW_ANGLE), -ARROW_SIZE*sin(dir-ARROW_ANGLE));
  Point2D<double> ar2 (-ARROW_SIZE*cos(dir+ARROW_ANGLE), -ARROW_SIZE*sin(dir+ARROW_ANGLE));
  push_back(pt);
  push_back(ar1);
  push_back(ar2);
}


ArrowHeadGeom::ArrowHeadGeom(Point2D<double> pt_, double angle_,  double totalLength_)
  {
    double theta=0;
    Point2D<double> temp;
    
    push_back(pt_);
    
    angle_ = fmod(angle_, 2*M_PI);
    if (angle_<0)
        angle_ +=2*M_PI;
    
    theta = angle_ +M_PI/3;
    
    temp.setX(pt_.getX() - (totalLength_/2.0)*(cos(theta)));
    temp.setY(pt_.getY() - (totalLength_/2.0)*(sin(theta)));
    push_back(temp);
    
    theta = angle_;
    
    temp.setX(pt_.getX() + (totalLength_/2.0)*(cos(theta)));
    temp.setY(pt_.getY() + (totalLength_/2.0)*(sin(theta)));
    push_back(temp);
    
    
    theta = (M_PI/3)-angle_;
    
    temp.setX(pt_.getX() - (totalLength_/2.0)*(cos(theta)));
    temp.setY(pt_.getY() + (totalLength_/2.0)*(sin(theta)));
    push_back(temp);
    push_back(pt_);
   }

void ArrowHeadGeom::setDirection(double angle_)
  {
   double theta=0, total_length=0;
   Point2D<double> temp;
  
   total_length=euc_distance(point(0), point(1)); 
   
   

    angle_ = fmod(angle_, 2*M_PI);
    if (angle_<0)
        angle_ +=2*M_PI;
    
    theta = angle_ +M_PI/3;
    
    temp.setX(point(0).getX() - (total_length)*(cos(theta)));
    temp.setY(point(0).getY() - (total_length)*(sin(theta)));
    setPoint(1, temp);
    
    theta = angle_;
    
    temp.setX(point(0).getX() + (total_length)*(cos(theta)));
    temp.setY(point(0).getY() + (total_length)*(sin(theta)));
    setPoint(2, temp);
    
    
    theta = (M_PI/3)-angle_;
    
    temp.setX( point(0).getX()- (total_length)*(cos(theta)));
    temp.setY( point(0).getY()+ (total_length)*(sin(theta)));
    setPoint(3, temp);
    setPoint(4, point(0));
   }

void ArrowHeadGeom::setPosition(Point2D<double> basePt_)
  {
    Point2D<double> diff;
    diff = basePt_-point(0);

    for (unsigned int i=0;i<_points.size();i++)
      {
        _points[i] = _points[i]+diff;
       }
   } 

double ArrowHeadGeom::getDirection()
  {
    if (_points.size()==5)
      {
        double theta=0;
        theta = atan2(_points[2].getY()-_points[0].getY(),_points[2].getX()-_points[0].getX());
        if (theta<0)
            theta +=2*M_PI;
        return theta;
       }
    else
      {
        return 0;
       }
   }


void EulerSpiralSplineGeom::draw(SceneGraphVisitor *visitor)
  {
    if (_pointTangents.size()<2)
        return;

    if (_params.size()!=(_pointTangents.size()-1))
        computeParams();


    EulerSpiral ES;
    Point2D<double> pt, prev_pt;
    
    StyleSet store=visitor->currentStyle();
    visitor->mergeStyle(Color(RED)& LineWidth(4));
    visitor->beginPolyline();
    for (unsigned int i=0;i<_params.size();i++)
      {
        for(double s = 0; s <= _params[i].getLength()*1.00001; s += (_params[i].getLength()*1.0)/100) 
          {
            pt = ES.compute_es_point(_params[i], s);
            if (prev_pt!=pt)
                visitor->vertex(pt.getX(), pt.getY());
            prev_pt=pt;
           }
       }
    visitor->end();
    visitor->loadStyle(store);
   }


void BiArcSplineGeom::computeParams()
  {
    if (_pointTangents.size()==0)
        return;

    if (_params.size()<(_pointTangents.size()-1))
      {
        BiArc BA;
        int start = _params.size();
        int end   = _pointTangents.size()-1;
        double start_angle=0, end_angle=0;
        for (int i=start;i<end;i++)
          {
            BA.set_params(_pointTangents[i].first, _pointTangents[i].second,_pointTangents[i+1].first, _pointTangents[i+1].second);  
            _params.push_back(BA.getParams());
            //
            //Compute BiArcInfo. (Basically info needed for drawing.
            //
            _params[_params.size()-1].computeBiArcInfo();

            start_angle   = atan2(_params[_params.size()-1].circle1Start.getY()-_params[_params.size()-1].circle1Center.getY(),_params[_params.size()-1].circle1Start.getX()-_params[_params.size()-1].circle1Center.getX());
            if (start_angle<0)
                start_angle +=2*M_PI;
            
            end_angle     = atan2(_params[_params.size()-1].circle1End.getY()  -_params[_params.size()-1].circle1Center.getY(),_params[_params.size()-1].circle1End.getX()  -_params[_params.size()-1].circle1Center.getX());
            if (end_angle<0)
                end_angle +=2*M_PI;

            EllipseInfo info;

            info.setCenter(_params[_params.size()-1].circle1Center);
            info.setA((_params[_params.size()-1].circle1Radius));
            info.setB((_params[_params.size()-1].circle1Radius));

            if (_params[_params.size()-1].circle1Radius>0)
              {
                info.setStartTheta(start_angle);
                info.setEndTheta(end_angle);
               }
            else
              {
                info.setStartTheta(end_angle);
                info.setEndTheta(start_angle);
               }
            _biarcs.push_back(info);

            start_angle   = atan2(_params[_params.size()-1].circle2Start.getY()-_params[_params.size()-1].circle2Center.getY(),_params[_params.size()-1].circle2Start.getX()-_params[_params.size()-1].circle2Center.getX());
            if (start_angle<0)
                start_angle +=2*M_PI;
 
            end_angle     = atan2(_params[_params.size()-1].circle2End.getY()  -_params[_params.size()-1].circle2Center.getY(),_params[_params.size()-1].circle2End.getX()  -_params[_params.size()-1].circle2Center.getX());
            if (end_angle<0)
                end_angle +=2*M_PI;
 
            info.setCenter(_params[_params.size()-1].circle2Center);
            info.setA((_params[_params.size()-1].circle2Radius));
            info.setB((_params[_params.size()-1].circle2Radius));

            if (_params[_params.size()-1].circle2Radius>0)
              {
                info.setStartTheta(start_angle);
                info.setEndTheta(end_angle);
               }
            else
              {
                info.setStartTheta(end_angle);
                info.setEndTheta(start_angle);
               }
            _biarcs.push_back(info);

           }
       }

   }

void BiArcSplineGeom::draw(SceneGraphVisitor *v)
  {
    if (_pointTangents.size()<2)
        return;

    if ((_params.size()!=(_pointTangents.size()-1))||(_biarcs.size()!=2*_params.size()))
        computeParams();
   
    int n_line_segs=0;
    double theta=0;
    double diff=0;

    StyleSet store=v->currentStyle();
    v->mergeStyle(LineWidth(4)&Color(CYAN));
    v->beginPolyline();
    for (unsigned int i=0;i<_biarcs.size();i++)
      {
        diff = _biarcs[i].getEndTheta()-_biarcs[i].getStartTheta();
        if (diff<0)
            diff +=2*M_PI;

        n_line_segs = int(NUM_ELLIPSE_SUBDIVISIONS*(diff)/(2*M_PI));
        if(n_line_segs < 4) n_line_segs = 4;
       
        if ((i%2)==0)
            v->mergeStyle(Color(CYAN));
        else
            v->mergeStyle(Color(MAGENTA));

        if (_biarcs[i].getA()>0)
          {
            for(int j = 0; j < n_line_segs; ++j) 
              {
                theta = _biarcs[i].getStartTheta() + (diff)*j/double(n_line_segs-1);
                v->vertex(_biarcs[i].getCenter().getX()+fabs(_biarcs[i].getA())*cos(theta), _biarcs[i].getCenter().getY()+fabs(_biarcs[i].getB())*sin(theta));
               }
           }
        else
          {
            for(int j = (n_line_segs-1); j >=0; --j) 
              {
                theta = _biarcs[i].getStartTheta() + (diff)*j/double(n_line_segs-1);
                v->vertex(_biarcs[i].getCenter().getX()+fabs(_biarcs[i].getA())*cos(theta), _biarcs[i].getCenter().getY()+fabs(_biarcs[i].getB())*sin(theta));
               }
           
           }
       }
    v->end();
    v->mergeStyle(store);
    
   }

