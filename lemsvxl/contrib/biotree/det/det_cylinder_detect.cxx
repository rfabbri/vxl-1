#include <det/det_cylinder_detect.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_numeric_traits.h>
#include <vcl_list.h>
#include <vcl_set.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vcl_limits.h>

// max eigen value of the noise covariance matrix is multiplied by a big number to get 
// smaller results at the end as strength

// used for nonoise, 5um resolution 2.02834*1000.0
double det_cylinder_detect::DET_MAX_EIG_VAL = 2.02834*1000.0; //100000000.0;//1000.0; 

double det_cylinder_detect::DET_NOISE_THRESH =  10.0;//214.0; //35.0;//65.0;

double det_cylinder_detect::EPS =  0.5;

det_cylinder_detect::det_cylinder_detect(void)
{
}

det_cylinder_detect::~det_cylinder_detect(void)
{
}


// gets a vector of vectors and a number to search inside. All the vectors containing 
// index will be stored in a new vector and returned
vcl_vector<vcl_vector<unsigned > *>
det_cylinder_detect::includes(vcl_vector<vcl_vector<unsigned > *> v, unsigned index)
{
  vcl_vector<vcl_vector<unsigned > *> result;
 
  for (unsigned int i=0; i < v.size(); i++){
    vcl_vector<unsigned > l = *(v[i]);
    bool found = false;
    for (unsigned int j=0; j < l.size() && !found; j++){
      if (l[j] == index) {
        found = true;
      }
    }
    if (found)
      result.push_back(v[i]);
  }
  return result;
}

vcl_vector<vgl_vector_3d<double> >
det_cylinder_detect::compute_v(xmvg_filter_response<double> &resp,
                               vcl_vector<vgl_vector_3d<double> > filter_dir) {

  vcl_list<vcl_pair<double, unsigned int > > resp_and_direct;
  for (unsigned int i=0; i < resp.size(); i++) {
    resp_and_direct.push_back(vcl_pair<double, unsigned int > (resp[i], i));
  }
  resp_and_direct.sort();

  // reverse the directions so that the biggest is at the beginning
  resp_and_direct.reverse();
 
  vcl_vector<vcl_pair<double, unsigned int > > resp_and_direct_vector;
  for (vcl_list<vcl_pair<double, unsigned int > >::iterator it = resp_and_direct.begin(); it != resp_and_direct.end(); it++) {
    vcl_pair<double, unsigned int > p = *it;
    resp_and_direct_vector.push_back(p);
  }

  unsigned int first_index = resp_and_direct_vector[0].second;
  unsigned int second_index = resp_and_direct_vector[1].second;
  unsigned int third_index = resp_and_direct_vector[2].second;
  // os << "the biggest filter indices: " << first_index << " " << second_index << " " << third_index << "\n";
 
  // creates the valid indices sets
  vcl_vector<vcl_vector<unsigned > *> triangles;
  vcl_vector<unsigned> *tri1 = new vcl_vector<unsigned> (3);
  (*tri1)[0]=0; (*tri1)[1]=1; (*tri1)[2]=4;   // case 014
  triangles.push_back(tri1);
    
  vcl_vector<unsigned> *tri2 = new vcl_vector<unsigned> (3);
  (*tri2)[0]=0; (*tri2)[1]=2; (*tri2)[2]=3;   // case 015
  triangles.push_back(tri2);

  vcl_vector<unsigned> *tri3 = new vcl_vector<unsigned> (3);
  (*tri3)[0]=0; (*tri3)[1]=2; (*tri3)[2]=4;   // case 023
  triangles.push_back(tri3);

  vcl_vector<unsigned> *tri4 = new vcl_vector<unsigned> (3);
  (*tri4)[0]=1; (*tri4)[1]=4; (*tri4)[2]=3;   // case 024
  triangles.push_back(tri4);

  vcl_vector<unsigned> *tri5 = new vcl_vector<unsigned> (3);
  (*tri5)[0]=2; (*tri5)[1]=1; (*tri5)[2]=5;   // case 035
  triangles.push_back(tri5);

  vcl_vector<unsigned> *tri6 = new vcl_vector<unsigned> (3);
  (*tri6)[0]=2; (*tri6)[1]=3; (*tri6)[2]=1;   // case 123
  triangles.push_back(tri6);

  vcl_vector<unsigned> *tri7 = new vcl_vector<unsigned> (3);
  (*tri7)[0]=2; (*tri7)[1]=4; (*tri7)[2]=5;   // case 125
  triangles.push_back(tri7);

  vcl_vector<unsigned> *tri8 = new vcl_vector<unsigned> (3);
  (*tri8)[0]=4; (*tri8)[1]=3; (*tri8)[2]=5;   // case 134
  triangles.push_back(tri8);

  vcl_vector<unsigned> *tri9 = new vcl_vector<unsigned> (3);
  (*tri9)[0]=5; (*tri9)[1]=0; (*tri9)[2]=1;   // case 245
  triangles.push_back(tri9);

  vcl_vector<unsigned> *tri10 = new vcl_vector<unsigned> (3);
  (*tri10)[0]=5; (*tri10)[1]=0; (*tri10)[2]=3;   // case 345
  triangles.push_back(tri10);

  vcl_vector<vcl_vector<unsigned > *> third_set, second_set;

  // first find the set of valid indices that contain the biggest response
  vcl_vector<vcl_vector<unsigned > *> first_set = includes(triangles, first_index);
  if (first_set.empty()) {
    //os << "ERROR  --- first biggest filter is not found!!!!!" << vcl_endl;
  } else {
    // find the second set which includes the 2nd one
    second_set = includes(first_set, second_index);
    if (second_set.empty()) {
      //os << "ERROR  --- second biggest filter is not found!!!!!" << vcl_endl;
    } else {
      // find the second set which includes the 2nd one
      third_set = includes(second_set, third_index);
      if (third_set.empty()) {
        // third one id not found, we will search for the best possible
        
        bool selected = false;
        for (unsigned int i = 3; i < resp_and_direct_vector.size() && !selected; i++) {
          unsigned int index = resp_and_direct_vector[i].second;
          //vcl_vector<vcl_vector<unsigned > *>
            third_set = includes(second_set, index);
          if (!third_set.empty()) {
            third_index = index;
            selected = true;
          }
        }
        if (!selected) {
          //os << "ERROR  --- second biggest filter is not found!!!!!" << vcl_endl;
        }
      }
      else {
      }
    }
  }

  //os << "resulting filter indices: " << first_index << " " << second_index << " " << third_index << "\n";

  unsigned int select_index;
  for (unsigned i=0; i<triangles.size(); i++) {
    if (third_set[0] == triangles[i]) {
      select_index = i;
    }
  }

  vgl_vector_3d<double> fmax, fmax_, fmax__;
  fmax = filter_dir[first_index];
  fmax_ = filter_dir[second_index];
  fmax__ = filter_dir[third_index];

  

  if (select_index == 3) {
    if (first_index == 3) 
      fmax = -1*filter_dir[first_index];
    else if (second_index == 3)
      fmax_ = -1*filter_dir[second_index];
    else if (third_index == 3) 
      fmax__ = -1*filter_dir[third_index];

  } else if ((select_index == 4) || (select_index == 5)) {
    if (first_index == 1) 
      fmax = -1*filter_dir[first_index];
    else if (second_index == 1)
      fmax_ = -1*filter_dir[second_index];
    else if (third_index == 1) 
      fmax__ = -1*filter_dir[third_index];

  } else if (select_index == 7) {
    if (first_index == 3) 
      fmax = -1*filter_dir[first_index];
    else if (second_index == 3)
      fmax_ = -1*filter_dir[second_index];
    else if (third_index == 3) 
      fmax__ = -1*filter_dir[third_index];

  } else if (select_index == 8) {
    if ((first_index == 0) || (first_index == 1)) 
      fmax = -1*filter_dir[first_index];
    if ((second_index == 0) || (second_index == 1)) 
      fmax_ = -1*filter_dir[second_index];
    if ((third_index == 0) || (third_index == 1)) 
      fmax__ = -1*filter_dir[third_index];

  } else if (select_index == 9) {
    if ((first_index == 0) || (first_index == 3)) 
      fmax = -1*filter_dir[first_index];
    if ((second_index == 0) || (second_index == 3)) 
      fmax_ = -1*filter_dir[second_index];
    if ((third_index == 0) || (third_index == 3)) 
      fmax__ = -1*filter_dir[third_index];
  }
 
  // compute the sub filter positions
  vcl_vector<vgl_vector_3d<double> > v(15);
  v[0] = fmax;
  v[14] = fmax_;
  v[10] = fmax__;

  vgl_vector_3d<double> temp = v[0] + v[10];
  v[3] = temp / temp.length();

  temp = v[0] + v[14];
  v[5] = temp / temp.length();

  temp = v[14] + v[10];
  v[12] = temp / temp.length();

  temp = v[0] + v[3];
  v[1] = temp / temp.length();

  temp = v[0] + v[5];
  v[2] = temp / temp.length();

  temp = v[3] + v[5];
  v[4] = temp / temp.length();

  temp = v[3] + v[10];
  v[6] = temp / temp.length();

  temp = v[3] + v[12];
  v[7] = temp / temp.length();

  temp = v[5] + v[12];
  v[8] = temp / temp.length();

  temp = v[3] + v[14];
  v[9] = temp / temp.length();

  temp = v[12] + v[10];
  v[11] = temp / temp.length();

  temp = v[12] + v[14];
  v[13] = temp / temp.length();

  // modify v if necessary
  return v;
}
vgl_vector_3d<double>
det_cylinder_detect::detect_dir(xmvg_filter_response<double>& resp,
                            vcl_vector<vgl_vector_3d<double> > v,
                            vcl_vector<vgl_vector_3d<double> > filter_dir)
{
  vcl_vector<xmvg_filter_response<double> > fv;

  // for each v, compute the filter response
  for (unsigned int i=0; i < v.size(); i++) {
    xmvg_filter_response<double> f(resp.size());
    for (unsigned int j=0; j < filter_dir.size(); j++) {
      double dot = dot_product(v[i], filter_dir[j]);
      f[j] = (1.0 - (1.0 - 2.0*EPS*EPS)*dot*dot) / 
        vcl_pow((1. - (1. -(EPS*EPS))*dot*dot), 1.5);
    }
    fv.push_back(f);
  }


  // find a, scale factor
  vcl_vector<double> a;
  for (unsigned j=0; j < v.size(); j++) {
    double sum1 = 0;
    for (unsigned i=0; i< resp.size(); i++) {
      sum1 += resp[i]* fv[j][i];
    }
    double sum2 = 0;
    for (unsigned i=0; i< resp.size(); i++) {
      sum2 += fv[j][i]*fv[j][i];
    }
    a.push_back(sum1/sum2);
  }

  // find the error for each v
  vcl_vector<double> E;
  double min_e = vnl_numeric_traits<double>::maxval;
  int min_index=0;
  for (unsigned j=0; j < v.size(); j++) {
    double e=0;
    for (unsigned i=0; i< resp.size(); i++) {
      e += (resp[i] - a[j]*fv[j][i])*(resp[i] - a[j]*fv[j][i]);
    }
    if (e < min_e) {
      min_e = e;
      min_index = j;
    }
    E.push_back(e);
  }
  // modify v if necessary
  vgl_vector_3d<double> x(v[min_index]);
  if (v[min_index].z() < 0) {
    return -1*v[min_index];
  } else if (v[min_index].z() == 0) {
    if (v[min_index].y() < 0) {
      return -1*v[min_index];
    } else if ((v[min_index].y() == 0) && (v[min_index].x() < 0)) {
       return -1*v[min_index];
    }
  }
  return v[min_index];
}

void det_cylinder_detect::normalize(xmvg_filter_response<double> &resp) {
  // get the sum of the vector values
  double sum = 0.0;
  double max = 0.0;
  for (unsigned i=0; i < resp.size(); i++) {
    double val = vcl_fabs(resp[i]);
    sum += val;
    if (val > max)
      max = val;
  }
  
  if (sum != 0.0) {
    for (unsigned i=0; i < resp.size(); i++) {
      resp[i] = resp[i]/sum;
    }
  }
}

//: detect cylinder in a point given response and 
// filter_descriptor. It return a data structure of cylinder map entry.
det_map_entry det_cylinder_detect::det_type(xmvg_filter_response<double> & resp, 
                                           xmvg_composite_filter_descriptor const& fds,
                       double radius)
{

  int num_filters = resp.size();
  assert(num_filters == fds.size());

  vcl_vector<vgl_vector_3d<double> > filter_dir(num_filters);
  vcl_vector<double> resp_value(num_filters);

  for(int i = 0; i < num_filters; i++)
  {
    filter_dir[i] = fds[i].orientation();
    resp_value[i] = resp[i];
  }

  double min_val = *(vcl_min_element(resp_value.begin(), resp_value.end()));
  double max_val = *(vcl_max_element(resp_value.begin(), resp_value.end()));
  
  double t = sqrt((min_val*min_val)/DET_MAX_EIG_VAL);
 
  cme_t cm;

  if (radius == 0) { //used to be ((t <= thresh) || (min_val < 0)){
    cm.strength_ = 0.0;
    cm.dir_ = vgl_vector_3d<double> (0, 0, 0);

  // it is a cylinder
  } else { 
    
    // find the biggest filter value and it's index
    t = sqrt((max_val*max_val)/DET_MAX_EIG_VAL);
    cm.strength_ = t;
    int index_of_biggest;
    for (unsigned int i=0; i < resp.size(); i++) {
      if (max_val == resp[i])
        index_of_biggest = i;
    }
    // icosahedron filter case
    if (resp.size() == 6) {
      vcl_vector<vgl_vector_3d<double> > v = compute_v(resp, filter_dir);
      vgl_vector_3d<double> dir = detect_dir(resp, v, filter_dir);
      cm.dir_ = dir; 
    } else {
       cm.dir_ = filter_dir[index_of_biggest];
       //cm.strength_ = sqrt((resp[4]*resp[4])/DET_MAX_EIG_VAL);
       //cm.dir_ = vgl_vector_3d<double> (0,0,1.); 
    }
  }
  return cm;
}

// creates a cylinder map from the filter responses
det_cylinder_map det_cylinder_detect::apply(unsigned nx, unsigned ny, unsigned nz,
    vcl_vector<xmvg_filter_response<double> > const& responses,
    xmvg_composite_filter_descriptor const & fds,
    vil3d_image_view<unsigned char> *radius_view) 
{
  int index = 0;
  det_cylinder_map map(nx, ny, nz);
  double radius=0;
  vil3d_image_view<unsigned char> rv;

  if (radius_view != 0) {
    int x=radius_view->ni();
    int y=radius_view->nj();
    int z=radius_view->nk();
    assert((x == nx) && (y == ny) && (z == nz));
    rv = *radius_view;
  }

  for(unsigned int k=0; k < nz; k++)
    for(unsigned int j=0; j < ny; j++)
      for(unsigned int i=0; i < nx; i++) {
        xmvg_filter_response<double> resp(responses[index++]);
    if (radius_view != 0) {
      radius = rv(i,j,k);
    }
        det_map_entry entry = det_type(resp, fds, radius);
        map[i][j][k] = entry;
        map[i][j][k].radius_ = radius;
      }

  return map;
}

// creates a cylinder map from the directions and the strengths
det_cylinder_map det_cylinder_detect::apply(unsigned nx, unsigned ny, unsigned nz,
                                            vil3d_image_view<float> &directions,
                                            vil3d_image_view<float> &I_s) 
{
  int index = 0;
  det_cylinder_map map(nx, ny, nz);

  for(unsigned int k=0; k < nz; k++)
    for(unsigned int j=0; j < ny; j++)
      for(unsigned int i=0; i < nx; i++) {
        det_map_entry entry;
        entry.strength_ = I_s(i,j,k);
        vgl_vector_3d<double> direction(static_cast<double>(directions(i,j,k,0)), static_cast<double>(directions(i,j,k,1)), static_cast<double>(double(directions(i,j,k,2))));
        entry.dir_ = direction;
        entry.location_ = vgl_point_3d<double> (0.0, 0.0, 0.0);
        map[i][j][k] = entry;
      }

      return map;
}
