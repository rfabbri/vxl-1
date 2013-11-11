// This is ntrinh/dbsk2d_cmd/dbsk2d_cmd_utils.cxx
//:
// \file

#include "dbsk2d_cmd_utils.h"

#include <dbsk2d/dbsk2d_file_io.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>

#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_conic_2d_sptr.h>


//: A point on a circle
bool dbsk2d_cmd_generate_random_bnd(const vcl_string& bnd_file,
                                    int num_lines,
                                    int num_arcs,
                                    int image_width,
                                    int image_height)
{
  // Generate random points
  vnl_random random;
  random.reseed();

  vcl_vector<vsol_spatial_object_2d_sptr > vsol_list;

  // add a whole bunch of lines
  for (int i =0; i < num_lines; ++i)
  {
    

    double x1 = random.drand32(image_width * 0.1, image_width * 0.9);
    double y1 = random.drand32(image_height * 0.1, image_height * 0.9);

    double x2 = random.drand32(image_width * 0.1, image_width * 0.9);
    double y2 = random.drand32(image_height * 0.1, image_height * 0.9);

    vgl_point_2d<double > p1(x1, y1);
    vgl_point_2d<double > p2(x2, y2);

    vsol_list.push_back(new vsol_line_2d(p1, p2));
    
  }

  // add a whole bunch of arcs
  for (int i =0; i < num_arcs; ++i)
  {
      double x1 = random.drand32(image_width * 0.1, image_width * 0.9);
    double y1 = random.drand32(image_height * 0.1, image_height * 0.9);

    double x2 = random.drand32(image_width * 0.1, image_width * 0.9);
    double y2 = random.drand32(image_height * 0.1, image_height * 0.9);

    vgl_point_2d<double > p1(x1, y1);
    vgl_point_2d<double > p2(x2, y2);

    // random curvature
    double len = (p2-p1).length();
    double center_to_chord = random.drand32(1, image_height * 0.7);

    // determine position of the center
    vgl_vector_2d<double > t = normalized(p2 - p1);
    vgl_vector_2d<double > n(-t.y(), t.x());

    // center of arc
    vgl_point_2d<double > c = midpoint(p1, p2) - center_to_chord * n;
    double r = (c-p1).length();


    // parameter of conic (circle)
    double conic_a = 1;
    double conic_b = 0;
    double conic_c = 1;
    double conic_d = -2*c.x();
    double conic_e = -2*c.y();
    double conic_f = c.x()*c.x()+c.y()*c.y()-r*r;

    vsol_conic_2d_sptr circ_arc = new vsol_conic_2d(conic_a, conic_b,
      conic_c, conic_d, conic_e, conic_f);
    circ_arc->set_p0(new vsol_point_2d(p1));
    circ_arc->set_p1(new vsol_point_2d(p2));

    vsol_list.push_back(circ_arc->cast_to_spatial_object());
  
  }
  

  // write boundary to file
  dbsk2d_file_io::save_bnd_v3_0(bnd_file, vsol_list);
  
  return true;
}






