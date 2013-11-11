//: 
// \file     geom_index_example.cxx
// \brief    
// \         
// \author   Gamze Tunali
// \date     Feb 27, 2006
// 
#include <geom/geom_index_structure.h>
#include <geom/geom_volume.h>
#include <geom/geom_volume_sptr.h>
#include <biob/biob_explicit_worldpt_roster.h>

#include <bsta/bsta_histogram.h>
#include <vsol/vsol_orient_box_3d.h>
#include <vsol/vsol_orient_box_3d_sptr.h>
#include <vsol/vsol_cylinder.h>
#include <vsol/vsol_cylinder_sptr.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cross.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vnl/vnl_quaternion.h>

vnl_quaternion<double> findRot(vnl_vector<double> & from, vnl_vector<double> & to)
{
  from.normalize();
  to.normalize();

  double dot =  dot_product(from, to);//from.dot(to);
  vnl_vector<double> crossvec = vnl_cross_3d(from, to); //from.cross(to);
  double crosslen = crossvec.magnitude();

  if (crosslen == 0.0f) { // Parallel vectors
    // Check if they are pointing in the same direction.
    if (dot > 0.0f) {
      return vnl_quaternion<double> (0.0f, 0.0f, 0.0f, 1.0f);
      //this->setValue(0.0f, 0.0f, 0.0f, 1.0f);
    }
    // Ok, so they are parallel and pointing in the opposite direction
    // of each other.
    else {
      // Try crossing with x axis.
      //SbVec3f t = from.cross(SbVec3f(1.0f, 0.0f, 0.0f));
      vnl_vector<double> x_dir(3); x_dir[0]=1.; x_dir[1]=0.; x_dir[2]= 0.;
      vnl_vector<double> t = vnl_cross_3d(from, x_dir);
      // If not ok, cross with y axis.
      //if(t.length() == 0.0f) t = from.cross(SbVec3f(0.0f, 1.0f, 0.0f));
      vnl_vector<double> y_dir(3); y_dir[0]=0.; y_dir[1]=1.; y_dir[2]= 0.;
      if(t.magnitude() == 0.0f) t = vnl_cross_3d(from, y_dir);
      t.normalize();
      //this->setValue(t[0], t[1], t[2], 0.0f);
      return vnl_quaternion<double> (t[0], t[1], t[2], 0.0f);
    }
  }
  else { // Vectors are not parallel
    crossvec.normalize();
    // The fabs() wrapping is to avoid problems when `dot' "overflows"
    // a tiny wee bit, which can lead to sqrt() returning NaN.
    crossvec *= (float)sqrt(0.5f * fabs(1.0f - dot));
    // The fabs() wrapping is to avoid problems when `dot' "underflows"
    // a tiny wee bit, which can lead to sqrt() returning NaN.
    //this->setValue(crossvec[0], crossvec[1], crossvec[2],
    //               (float)sqrt(0.5 * fabs(1.0 + dot)));
    return vnl_quaternion<double> (crossvec[0], crossvec[1], crossvec[2],
      (float)sqrt(0.5 * fabs(1.0 + dot)));
  }
}

vcl_vector<vsol_cylinder> read_cylinders(vcl_string file_name, int& num)
{
  int ver;
  double strength;
  vsol_cylinder cylinder;
  vcl_vector<vsol_cylinder> cylinders;

  vsl_b_ifstream istream(file_name.c_str());
  vsl_b_read(istream, ver);
  vsl_b_read(istream, num);
  
  for (int i=0; i < num; i++) {
    vsl_b_read(istream, strength);
    /*if (min_strength > strength)
      min_strength = strength;
    if (max_strength < strength)
      max_strength = strength;*/

    //vcl_cout << strength << " min=" << min_strength << " max=" << max_strength << vcl_endl;
    cylinder.b_read(istream);
    cylinders.push_back(cylinder);//(vcl_pair<vsol_cylinder, double> (cylinder,strength));
    vcl_cout << cylinder << vcl_endl;
  }
  return cylinders;
}

void write_cylinders(vcl_string o_file, vcl_vector<vsol_cylinder_sptr> cylinders)
{
  vsl_b_ofstream stream(o_file);
  // write the version number
  vsl_b_write(stream, (int) 1);

  // write the number of cylinders
  vsl_b_write(stream, (int) cylinders.size());

  for (unsigned int i=0; i<cylinders.size(); i++){
    vsol_cylinder_sptr cyl = cylinders[i];
    // first write the strength
    vsl_b_write(stream, (double) 100.0); //(double) strengths[i]);
    cyl->b_write(stream);
  }
  stream.close();
}

using namespace std;
int main(int argc, char* argv[]) {

  vcl_string edge_file=  "";
  vcl_string centerline_file = "";
  vcl_string output_file = "";

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    vcl_string arg (argv[i]);
    vcl_cout << arg << vcl_endl;
    if (arg == vcl_string ("-c")) { centerline_file = vcl_string(argv[++i]);}
    else if (arg == vcl_string ("-e")) { edge_file = vcl_string (argv[++i]); }
    else if (arg == vcl_string ("-o")) { output_file = vcl_string (argv[++i]); }
    else
    {
      vcl_cout << "Usage: " << argv[0] << "[-c centerline] [-e edge] [-o output]" << vcl_endl;
      throw -1;
    }
  }

  // create the roster and add points
  biob_explicit_worldpt_roster * roster = new biob_explicit_worldpt_roster();
  
  int edge_num, cyl_num;
  vcl_vector<vsol_cylinder> edge_cylinders = read_cylinders(edge_file, edge_num);
  vcl_vector<vsol_cylinder> center_cylinders = read_cylinders(centerline_file, cyl_num);
  vgl_box_3d<double> edge_box;

  for (unsigned int i=0; i<edge_cylinders.size(); i++) {
    vcl_cout << edge_cylinders[i] << vcl_endl;
    roster->add_point(edge_cylinders[i].center());
    edge_box.add(edge_cylinders[i].center());
  }

  vcl_cout << edge_box << vcl_endl;

  // add the points out of edge detection
  geom_index_structure index_structure(roster, 1.);

  vcl_vector<vsol_cylinder_sptr> final_cylinders;
  for (unsigned int i=0; i<center_cylinders.size(); i++) {
    vcl_cout << center_cylinders[i] << vcl_endl;
    vsol_cylinder cylinder = center_cylinders[i];
    vnl_vector<double> from(3);
    from[0] = 0; from[1] = 0; from[2]=1;
    vnl_vector<double> to(3);
    to[0] = cylinder.orientation().x(); 
    to[1] = cylinder.orientation().y(); 
    to[2] = cylinder.orientation().z();
    vnl_quaternion<double> dir = findRot(from, to);
    vgl_box_3d<double> box(cylinder.center(),
             cylinder.length()*50, cylinder.length()*50, cylinder.length(),
             vgl_box_3d<double>::centre); 

    vgl_orient_box_3d<double> obb(box, dir);
    vsol_volume_3d_sptr v = new vsol_orient_box_3d(obb);
    geom_probe_volume_sptr volume = new geom_volume(v);
    biob_worldpt_index_enumerator_sptr list = index_structure.enclosed_by(volume);
    vcl_vector<double> dist;
    double min=1e29, max=-1e29;
    int c=0;
    while (list->has_next()) {
      biob_worldpt_index index = list->next();
      vgl_point_3d<double> p = roster->point(index);
      // find the distance between the point 
      double d = vgl_distance(cylinder.center(), p);
      if (min > d)
        min = d;
      if (max < d)
        max=d;

      dist.push_back(d);
    }
    bsta_histogram<double> h(min, max, dist);
    vcl_cout << "min=" << h.min() << " max=" <<  h.max() << vcl_endl;
    double h_max =-1e29;
    double max_index = -1;
    for (unsigned int h_i=0; h_i<h.nbins(); h_i++) {
      if (h.counts(h_i) > h_max) {
        h_max = h.counts(h_i);
        max_index = h_i;
      }
    }
    //h.print();
    if (max_index != -1) {
       vsol_cylinder_sptr cyl = new vsol_cylinder(cylinder.center(), h.counts(static_cast<unsigned>(max_index)),
        cylinder.length(), cylinder.orientation());
               
      final_cylinders.push_back(cyl);
    }
  }

  // create cylinder and write to binary stream
  write_cylinders(output_file, final_cylinders);

}

