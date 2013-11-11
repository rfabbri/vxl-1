//      dbcfg_line.txx
#ifndef dbcfg_line_txx_
#define dbcfg_line_txx_

// Template for the contour fragment graph line
// Benjamin Gould
// 8/04/09

#include "dbcfg_line.h"



// creates a new line
template <class T>
dbcfg_line<T>::dbcfg_line(point_t& p1, point_t& p2, int depth) :
p1_(p1), p2_(p2), depth_(depth) {
}

// destruct the contour fragment graph constructor
template <class T>
dbcfg_line<T>::~dbcfg_line() {
}

// returns the length of this line segment
template <class T>
inline
double dbcfg_line<T>::length() {
  return vgl_distance(p1_, p2_);
}

// accesses the first endpoint
template <class T>
inline
vgl_point_2d<T> dbcfg_line<T>::p1() {
  return p1_;
}

// accesses the second endpoint
template <class T>
inline
vgl_point_2d<T> dbcfg_line<T>::p2() {
  return p2_;
}

// access the depth of the line segment
template <class T>
inline
int dbcfg_line<T>::depth() {
  return depth_;
}

// returns the distance between this line segment and the point
template <class T>
T dbcfg_line<T>::distance(point_t& point) {
  T closest_x;
  T closest_y;

  vgl_closest_point_to_linesegment(closest_x, closest_y, p1_.x(), p1_.y(), p2_.x(), p2_.y(), point.x(), point.y());

  return vgl_distance(point, point_t(closest_x, closest_y));
}

// returns the distance between this line segment and the line segment given
template <class T>
T dbcfg_line<T>::distance(line_t& line) {
  // get the line (not line segment) intersection
  // actually, the line intersection isn't strictly required
  // but this code is still used to test for the intersecting case below
  T x1 = p1_.x();
  T x2 = p2_.x();
  T x3 = line.p1_.x();
  T x4 = line.p2_.x();
  T y1 = p1_.y();
  T y2 = p2_.y();
  T y3 = line.p1_.y();
  T y4 = line.p2_.y();

  T num1 = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3));
  T num2 = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3));
  T den =  ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

  // parallel case
  if (den == 0) {
    return std::min(std::min(distance(line.p1_), distance(line.p2_)),
                    std::min(line.distance(p1_), line.distance(p2_)));
  }

  // intersecting case
  // u1 and u2 are intersection-fractions:
  // x = x1 + u1(x2 - x1)
  //   = x3 + u2(x4 - x3)
  // y = y1 + u1(y2 - y1)
  //   = y3 + u2(y4 - y3)
  double u1 = ((double) num1) / den;
  double u2 = ((double) num2) / den;
  if (u1 >=0 && u1 <= 1 && u2 >= 0 && u2 <= 1) {
    return 0;
  }

  // non-parallel, non-intersecting
  return std::min(std::min(distance(line.p1_), distance(line.p2_)),
                  std::min(line.distance(p1_), line.distance(p2_)));
}

// returns true if this line intersects the given one, within an epsilon error
template <class T>
bool dbcfg_line<T>::intersects(line_t& line, T epsilon) {
  return distance(line) <= epsilon;
}

// returns true if this line and the given one are identical, within an epsilon error
template <class T>
bool dbcfg_line<T>::is_duplicate(line_t& line, T epsilon) {
  return (vgl_distance(p1_, line.p1_) <= epsilon && vgl_distance(p2_, line.p2_) <= epsilon) ||
         (vgl_distance(p1_, line.p2_) <= epsilon && vgl_distance(p2_, line.p1_) <= epsilon);
}

// returns the midpoint "intersection" between this line and a point
template <class T>
vgl_point_2d<T> dbcfg_line<T>::get_intersection(point_t& point, T epsilon) {
  T closest_x;
  T closest_y;

  vgl_closest_point_to_linesegment(closest_x, closest_y, p1_.x(), p1_.y(), p2_.x(), p2_.y(), point.x(), point.y());

  return midpoint(point, point_t(closest_x, closest_y));
}

// returns the approx. intersection between this line and another
template <class T>
vgl_point_2d<T> dbcfg_line<T>::get_intersection(line_t& line, T epsilon) {
  if (vgl_distance(p1_, line.p1_) <= epsilon)
    return midpoint(p1_, line.p1_);
  if (vgl_distance(p1_, line.p2_) <= epsilon)
    return midpoint(p1_, line.p2_);
  if (vgl_distance(p2_, line.p1_) <= epsilon)
    return midpoint(p2_, line.p1_);
  if (vgl_distance(p2_, line.p2_) <= epsilon)
    return midpoint(p2_, line.p2_);

  // get the line (not line segment) intersection
  T x1 = p1_.x();
  T x2 = p2_.x();
  T x3 = line.p1_.x();
  T x4 = line.p2_.x();
  T y1 = p1_.y();
  T y2 = p2_.y();
  T y3 = line.p1_.y();
  T y4 = line.p2_.y();

  T num1 = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3));
  T num2 = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3));
  T den =  ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

  double u1 = ((double) num1) / den;
  double u2 = ((double) num2) / den;
  
  point_t intersect_point = point_t(x1 + u1 * (x2 - x1), y1 + u1 * (y2 - y1));

  if (u1 >=0 && u1 <= 1 && u2 >= 0 && u2 <= 1)
    return intersect_point;
  
  // no intersection case
  line_t* xline = false;
  point_t* xpoint = false;
  T min_distance = -1;
  T new_distance;

  new_distance = distance(line.p1_);
  if (min_distance < 0 || new_distance < min_distance){
    min_distance = new_distance;
    xline = this;
    xpoint = &(line.p1_);
  }

  new_distance = distance(line.p2_);
  if (min_distance < 0 || new_distance < min_distance){
    min_distance = new_distance;
    xline = this;
    xpoint = &(line.p2_);
  }

  new_distance = line.distance(p1_);
  if (min_distance < 0 || new_distance < min_distance){
    min_distance = new_distance;
    xline = &line;
    xpoint = &p1_;
  }

  new_distance = line.distance(p2_);
  if (min_distance < 0 || new_distance < min_distance){
    min_distance = new_distance;
    xline = &line;
    xpoint = &p2_;
  }

  return xline->get_intersection(*xpoint, epsilon);
}

// returns the transformed line(s) based on forced-intersections with the given points
// TODO : pierce lines with mid-line intersections to form multiple lines
template <class T>
vcl_vector<dbcfg_line<T> > dbcfg_line<T>::get_transform(vcl_vector<unsigned>& point_nums, points_t& points, T epsilon) {
  lines_t new_lines;
  if (point_nums.size() == 0) {
    new_lines.push_back(*this);
    return new_lines;
  }

  point_t* p1_tag = false;
  point_t* p2_tag = false;

  for (unsigned p = 0; p < point_nums.size(); p++) {
    if (p1_tag == false && vgl_distance(p1_, points[point_nums[p]]) <= epsilon) {
      p1_tag = &(points[point_nums[p]]);
    }
    else if (p2_tag == false && vgl_distance(p2_, points[point_nums[p]]) <= epsilon) {
      p2_tag = &(points[point_nums[p]]);
    }
  }

  if (p1_tag == false) p1_tag = &p1_;
  if (p2_tag == false) p2_tag = &p2_;

  new_lines.push_back(line_t(*p1_tag, *p2_tag, this->depth_));
  return new_lines;
}



// -------------------------
// INSTANTIATION
// -------------------------

#define DBCFG_LINE_INSTANTIATE(T) \
template class dbcfg_line<T>;\

#endif dbcfg_line_txx_
// end dbcfg_line.txx

