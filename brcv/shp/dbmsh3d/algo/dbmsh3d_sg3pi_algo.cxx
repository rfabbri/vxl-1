//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/algo/dbmsh3d_sg_scan_algo.cxx
//-------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_vector_fixed.h>

#include <dbgl/dbgl_dist.h>
#include <dbgl/dbgl_triangle.h>
#include <dbgl/algo/dbgl_distance.h>
#include <dbgl/algo/dbgl_curve_smoothing.h>
#include <dbmsh3d/algo/dbmsh3d_sg3pi_algo.h>

// #################################################################
//    SHAPE GRABBER .3PI RAW SCAN FILE I/O
// #################################################################

bool dbmsh3d_load_sg3pi (dbmsh3d_sg3pi* sg3pi, const char* file)
{
  vcl_ifstream  in;
  vcl_string    linestr;
  in.open (file);
  assert (sg3pi->data().size() == 0);
  unsigned int n_scanlines = 0;
  unsigned int n_scanpts = 0;
  double x, y, z;
  int intensity, np;
  vcl_vector<dbmsh3d_sg3pi_pt*> scanline;

  if (in == NULL) {
    return false;
  }

  vul_printf (vcl_cout, "  loading %s...\n", file);

  //Main loop of file reading
  while (in) {
    linestr.clear();
    vcl_getline (in, linestr);
    //vcl_cout << linestr << vcl_endl;

    //skip empty lines
    if (linestr.length() == 0)
      continue;

    //count scan lines.
    if (vcl_strncmp (linestr.c_str(), "#:Profile:", 10) == 0) {
      n_scanlines++;
      scanline.clear();
      sg3pi->add_scanline (scanline);
    }

    //skip comments
    if (vcl_strncmp (linestr.c_str(), "#", 1) == 0)
      continue;

    //put points into the scan data structure.
    vcl_sscanf (linestr.c_str(), "%lf %lf %lf %d %d", &x, &y, &z, &intensity, &np);
    n_scanpts++;

    dbmsh3d_sg3pi_pt* scanpt = new dbmsh3d_sg3pi_pt (x, y, z, intensity, np);
    sg3pi->add_scanpt (scanpt);
  }

  in.close();
  vul_printf (vcl_cout, "  %d points loaded from %s.\n", n_scanpts, file);
  return true;
}

bool dbmsh3d_save_sg3pi (dbmsh3d_sg3pi* sg3pi, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    vul_printf (vcl_cout, "  can't open output .3PI file %s\n", file);
    return false; 
  }
  vul_printf (vcl_cout, "  saving %s : %u scanlines ...\n", 
               file, sg3pi->data().size());

  vcl_fprintf (fp, "#3PI file Copyright(C) 1997-2001 ShapeGrabber Incorporated\n");

  int max_pn = sg3pi->get_max_pn ();
  vcl_fprintf (fp, "#:Number of Points per Profiles: %d\n", max_pn);
  vcl_fprintf (fp, "#:Number of Profiles: %d\n", sg3pi->data().size());
  vcl_fprintf (fp, "#:Pose Transformation:\n");
  vcl_fprintf (fp, "#                      1.000000 0.000000 0.000000 0.000000\n");
  vcl_fprintf (fp, "#                      0.000000 1.000000 0.000000 0.000000\n");
  vcl_fprintf (fp, "#                      0.000000 0.000000 1.000000 0.000000\n");
  vcl_fprintf (fp, "#                      0.000000 0.000000 0.000000 1.000000\n");

  for (unsigned int i=0; i<sg3pi->data().size(); i++) {
    vcl_fprintf (fp, "#:Profile: %d\n", i);

    for (unsigned int j=0; j<sg3pi->data()[i].size(); j++) {
      dbmsh3d_sg3pi_pt* SP = sg3pi->data()[i][j];
      vcl_fprintf (fp, "%.16lf %.16lf %.16lf %d %d\n", 
                   SP->pt().x(), SP->pt().y(), SP->pt().z(), SP->intensity(), SP->pn());
    }
  }

  vcl_fclose (fp);
  vul_printf (vcl_cout, "  done.\n");
  return true;
}

// #################################################################
//    SMOOTHING the SHAPE GRABBER .3PI RAW SCANS
// #################################################################

//: Smooth each scan line with thresholding to skip far away point in smoothing.
void dcs_smooth_scanlines_3pi (dbmsh3d_sg3pi* sg3pi, const unsigned int nsteps,
                               const float DCS_psi, const float DCS_th_ratio)
{
  vul_printf (vcl_cout, "dcs_smooth_scanlines_3pi(): %u scan lines, nsteps = %d\n", 
               sg3pi->data().size(), nsteps);
  vul_printf (vcl_cout, "\tDCS_psi: %f, DCS_th_ratio: %f.\n", DCS_psi, DCS_th_ratio);

  const double intra_sl_kth = sg3pi->intra_sl_dist() * DCS_th_ratio;
  vcl_vector<vgl_point_3d<double> > scanline;

  for (unsigned int i=0; i<sg3pi->data().size(); i++) {
    assert (scanline.size() == 0);
    for (unsigned int j=0; j<sg3pi->data(i).size(); j++)
      scanline.push_back (sg3pi->data(i,j)->pt());

    //Smooth each scanline with thresholding
    dbgl_curve_shorten_3d_th (scanline, intra_sl_kth, DCS_psi, nsteps);
    
    //Put the smoothed scanline back.
    for (unsigned int j=0; j<sg3pi->data(i).size(); j++)
      sg3pi->get_data(i,j)->set_pt (scanline[j]);
    scanline.clear();
  }  
}


//: Build curve of consecutive points with same index across scanlines.
//  Smooth each curve using dbgl_curve_smooth
void dcs_smooth_across_scanlines_3pi (dbmsh3d_sg3pi* sg3pi, const unsigned int nsteps,
                                      const float DCS_psi, const float DCS_th_ratio)
{
  unsigned int i, j, count, total;
  vul_printf (vcl_cout, "dcs_smooth_across_scanlines_3pi(): %u scan lines, nsteps = %d\n", 
              sg3pi->data().size(), nsteps);
  vul_printf (vcl_cout, "\tDCS_psi: %f, DCS_th_ratio: %f.\n", DCS_psi, DCS_th_ratio);

  //Build the points across scanlines into curves into another array,
  //  (transpose of the original array)
  //# curves: max number of point indicex: get_max_pn()
  //  where each curve can be broken into many sub-curves.
  //# length of curve: # profiles: sg3pi->data().size()
  vcl_vector <vcl_vector<dbmsh3d_sg3pi_pt*> > datat;
  const int maxpn = sg3pi->get_max_pn ();
  const int maxlen = sg3pi->data().size();
  datat.resize (maxpn);
  for (j=0; j<datat.size(); j++) {
    datat[j].resize (maxlen);
  }
  
  for (j=0; j<datat.size(); j++) {
    for (i=0; i<datat[j].size(); i++)
      datat[j][i] = NULL;
  }

  for (i=0; i<sg3pi->data().size(); i++) {
    for (j=0; j<sg3pi->data()[i].size(); j++) {
      datat[j][i] = sg3pi->data()[i][j];
    }
  }
  
  //Now go to the datat array and smooth each curve with same index.  
  for (j=0; j<datat.size(); j++) {
    //datat[j] should contain points with the same index.
    if (datat[j].size() == 0)
      continue;

    //try just put all points into a curve and smooth it (ignoring all gaps).
    vcl_vector<vgl_point_3d<double> > curve;
    curve.clear();

    count=0; 
    for (i=0; i<datat[j].size(); i++) {
      if (datat[j][i]) {
        curve.push_back (datat[j][i]->pt());
        count++;
      }
    }
    total = count;
    if (total < 3)
      continue;

    //smooth the curve.
    dbgl_curve_shorten_3d (curve, DCS_psi, nsteps);

    //put smoothed points back to the curve of datat[j]
    count=0; 
    for (i=0; i<datat[j].size(); i++) {
      if (datat[j][i]) {
        datat[j][i]->set_pt (curve[count]);
        count++;
      }
    }
    assert (count == total);
  }

  for (j=0; j<datat.size(); j++)
    datat[j].clear();
  datat.clear();

}

void dcs_smooth_scanlines_2dirs_3pi (dbmsh3d_sg3pi* sg3pi, 
                                     const float DCS_psi, const float DCS_th_ratio,
                                     const int n_intra, const int n_inter)
{
  unsigned int i, j, count, total;
  
  vul_printf (vcl_cout, "dcs_smooth_scanlines_2dirs_3pi(): %u scan lines\n", 
              sg3pi->data().size());
  vul_printf (vcl_cout, "\tDCS_psi: %f, DCS_th_ratio: %f.\n", DCS_psi, DCS_th_ratio);
  vul_printf (vcl_cout, "\tn_intra: %d, n_inter: %d.\n", n_intra, n_inter);

  const double intra_sl_kth = sg3pi->intra_sl_dist() * DCS_th_ratio;
  const double inter_sl_kth = sg3pi->inter_sl_dist() * DCS_th_ratio;
  //The stepsize is 1/4 of the smaller vector length that 
  //the resulting mesh will not cross itself.
  vcl_vector<vgl_point_3d<double> > scanline;

  //1) Smooth each scanline for n_intra times.
  for (unsigned int i=0; i<sg3pi->data().size(); i++) {
    assert (scanline.size() == 0);
    for (unsigned int j=0; j<sg3pi->data(i).size(); j++)
      scanline.push_back (sg3pi->data(i,j)->pt());

    //Smooth each scanline with thresholding
    dbgl_curve_shorten_3d_th (scanline, intra_sl_kth, DCS_psi, n_intra);
    
    //Put the smoothed scanline back.
    for (unsigned int j=0; j<sg3pi->data(i).size(); j++)
      sg3pi->get_data(i,j)->set_pt (scanline[j]);
    scanline.clear();
  }  

  //Build the points across scanlines into curves into another array,
  //  (transpose of the original array)
  //# curves: max number of point indicex: get_max_pn()
  //  where each curve can be broken into many sub-curves.
  //# length of curve: # profiles: sg3pi->data().size()
  vcl_vector <vcl_vector<dbmsh3d_sg3pi_pt*> > datat;
  const int maxpn = sg3pi->get_max_pn ();
  const int maxlen = sg3pi->data().size();
  datat.resize (maxpn);
  for (j=0; j<datat.size(); j++) {
    datat[j].resize (maxlen);
  }
  
  for (j=0; j<datat.size(); j++) {
    for (i=0; i<datat[j].size(); i++)
      datat[j][i] = NULL;
  }

  for (i=0; i<sg3pi->data().size(); i++) {
    for (j=0; j<sg3pi->data()[i].size(); j++) {
      datat[j][i] = sg3pi->data()[i][j];
    }
  }
  
  //Now go to the datat array and smooth each curve with same index.  
  for (j=0; j<datat.size(); j++) {
    //datat[j] should contain points with the same index.
    if (datat[j].size() == 0)
      continue;

    //try just put all points into a curve and smooth it (ignoring all gaps).
    vcl_vector<vgl_point_3d<double> > curve;
    curve.clear();

    count=0; 
    for (i=0; i<datat[j].size(); i++) {
      if (datat[j][i]) {
        curve.push_back (datat[j][i]->pt());
        count++;
      }
    }
    total = count;
    if (total < 3)
      continue;
    
    //Smooth each scanline with thresholding
    dbgl_curve_shorten_3d_th (curve, inter_sl_kth, DCS_psi, n_inter);

    //put smoothed points back to the curve of datat[j]
    count=0; 
    for (i=0; i<datat[j].size(); i++) {
      if (datat[j][i]) {
        datat[j][i]->set_pt (curve[count]);
        count++;
      }
    }
    assert (count == total);
  }

  for (j=0; j<datat.size(); j++)
    datat[j].clear();
  datat.clear();
}

//: only smooth the range z(x,y)
bool dbgl_curve_shorten_z (vcl_vector<dbmsh3d_sg3pi_pt*>& scanline, 
                           const float psi, const unsigned int nsteps)
{
  unsigned int n = scanline.size();
  if (n < 3)
    return false;

  vcl_vector<vgl_point_3d<double> > cs;
  cs.resize(n);

  cs[0] = vgl_point_3d<double> (scanline[0]->pt());
  cs[n-1] = vgl_point_3d<double> (scanline[n-1]->pt());

  for (unsigned int ns=0; ns < nsteps; ++ns) {
    for (unsigned int i=1; i < n-1; ++i) {
      vnl_vector_fixed<double,3> middle_p (scanline[i]->pt().x(), scanline[i]->pt().y(), scanline[i]->pt().z());
      vnl_vector_fixed<double,3> left_p (scanline[i-1]->pt().x(), scanline[i-1]->pt().y(), scanline[i-1]->pt().z());
      vnl_vector_fixed<double,3> right_p (scanline[i+1]->pt().x(), scanline[i+1]->pt().y(), scanline[i+1]->pt().z());

      vnl_vector_fixed<double,3> v1 = left_p - middle_p;
      vnl_vector_fixed<double,3> v2 = right_p - middle_p;

      double nv1 = v1.two_norm();
      double nv2 = v2.two_norm();

      vnl_vector_fixed<double,3> v1n = v1/nv1;
      vnl_vector_fixed<double,3> v2n = v2/nv2;

      vnl_vector_fixed<double,3> nrm;
      if (nv1 < nv2) {
        nrm = (v1 + v2n*nv1) / 4.0;
      } else {
        nrm = (v2 + v1n*nv2) / 4.0;
      }

      vnl_vector_fixed<double,3> cs_i = middle_p + double(psi)*nrm;
      cs[i] = vgl_point_3d<double> (cs_i[0],cs_i[1],cs_i[2]);
    }
    for (unsigned int i=0; i<n; i++)
      //scanline[i]->pt().set (cs[i].x(), cs[i].y(), cs[i].z());
      scanline[i]->pt().set (scanline[i]->pt().x(), scanline[i]->pt().y(), cs[i].z());
  }
  return true;
}

//: Median filter options:
//  option 1: 
//      - pick the closest point per scanline (since the scanline is smoothed),
//        if this point is within the distance threshold.
//      - consider 15 = 7*2+1 scanlines for each point.
//      - distance threshold = 7 * intra_sl_dist.
//
//  option 2: 
//      - pick all points within distance threshold.
//      - distance threshold = 7 * intra_sl_dist.
//
#define KERNEL_SCANLINE_SIZE_1    7 //5
#define KERNEL_DIST_TH_1          7 //5

void apply_median_filter_1 (dbmsh3d_sg3pi* sg3pi)
{
  int i, j, k;

  const double dist_th = KERNEL_DIST_TH_1 * sg3pi->intra_sl_dist();    

  //Temporary result array.
  vcl_vector <vcl_vector<vgl_point_3d<double> > > result;
  result.resize (sg3pi->data().size());
  for (i=0; i<int(sg3pi->data().size()); i++)
    result[i].resize (sg3pi->data(i).size());

  for (i=0; i<int(sg3pi->data().size()); i++) {
    for (j=0; j<int(sg3pi->data(i).size()); j++) {
      dbmsh3d_sg3pi_pt* SP = sg3pi->get_data(i,j);

      vcl_vector<float> kernel;
      kernel.clear();
      
      for (k=-KERNEL_SCANLINE_SIZE_1; k<=KERNEL_SCANLINE_SIZE_1; k++) {
        if (i+k<0 || i+k>=int(sg3pi->data().size()))
          continue;

        //add all qualified points to the kernel
        _add_to_kernel_1 (SP, dist_th, sg3pi->data(i+k), kernel);
      }

      if (kernel.size() == 0)
        continue;

      //find the median depth
      vcl_nth_element (kernel.begin(),
                       kernel.begin() + int(kernel.size()/2), 
                       kernel.end());
      double median_depth = *(kernel.begin() + int(kernel.size()/2));

      //compute new point position using the median depth
      vgl_point_3d<double> pt = SP->pt() + (median_depth - SP->depth()) * sg3pi->Vz();      
      result[i][j].set (pt.x(), pt.y(), pt.z()); ///SP->set_pt (pt);
    }
  }
  
  //Copy result to data_ and clear it.
  for (i=0; i<int(sg3pi->data().size()); i++) {
    for (j=0; j<int(sg3pi->data(i).size()); j++)
      sg3pi->get_data(i,j)->set_pt (result[i][j]);
    result[i].clear();
  }
  result.clear();
}

#define KERNEL_SCANLINE_SIZE_2    5
#define KERNEL_DIST_TH_2          5

void apply_median_filter_2 (dbmsh3d_sg3pi* sg3pi)
{
  int i, j, k;

  const double dist_th = KERNEL_DIST_TH_2 * sg3pi->intra_sl_dist();    

  //Temporary result array.
  vcl_vector <vcl_vector<vgl_point_3d<double> > > result;
  result.resize (sg3pi->data().size());
  for (i=0; i<int(sg3pi->data().size()); i++)
    result[i].resize (sg3pi->data(i).size());

  for (i=0; i<int(sg3pi->data().size()); i++) {
    for (j=0; j<int(sg3pi->data(i).size()); j++) {
      dbmsh3d_sg3pi_pt* SP = sg3pi->get_data(i,j);

      vcl_vector<float> kernel;
      kernel.clear();
      
      for (k=-KERNEL_SCANLINE_SIZE_2; k<=KERNEL_SCANLINE_SIZE_2; k++) {
        if (i+k<0 || i+k>=int(sg3pi->data().size()))
          continue;

        //add all qualified points to the kernel
        _add_to_kernel_2 (SP, dist_th, sg3pi->data(i+k), kernel);
      }

      if (kernel.size() == 0)
        continue;

      //find the median depth
      vcl_nth_element (kernel.begin(),
                       kernel.begin() + int(kernel.size()/2), 
                       kernel.end());
      double median_depth = *(kernel.begin() + int(kernel.size()/2));

      //compute new point position using the median depth
      vgl_point_3d<double> pt = SP->pt() + (median_depth - SP->depth()) * sg3pi->Vz();    
      result[i][j].set (pt.x(), pt.y(), pt.z()); ///SP->set_pt (pt);
    }
  }

  //Copy result to data_ and clear it.
  for (i=0; i<int(sg3pi->data().size()); i++) {
    for (j=0; j<int(sg3pi->data(i).size()); j++)
      sg3pi->get_data(i,j)->set_pt (result[i][j]);
    result[i].clear();
  }
  result.clear();
}

void median_filter_3pi (dbmsh3d_sg3pi* sg3pi, const int option, const int nsteps)
{
  vul_printf (vcl_cout, "median_filter_3pi(): option = %d, %d steps\n", option, nsteps);

  sg3pi->estimate_range_coord ();

  //Apply median filter on z(x,y)
  for (int i=0; i<nsteps; i++) {

    //compute the depth z for each scan point
    sg3pi->compute_range ();

    if (option == 1)
      apply_median_filter_1 (sg3pi);
    else if (option == 2)
      apply_median_filter_2 (sg3pi);
  }
}

//Points far away from the kernel_th is not considered in the smoothing kernel.
void gaussian_smooth_scanlines_3pi (dbmsh3d_sg3pi* sg3pi, const int nsteps,
                                    const float G_th_ratio)
{
  vul_printf (vcl_cout, "gaussian_smooth_scanlines_3pi(): %d steps\n", nsteps);

  const double intra_sl_kth = sg3pi->intra_sl_dist() * G_th_ratio;
  //const double inter_sl_kth = sg3pi->inter_sl_dist() * G_th_ratio;

  //Gaussian kernel parameters.
  /*double sigma = vcl_max (d_intra, d_inter) * G_sigma_ratio;
  vnl_gaussian_kernel_1d gauss1 ((double) sigma);
  vnl_gaussian_kernel_1d gauss2 ((double) sigma); 
  double ksize = vcl_max (d_intra, d_inter) * G_radius_ratio;
  double kth = vcl_max (d_intra, d_inter) * G_th_ratio;*/



  for (unsigned int i=0; i<sg3pi->data().size(); i++) {
    //For the i-th scanline, look for consecutive group of points from js to je
    unsigned int js = 0;
    while (js < sg3pi->data(i).size()) {
      //find je
      unsigned int jep = js;
      unsigned int je = js+1;
      while (je < sg3pi->data(i).size()-1 && 
             vgl_distance (sg3pi->data(i, jep)->pt(), sg3pi->data(i, je)->pt()) 
               < intra_sl_kth) {
        jep++;
        je++;
      }

      if (je >= sg3pi->data(i).size())
        je = sg3pi->data(i).size()-1;

      //smooth curve[js, je] with end points fixed
      vcl_vector<vgl_point_3d<double> > curve;
      curve.clear();
      for (unsigned int j=js; j<=je; j++)
        curve.push_back (sg3pi->data(i,j)->pt());

      dbgl_gaussian_smooth_curve_z_th_fixedendpt (curve, intra_sl_kth, 1.0f, nsteps);

      for (unsigned int j=js; j<=je; j++)
        sg3pi->get_data(i,j)->set_pt (curve[j-js]);

      //set new js
      js = je+1;
    }
  }  
}

//: 2D Gaussian smoothing
//  note that with thresholding to keep sharp gaps,
//  it is not easy to avoid shrinkage of depth at boundary.
//
void gaussian_smooth_2d_3pi (dbmsh3d_sg3pi* sg3pi, const int nsteps,
                             const float G_sigma_ratio,
                             const float G_radius_ratio,
                             const float G_th_ratio)
{
  vul_printf (vcl_cout, "gaussian_smooth_2d_3pi(): %d steps\n", nsteps);
  vul_printf (vcl_cout, "\tG_sigma_r: %f, G_kernel_r: %f, G_th_r: %f.\n", 
              G_sigma_ratio, G_radius_ratio, G_th_ratio);

  //Estimate intra- and inter- scanline sample distance.
  sg3pi->get_sl_sample_dist ();

  //Build a range function to apply Gaussian smoothing.
  //# Row: Raw->data().size(), # Column: maxpn.
  //Non-data value is DBL_MAX (default)
  const int maxpn = sg3pi->get_max_pn() + 1;
  vcl_vector<vcl_vector<double> > range;
  range.resize (sg3pi->data().size());
  for (unsigned int i=0; i<range.size(); i++)
    range[i].resize (maxpn, DBL_MAX);

  for (unsigned int i=0; i<sg3pi->data().size(); i++) {
    for (unsigned int j=0; j<sg3pi->data(i).size(); j++) {
      const dbmsh3d_sg3pi_pt* P = sg3pi->data (i, j);
      range[i][P->pn()] = P->pt().z();
    }
  }

  //Apply 2D Gaussian smoothing
  dbgl_gaussian_smooth_2d_th (range, float(sg3pi->intra_sl_dist()), float(sg3pi->inter_sl_dist()), 
                              nsteps, G_sigma_ratio, G_radius_ratio, G_th_ratio);

  //Put the smoothed range value back to sg3pi->data()
  assert (range.size() == sg3pi->data().size());
  for (unsigned int i=0; i<sg3pi->data().size(); i++) {
    for (unsigned int j=0; j<sg3pi->data(i).size(); j++) {
      dbmsh3d_sg3pi_pt* P = sg3pi->get_data (i, j);
      P->pt().set (P->pt().x(), P->pt().y(), range[i][P->pn()]);
    }
  }
}

// #################################################################
//    SHAPE GRABBER .3PI RAW SCAN MESHING
// #################################################################

void build_mesh_faces_3pi (dbmsh3d_sg3pi* sg3pi, dbmsh3d_mesh* M)
{
  vul_printf (vcl_cout, "build_mesh_faces_3pi(): %u scanlines.\n", sg3pi->data().size());

  //2) Estimate the average intra-scanline sample distance and
  //   inter-scanline sample distance.
  sg3pi->get_sl_sample_dist ();

  //3) First pass of building confident and valid triangles with 
  //   - size less than intra-scanline and inter-scanline threshold.
  //   - not violating with existing triangles.

  double intra_scanline_th = INTRA_SCANLINE_TH * sg3pi->intra_sl_dist();

  vul_printf (vcl_cout, "  meshing triangles between scanlines: ");

  for (int i=0; i<int(sg3pi->data().size())-1; i++) {
    vcl_vector<dbmsh3d_sg3pi_pt*> scanline0 = sg3pi->data()[i];
    if (scanline0.size() == 0)
      continue;

    //Look for next scan line
    vcl_vector<dbmsh3d_sg3pi_pt*> scanline1 = sg3pi->data()[i+1];
    if (scanline1.size() == 0)
      continue;

    if (i % 5 == 0)
      vul_printf (vcl_cout, "%d ", i);
    mesh_between_scanlines (scanline0, scanline1, intra_scanline_th, M);
  }

  vul_printf (vcl_cout, "  done.\n");
}

void mesh_between_scanlines (const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline0, 
                             const vcl_vector<dbmsh3d_sg3pi_pt*>& scanline1, 
                             const double intra_scanline_th,
                             dbmsh3d_mesh* M)
{
  bool test_foot, test_size;
  double t, l;
  dbmsh3d_sg3pi_pt *SP, *SA, *SB;
  dbmsh3d_vertex *VP, *VA, *VB;

  //Mesh points on scanline0 to scanline1
  for (unsigned int i=0; i<scanline0.size(); i++) {
    for (int j=0; j<int(scanline1.size())-1; j++) {
      //Test triangle i,j,j+1.
      SP = scanline0[i];
      SA = scanline1[j];
      SB = scanline1[j+1];

      test_foot = dbgl_footpt_on_line (SP->pt(), SA->pt(), SB->pt(), t, l);
      test_size = l < intra_scanline_th && 
                  vgl_distance (SP->pt(), SA->pt()) < intra_scanline_th && 
                  vgl_distance (SP->pt(), SB->pt()) < intra_scanline_th;

      if (test_foot && test_size) {
        //mesh triangle i, j, j+1.
        dbmsh3d_face* F = M->_new_face ();

        //add vertices into F
        VP = M->vertexmap (SP->id());
        F->_ifs_add_bnd_V (VP);
        VA = M->vertexmap (SA->id());
        F->_ifs_add_bnd_V (VA);
        VB = M->vertexmap (SB->id());
        F->_ifs_add_bnd_V (VB);

        M->_add_face (F);
      }
    }
  }
  
  //Mesh points on scanline1 to scanline0
  for (unsigned int i=0; i<scanline1.size(); i++) {
    for (int j=0; j<int(scanline0.size())-1; j++) {
      //Test triangle i,j,j+1.
      SP = scanline1[i];
      SA = scanline0[j];
      SB = scanline0[j+1];

      test_foot = dbgl_footpt_on_line (SP->pt(), SA->pt(), SB->pt(), t, l);
      test_size = l < intra_scanline_th && 
                  vgl_distance (SP->pt(), SA->pt()) < intra_scanline_th && 
                  vgl_distance (SP->pt(), SB->pt()) < intra_scanline_th;

      if (test_foot && test_size) {
        //mesh triangle i, j, j+1.
        dbmsh3d_face* F = M->_new_face ();

        //add vertices into F
        VP = M->vertexmap (SP->id());
        F->_ifs_add_bnd_V (VP);
        VA = M->vertexmap (SA->id());
        F->_ifs_add_bnd_V (VA);
        VB = M->vertexmap (SB->id());
        F->_ifs_add_bnd_V (VB);

        M->_add_face (F);
      }
    }
  }
}

void sg_detect_bbox (const dbmsh3d_sg3pi* sg3pi, vgl_box_3d<double>& box)
{
  vul_printf (vcl_cout, "  sg_detect_bbox()\n");
  box.empty();  
  for (unsigned int i=0; i<sg3pi->data().size(); i++) {
    for (unsigned int j=0; j<sg3pi->data()[i].size(); j++) {
      dbmsh3d_sg3pi_pt* SP = sg3pi->data()[i][j];
      box.add (SP->pt());
    }
  }
  if (box.is_empty())
    vcl_cerr << "    Empty point set, bounding box undefined." << vcl_endl;
  else
    vcl_cerr << "    " << box << vcl_endl;
}

//: Crop the scanned dataset.
void sg_crop_3pi (dbmsh3d_sg3pi* sg3pi, 
                  const double minX, const double minY, const double minZ, 
                  const double maxX, const double maxY, const double maxZ)
{
  vul_printf (vcl_cout, "sg_crop_3pi(): %u points.\n", sg3pi->get_num_points());
  
  for (unsigned int i=0; i<sg3pi->data().size(); i++) {
    vcl_vector<dbmsh3d_sg3pi_pt*>::iterator it = sg3pi->get_data()[i].begin();
    while (it != sg3pi->get_data()[i].end()) {
      dbmsh3d_sg3pi_pt* SP = (*it);

      if (SP->pt().x() < minX || SP->pt().x() > maxX ||
          SP->pt().y() < minY || SP->pt().y() > maxY ||
          SP->pt().z() < minZ || SP->pt().z() > maxZ) {
        //delete this point from this scanline.
        if (it == sg3pi->get_data()[i].begin()) {
          (sg3pi->get_data()[i]).erase (it);
          it = sg3pi->get_data()[i].begin();
        }
        else {
          vcl_vector<dbmsh3d_sg3pi_pt*>::iterator prev = it;
          prev--;
          (sg3pi->get_data()[i]).erase (it);
          it = ++prev;
        }
      }
      else
        it++;
    }
  }

  vul_printf (vcl_cout, "  After cropping, remaining %u points.\n", sg3pi->get_num_points());
}

