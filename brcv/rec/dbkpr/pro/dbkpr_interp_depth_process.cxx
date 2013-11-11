// This is brcv/rec/dbkpr/pro/dbkpr_interp_depth_process.cxx

//:
// \file

#include "dbkpr_interp_depth_process.h"
#include <dbdet/dbdet_lowe_keypoint.h>
#include <dbdet/dbdet_keypoint_corr3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_memory_image.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_distance.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_iomanip.h>
#include <vcl_algorithm.h>

#include <dbkpr/pro/dbkpr_corr3d_storage_sptr.h>
#include <dbkpr/pro/dbkpr_corr3d_storage.h>

#include <vidpro1/storage/vidpro1_image_storage.h>

#include <bpro1/bpro1_parameters.h>


//: Constructor
dbkpr_interp_depth_process::dbkpr_interp_depth_process()
{
  if( !parameters()->add( "Output File",       "-file",  bpro1_filepath("","*")) ||
      !parameters()->add( "Sample Rate",       "-samp",  (int)1) ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbkpr_interp_depth_process::~dbkpr_interp_depth_process()
{
}


//: Clone the process
bpro1_process*
dbkpr_interp_depth_process::clone() const
{
  return new dbkpr_interp_depth_process(*this);
}


//: Return the name of this process
vcl_string
dbkpr_interp_depth_process::name()
{
  return "Depth Iterpolation";
}


//: Return the number of input frame for this process
int
dbkpr_interp_depth_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbkpr_interp_depth_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbkpr_interp_depth_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "keypoints_corr3d" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbkpr_interp_depth_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}

inline static double phi(const vgl_vector_2d<double>& v)
{
  double dist = v.length();
  return (dist==0.0) ? 0.0 : dist*dist*vcl_log(dist);
}

inline static double eval_depth(const vgl_point_2d<double>& c, 
                                const vnl_vector<double>& w,
                                const vcl_vector<vgl_point_2d<double> >& pts)
{
  double depth = 0.0;
  unsigned int dim = w.size();
  for(unsigned int k=0; k<dim-3; ++k)
    depth += w[k]*phi(c - pts[k]);
  depth += w[dim-3];
  depth += w[dim-2]*c.x();
  depth += w[dim-1]*c.y();
  return depth;
}

inline static vnl_vector<double>
find_weights(const vcl_vector<vgl_point_2d<double> >& points,
             const vcl_vector<double>& depths)
{
  unsigned int dim = points.size()+3;
  vnl_matrix<double> M(dim,dim,0.0);
  vnl_vector<double> h(dim,0.0);
  for(unsigned int i=0; i<dim-3; ++i){
    M(i,i) = 0.0;
    h[i] = depths[i];
    for(unsigned int j=i+1; j<dim-3; ++j){
      M(i,j) = M(j,i) = phi(points[i] - points[j]);
    }
    M(i,dim-3) = M(dim-3,i) = 1.0;
    M(i,dim-2) = M(dim-2,i) = points[i].x();
    M(i,dim-1) = M(dim-1,i) = points[i].y();
  }
  vnl_svd<double> svd_M(M);
  return svd_M.solve(h);
}


//: Execute the process
bool
dbkpr_interp_depth_process::execute()
{
  // cast the storage class
  dbkpr_corr3d_storage_sptr frame_corr3d;
  frame_corr3d.vertical_cast(input_data_[0][0]);
  int frame = frame_corr3d->frame();

  // get the data
  vcl_vector<dbdet_keypoint_corr3d_sptr> all_corr3d = frame_corr3d->correspondences();
  const vpgl_proj_camera<double> &camera = *(frame_corr3d->get_camera());
  unsigned int ni = frame_corr3d->ni();
  unsigned int nj = frame_corr3d->nj();

  bpro1_filepath file;
  int samp_rate=0;
  parameters()->get_value( "-file" , file );
  parameters()->get_value( "-samp" , samp_rate );
  vcl_stringstream filename;
  filename << file.path << vcl_setfill('0') <<vcl_setw(5) << frame <<".dmp";
  vil_image_view<float> depthmap(ni/samp_rate+1,nj/samp_rate+1);
  depthmap.fill(0.0);

  vcl_vector<vgl_point_2d<double> > points;
  vcl_vector<double> depths;
  double max_d = 0.0;
  double min_d = vcl_numeric_limits<double>::infinity();
  for(unsigned int i=0; i<all_corr3d.size(); ++i)
  {
    //if(points.size() > 100) break;
    // only use points originally found in this frame
    if(all_corr3d[i]->in_view(frame)){
      vgl_homg_point_2d<double> pt = camera(vgl_homg_point_3d<double>(*all_corr3d[i]));
      vgl_point_2d<double> kp(*all_corr3d[i]->in_view(frame));
      if(vgl_distance(kp, vgl_point_2d<double>(pt)) > 1.0) continue;
      points.push_back(pt);
      depths.push_back(pt.w());
      if(pt.w() > max_d) max_d = pt.w();
      if(pt.w() < min_d) min_d = pt.w();
    }
  }
  
  
  // remove duplicates that could cause singularities
  vcl_vector<vgl_point_2d<double> > points_u;
  vcl_vector<double> depths_u;
  for(unsigned int i=0; i<points.size(); ++i){
    bool unique = true;
    for(unsigned int j=i+1; j<points.size(); ++j){
      if(vgl_distance(points[i],points[j]) < 1e-8){
        unique = false;
        break; 
      }
    } 
    if(unique){
      points_u.push_back(points[i]);
      depths_u.push_back(depths[i]);
    } 
  }

  vcl_cout << "solving"<< vcl_endl;
  vnl_vector<double> w = find_weights(points_u,depths_u);

  vcl_vector<vcl_pair<double,int> > ind_weights;
  for(int i=0; i<(int)w.size()-3; ++i)
    ind_weights.push_back(vcl_pair<double,int>(vcl_abs(w[i]),i));

  vcl_sort(ind_weights.begin(), ind_weights.end(), vcl_less<vcl_pair<double,int> >());

  // remove extreme points
  points.clear();
  depths.clear();
  for(int i=0; i<int(ind_weights.size()*0.75); ++i){
    int idx = ind_weights[i].second;
    points.push_back(points_u[idx]);
    depths.push_back(depths_u[idx]);
  }

  vcl_cout << "solving again"<< vcl_endl;
  w = find_weights(points,depths);
  
  vcl_cout << "evaluating" << vcl_endl;
  
  for(unsigned int i=0; i<=ni; i+=samp_rate)
    for(unsigned int j=0; j<=nj; j+=samp_rate)
      depthmap(i/samp_rate,j/samp_rate) = (float)eval_depth(vgl_point_2d<double>(i,j),w,points);
      
  vcl_ofstream os(filename.str().c_str(), vcl_ios::out | vcl_ios::binary);
  unsigned int size[] = {ni/samp_rate+1, nj/samp_rate+1};
  vcl_cout << size[0] <<" x "<<size[1]<<vcl_endl;
  os.write((const char*)size, (unsigned long)2*sizeof(unsigned int));
  os.write((const char*)depthmap.top_left_ptr(), (unsigned long)(size[0]*size[1]*sizeof(float)));
  os.close();
  

  vil_math_truncate_range(depthmap,(float)min_d,(float)max_d);
  vil_image_resource_sptr rsc = vil_new_image_resource_of_view(depthmap);
  vil_image_view<vxl_byte> viewable = vil_convert_stretch_range(vxl_byte(0), rsc->get_view());
  vil_math_scale_and_offset_values(viewable,-1.0,255); // negate for viewing
  // pack up the results into storage classes
  vidpro1_image_storage_sptr out_storage = vidpro1_image_storage_new();
  out_storage->set_image(vil_new_image_resource_of_view(viewable));
  output_data_[0].push_back(out_storage);

  return true;
}


bool
dbkpr_interp_depth_process::finish()
{
  return true;
}





