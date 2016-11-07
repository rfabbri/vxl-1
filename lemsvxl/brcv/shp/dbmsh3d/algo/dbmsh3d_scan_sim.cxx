//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/algo/dbmsh3d_scan_sim.cxx
// 3D scan simulator that takes a mesh and produces the simulated scan files.
//-------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_random.h>
#include <vul/vul_printf.h>
#include <vul/vul_sprintf.h>
#include <dbnl/dbnl_min_max_3.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/algo/dbmsh3d_scan_sim.h>
#include <dbmsh3d/algo/dbmsh3d_sg3pi_algo.h>
#include <dbmsh3d/algo/dbmsh3d_ray_intersect.h>

//: 3D scan simulator parameters.

//: SCAN_SIM_TYPE
typedef enum {
  SCAN_SIM_BOGUS = 0,
  FS_TYPE_CYLINDER = 1,
} SCAN_SIM_TYPE;

#define SCAN_CENTER_D_BOX_RATIO     2

//Seed for vnl_random
vnl_random ptb_rand;

//: The 3D scan simulator that takes a mesh M and
//  produces the simulated scan files (*.3PI) according to
//  specified parameters.
//  Simulated scan ray is repeatly intersected with the given mesh.
bool scan_sim (dbmsh3d_mesh* M, 
               const int n_view, const int view_min, const int view_max,
               const float inter_sl_ratio, const float intra_sl_ratio, 
               const int minY, const int maxY, const int minX, const int maxX,
               const float ptb, const char* prefix)
{
  vul_printf (vcl_cout, "scan_sim(): M %u points, %u faces.\n",
              M->vertexmap().size(), M->facemap().size());
  vul_printf (vcl_cout, "  Parameters: file prefix: %s\n", prefix);
  vul_printf (vcl_cout, "    %d simulated scan views, where view %d to %d will be processed.\n",
              n_view, view_min, view_max);
  vul_printf (vcl_cout, "    inter_sl_ratio %f (minY %d, maxY %d),\n",
              inter_sl_ratio, minY, maxY);
  vul_printf (vcl_cout, "    intra_sl_ratio %f (minX %d, maxX %d).\n",
              intra_sl_ratio, minX, maxX);

  //1) Detect the center and bounding box of mesh M.
  vgl_box_3d<double> bbox;
  bool result = detect_bounding_box (M, bbox);
  if (result == false)
    return false;
  vgl_point_3d<double> O;
  result = detect_geom_center (M, O);
  assert (result);

  //Detect the avg. sampling dist of M for intra- and inter- scanline dist.
  const double avg_samp_dist = M->get_avg_edge_len_from_F ();
  //intra- scanline sampling distance.
  const double delta_y = avg_samp_dist * intra_sl_ratio;
  //inter- scanline sampling distance.
  const double delta_x = avg_samp_dist * inter_sl_ratio;
  const double max_noise = delta_y * ptb / 100;
  vul_printf (vcl_cout, "    max. perturbation (noise): %lf (%2.2f %% of intra_dist %f).\n", 
              max_noise, ptb, delta_y);
  ptb_rand.reseed (7654321);

  //Repeat scan simulation for n views.
  //Use the center as origin O.
  //Use dOC as distance from scanner to O.
  double dOC = dbnl_max3 (bbox.width(), bbox.height(), bbox.depth());
  dOC *= SCAN_CENTER_D_BOX_RATIO;

  //number of intra- scanline samples: [minY to maxY]
  //number of inter- scanline samples: [minX to maxX]
  scan_sim_cylinder (M, n_view, view_min, view_max, O, dOC, 
                     delta_y, delta_x, minY, maxY, minX, maxX, max_noise, prefix);
  return true;
}

//: 3D scan simulator of the cylinder type.
void scan_sim_cylinder (dbmsh3d_mesh* M, 
                        const int n_view, const int view_min, const int view_max,
                        const vgl_point_3d<double>& O, const double dOC,
                        const double delta_y, const double delta_x,
                        const int minY, const int maxY, const int minX, const int maxX,
                        const double max_noise, const char* prefix)
{
  //Repeat scan simulator for n views, each with angle theta.
  assert (0 <= view_min);
  assert (view_min <= view_max);
  assert (view_max < n_view);
  for (int i=view_min; i<=view_max; i++) {
    double theta = vnl_math::pi * 2 * i / n_view;
    //Computed the scan direction D = (d cos_theta. d sin_theta).
    const vgl_vector_3d<double> D (vcl_cos (theta), vcl_sin (theta), 0);
    //Compute the simulated scanner position C.
    const vgl_point_3d<double> C = O + D * dOC;

    //Simulate scan for the view i
    dbmsh3d_sg3pi* sg3pi = scan_sim_view (M, C, theta, delta_y, delta_x, minY, maxY, minX, maxX, max_noise);

    //Save to 3pi file.
    vcl_string file_3pi = vul_sprintf ("%s_scan_%02d.3pi", prefix, i);
    dbmsh3d_save_sg3pi (sg3pi, file_3pi.c_str());

    //Save the alignment file for this 3pi (to transform it back to original object).
    vcl_string file_af = vul_sprintf ("%s_scan_%02d.txt", prefix, i);
    save_scan_sim_af_file (O, dOC, theta, file_af.c_str());

    delete sg3pi;
  }
}

//: Simulate scan for the view from center C and direction D
//  with parameters:
//    intra-scanline dist: delta_y, inter-scanline dist: delta_x.
//    # scanlines [minY to maxY], # of points in scanline [minX to maxX]
//
dbmsh3d_sg3pi* scan_sim_view (dbmsh3d_mesh* M, 
                              const vgl_point_3d<double>& C, const double theta,
                              const double delta_y, const double delta_x,
                              const int minY, const int maxY, const int minX, const int maxX,
                              const double max_noise)
{
  vul_printf (vcl_cout, "scan_sim_view():\n");
  dbmsh3d_sg3pi* sg3pi = new dbmsh3d_sg3pi;

  const vgl_vector_3d<double> minus_D (-vcl_cos (theta), -vcl_sin (theta), 0);
  const int intensity = 5000;

  //The scan process is simulated by repeatly 
  //intersecting each scan ray to the mesh, then
  //picking the closest intersection of each ray.
  assert (minY <= maxY);
  assert (minX <= maxX);
  for (int i = minY; i <= maxY; i++) {
    //Add a scan line.
    vgl_vector_3d<double> Vi (- delta_y * i * vcl_sin(theta), delta_y * i * vcl_cos(theta), 0);
    vgl_point_3d<double> Si = C + Vi;     
    vcl_vector<dbmsh3d_sg3pi_pt*> scanline;
    scanline.clear ();
    sg3pi->add_scanline (scanline);
    vul_printf (vcl_cout, "scanline %d: ", i-minY);

    for (int j = minX; j <= maxX; j++) {
      //For each scan point Six and scan ray direction D, compute the closest mesh intersection.
      vgl_point_3d<double> Six (Si.x(), Si.y(), Si.z() + j*delta_x);
      ///vgl_point_3d<double> Six (Si.x() + j*delta_x, Si.y(), Si.z());

      //Intersect the scan ray to mesh M to find the scan object point iP.
      vgl_point_3d<double> iP;
      dbmsh3d_face* F = intersect_ray_mesh_all (Six, minus_D, M, iP);
      if (F) { //Projection of scan point on object found.
        //Add a scan data point to sg3pi.
        //Convert the data point iP to the local scan coordinate (j*delta_x, i*delta_y, depth).
        double x = j * delta_x;
        double y = i * delta_y;
        double z = vgl_distance (iP, Six);
        //Add random noise to the depth to simulate sensing error.
        z += max_noise * ptb_rand.drand32 (-1, 1);
        dbmsh3d_sg3pi_pt* scanpt = new dbmsh3d_sg3pi_pt (x, y, z, intensity, j-minX);
        sg3pi->add_scanpt (scanpt);
        vul_printf (vcl_cout, "%d ", j-minX);

        ///#if DBMSH3D_DEBUG>4
        //Brute-forcely validate the transformation between (x,y,z) and (u,v,w) = iP
        ///double u = (dOC - z) * cos(theta) - y * sin(theta) + Ou;
        ///double v = (dOC - z) * sin(theta) + y * cos(theta) + Ov;
        ///double w = x + Ow;
        ///#endif
      }
    }
    vul_printf (vcl_cout, "\n");
  }

  return sg3pi;
}

bool save_scan_sim_af_file (const vgl_point_3d<double>& O, 
                            const double& dOC, const double& theta, 
                            const char* file_af)
{
  // [u]    [ 0 -sin_theta -cos_theta ] [x]   [ Ou + dOC cos_theta ]
  // [v]  = [ 0  cos_theta -sin_theta ] [y] + [ Ov + dOC sin_theta ]
  // [w]    [ 1      0          0     ] [z]   [        Ow          ]
  vnl_matrix_fixed<double,4,4> H44;
  double sin_theta = vcl_sin (theta);
  double cos_theta = vcl_cos (theta);

  H44(0,0) = 0;
  H44(0,1) = - sin_theta;
  H44(0,2) = - cos_theta;
  H44(0,3) = O.x() + dOC * cos_theta;
  H44(1,0) = 0;
  H44(1,1) = cos_theta;
  H44(1,2) = - sin_theta;
  H44(1,3) = O.y() + dOC * sin_theta;
  H44(2,0) = 1;
  H44(2,1) = 0;
  H44(2,2) = 0;
  H44(2,3) = O.z();
  H44(3,0) = 0;
  H44(3,1) = 0;
  H44(3,2) = 0;
  H44(3,3) = 1;
  vgl_h_matrix_3d<double> H;
  H.set (H44);
  return dbmsh3d_write_xform_file (file_af, H);
}
