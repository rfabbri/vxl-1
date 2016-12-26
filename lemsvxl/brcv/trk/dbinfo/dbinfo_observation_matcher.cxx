#include <vcl_cmath.h> // for log(), exp() ..
#include <vnl/vnl_math.h>
#include <vbl/vbl_array_1d.h>
#include <vsol/vsol_point_2d.h>
#include <brip/brip_roi.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_k_medoid.h>
#include <bsta/bsta_joint_histogram.h>
#include <dbinfo/dbinfo_feature_data_base.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_feature_base.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_generator.h>
#include <dbinfo/dbinfo_observation_matcher.h>


//: Abstract mutual information computation
// cast down to appropriate data types
float dbinfo_observation_matcher::minfo(dbinfo_feature_data_base_sptr const& d0,
                                        dbinfo_feature_data_base_sptr const& d1,
                                        bool verbose)
{
  if(d0->format()!=d1->format())
    return 0;
  
  //cast away const
  dbinfo_feature_data_base_sptr& nconst_d0 = (dbinfo_feature_data_base_sptr&)d0;
  dbinfo_feature_data_base_sptr& nconst_d1 = (dbinfo_feature_data_base_sptr&)d1;
    

  switch (d0->format())
    {
    case DBINFO_INTENSITY_FEATURE://scalar intensity data, single frame history
      {
        float range = 255.0;
        unsigned bins = 16;

        dbinfo_feature_data<vbl_array_1d<float> >* cd0 = 
          dbinfo_feature_data<vbl_array_1d<float> >::ptr(nconst_d0);

        dbinfo_feature_data<vbl_array_1d<float> >* cd1 = 
          dbinfo_feature_data<vbl_array_1d<float> >::ptr(nconst_d1);

        vbl_array_1d<float>& v0 = cd0->single_data();
        unsigned npts0 = v0.size();

        vbl_array_1d<float>& v1 = cd1->single_data();
        unsigned npts1 = v1.size();
        if(npts0!=npts1)
          return 0;
        bsta_histogram<float> h0(range, bins), h1(range, bins);
        bsta_joint_histogram<float> jh(range, bins);
        for(unsigned i =0; i<npts0; ++i)
          {
            float vv0 = v0[i], vv1 = v1[i];
            bool valid_v0 = vv0>=0, valid_v1 = vv1>=0;
            if(valid_v0)h0.upcount(vv0, 1.0f);
            if(valid_v1)h1.upcount(vv1, 1.0f);
            if(valid_v0&&valid_v1)
              jh.upcount(vv0, 1.0f, vv1, 1.0f);
          }
        float H0 = h0.entropy(), H1 = h1.entropy();
        float H01 = jh.entropy();
        float mi = H0 + H1 - H01;
        if(verbose)
          {
            vcl_cout << "\nIntensity Histogram 0\n";
            h0.print();
            vcl_cout << "\nIntensity Histogram 1\n";
            h1.print();
            vcl_cout << "\nJoint Intensity Histogram 01\n";
            jh.print();
            vcl_cout << "MI = " << mi << " = H0(" << H0 << ") + H1(" 
                     << H1 << ") - H01(" << H01 << ")\n";
          }
        return mi;
      }
    case DBINFO_GRADIENT_FEATURE://vector inten. gradient, single frame history
      {

        float range = 360.0;
        unsigned bins = 8;
        float deg_rad = (float)(180.0/vnl_math::pi);
        dbinfo_feature_data<vbl_array_2d<float> >* cd0 = 
          dbinfo_feature_data<vbl_array_2d<float> >::ptr(nconst_d0);

        dbinfo_feature_data<vbl_array_2d<float> >* cd1 = 
          dbinfo_feature_data<vbl_array_2d<float> >::ptr(nconst_d1);

        vbl_array_2d<float>& v0 = cd0->single_data();
        vbl_array_2d<float>& v1 = cd1->single_data();
        unsigned npts0 = v0.rows();
        unsigned npts1 = v1.rows();
        if(npts0!=npts1)
          return 0;
        bsta_histogram<float> h0(range, bins), h1(range, bins);
        bsta_joint_histogram<float> jh(range, bins);
        for(unsigned i =0; i<npts0; ++i)
          {
            float Ix0 = v0[i][0], Iy0 = v0[i][1];
            float Ix1 = v1[i][0], Iy1 = v1[i][1];
            float ang0 = deg_rad*vcl_atan2(Iy0, Ix0) + 180.0f;
            float ang1 = deg_rad*vcl_atan2(Iy1, Ix1) + 180.0f;
            float mag0 = vcl_abs(Ix0)+vcl_abs(Iy0);
            float mag1 = vcl_abs(Ix1)+vcl_abs(Iy1);
            h0.upcount(ang0, mag0); h1.upcount(ang1, mag1);
            jh.upcount(ang0, mag0, ang1, mag1);
          }
        
        float H0 = h0.entropy(), H1 = h1.entropy();
        float H01 = jh.entropy();
        float mi = H0 + H1 - H01;
        if(verbose)
          {
            vcl_cout << "\nGradient Dir Histogram 0\n";
            h0.print();
            vcl_cout << "\nGradient Dir Histogram 1\n";
            h1.print();
            vcl_cout << "\nJoint Gradient Dir Histogram 01\n";
            jh.print();
            vcl_cout << "MI = " << mi << " = H0(" << H0 << ") + H1(" 
                     << H1 << ") - H01(" << H01 << ")\n";
          }
        return mi;
      }
    case DBINFO_IHS_FEATURE://vector IHS color angle, single frame history
      {
        float range = 360.0;
        unsigned bins = 8;
        //unused float deg_rad = (float)(180.0/vnl_math::pi);
        dbinfo_feature_data<vbl_array_2d<float> >* cd0 = 
          dbinfo_feature_data<vbl_array_2d<float> >::ptr(nconst_d0);

        dbinfo_feature_data<vbl_array_2d<float> >* cd1 = 
          dbinfo_feature_data<vbl_array_2d<float> >::ptr(nconst_d1);

        vbl_array_2d<float>& v0 = cd0->single_data();
        vbl_array_2d<float>& v1 = cd1->single_data();
        unsigned npts0 = v0.rows();
        unsigned npts1 = v1.rows();
        if(npts0!=npts1)
          return 0;
        bsta_histogram<float> h0(range, bins), h1(range, bins);
        bsta_joint_histogram<float> jh(range, bins);
        for(unsigned i =0; i<npts0; ++i)
          {
            float hue0 = v0[i][0], sat0 = v0[i][1];
            float hue1 = v1[i][0], sat1 = v1[i][1];
            h0.upcount(hue0, sat0); h1.upcount(hue1, sat1);
            jh.upcount(hue0, sat0, hue1, sat1);
          }
        
        float H0 = h0.entropy(), H1 = h1.entropy();
        float H01 = jh.entropy();
        float mi = H0 + H1 - H01;
        if(verbose)
          {
            vcl_cout << "\nHue Histogram 0\n";
            h0.print();
            vcl_cout << "\nHue Histogram 1\n";
            h1.print();
            vcl_cout << "\nJoint hue Histogram 01\n";
            jh.print();
            vcl_cout << "MI = " << mi << " = H0(" << H0 << ") + H1(" 
                     << H1 << ") - H01(" << H01 << ")\n";
          }
        return mi;
      }
    default:
      return 0;
    }
}
//: compute the mutual information between two observations
//selecting the information channels to use (if available)
float dbinfo_observation_matcher::minfo(dbinfo_observation_sptr const& obs0,
                                        dbinfo_observation_sptr const& obsi,
                                        bool use_intensity,
                                        bool use_gradient,
                                        bool use_ihs,
                                        bool verbose )
{
  if(!(obs0&&obsi))
    return 0;

  vcl_vector<dbinfo_feature_base_sptr> feat0 = obs0->features();
  vcl_vector<dbinfo_feature_base_sptr> feati = obsi->features();
  unsigned n0 = feat0.size(), ni = feati.size();
  if(n0!=ni)
    return 0;
  float sum = 0;
  for(unsigned i = 0; i<n0; ++i)
    {
      dbinfo_feature_base_sptr f0 = feat0[i];
      vcl_string type = f0->is_a();
      if(type == "dbinfo_intensity_feature"&&!use_intensity)
        continue;
      if(type == "dbinfo_gradient_feature"&&!use_gradient)
        continue;
      if(type == "dbinfo_ihs_feature"&&!use_ihs)
        continue;
      dbinfo_feature_data_base_sptr d0 = feat0[i]->data();
      dbinfo_feature_data_base_sptr di = feati[i]->data();
   if(!d0||!di)
   {vcl_cout << "Warning- null feature data in dbinfo_observation_matcher::minfo(.)\n";
   continue;}
    
      sum += dbinfo_observation_matcher::minfo(d0, di, verbose);
    }
  return sum;
}

float dbinfo_observation_matcher::minfo(dbinfo_observation_sptr const& obs0,
                                        dbinfo_observation_sptr const& obsi,
                                        bool verbose)
{
  if(!(obs0&&obsi))
    return 0;

  vcl_vector<dbinfo_feature_base_sptr> feat0 = obs0->features();
  vcl_vector<dbinfo_feature_base_sptr> feati = obsi->features();
  unsigned n0 = feat0.size(), ni = feati.size();
  if(n0!=ni)
    return 0;
  float sum = 0;
  for(unsigned i = 0; i<n0; ++i)
    {
      dbinfo_feature_data_base_sptr d0 = feat0[i]->data();
      dbinfo_feature_data_base_sptr di = feati[i]->data();
   if(!d0||!di)
   {vcl_cout << "Warning- null feature data in dbinfo_observation_matcher::minfo(.)\n";
   continue;}
      sum += dbinfo_observation_matcher::minfo(d0, di, verbose);
    }
  return sum;
}

float dbinfo_observation_matcher::minfo(dbinfo_feature_data_base_sptr const& d0,
                                        dbinfo_feature_data_base_sptr const& d1,
                                        vcl_vector<vcl_pair<unsigned, unsigned> > const& correspondences,
                                        bool verbose, float max_value)
{
  if(d0->format()!=d1->format())
    return 0;
  
  //cast away const
  dbinfo_feature_data_base_sptr& nconst_d0 = (dbinfo_feature_data_base_sptr&)d0;
  dbinfo_feature_data_base_sptr& nconst_d1 = (dbinfo_feature_data_base_sptr&)d1;
    

  switch (d0->format())
    {
    case DBINFO_INTENSITY_FEATURE://scalar intensity data, single frame history
      {
        float range = max_value;
        unsigned bins = 16;

        dbinfo_feature_data<vbl_array_1d<float> >* cd0 = 
          dbinfo_feature_data<vbl_array_1d<float> >::ptr(nconst_d0);

        dbinfo_feature_data<vbl_array_1d<float> >* cd1 = 
          dbinfo_feature_data<vbl_array_1d<float> >::ptr(nconst_d1);

        vbl_array_1d<float>& v0 = cd0->single_data();
        unsigned npts0 = v0.size();

        vbl_array_1d<float>& v1 = cd1->single_data();
        unsigned npts1 = v1.size();
        //if(npts0!=npts1)  // we have an explicit correspondence defined,
        //  return 0;       //  the two corresponding regions don't necessarily have same number of pixels
        bsta_histogram<float> h0(range, bins), h1(range, bins);
        bsta_joint_histogram<float> jh(range, bins);
        for(unsigned i =0; i<correspondences.size(); ++i)
          {
            int region0_id = correspondences[i].first;
            int region1_id = correspondences[i].second;
            float vv0 = v0[region0_id], vv1 = v1[region1_id];
            bool valid_v0 = vv0>=0, valid_v1 = vv1>=0;
            if(valid_v0)h0.upcount(vv0, 1.0f);
            if(valid_v1)h1.upcount(vv1, 1.0f);
            if(valid_v0&&valid_v1)
              jh.upcount(vv0, 1.0f, vv1, 1.0f);
          }
        float H0 = h0.entropy(), H1 = h1.entropy();
        float H01 = jh.entropy();
        float mi = H0 + H1 - H01;
        if(verbose)
          {
            vcl_cout << "\nIntensity Histogram 0\n";
            h0.print();
            vcl_cout << "\nIntensity Histogram 1\n";
            h1.print();
            vcl_cout << "\nJoint Intensity Histogram 01\n";
            jh.print();
            vcl_cout << "MI = " << mi << " = H0(" << H0 << ") + H1(" 
                     << H1 << ") - H01(" << H01 << ")\n";
          }
        return mi;
      }
    case DBINFO_GRADIENT_FEATURE://vector inten. gradient, single frame history
      {

        float range = 360.0;
        unsigned bins = 8;
        float deg_rad = (float)(180.0/vnl_math::pi);
        dbinfo_feature_data<vbl_array_2d<float> >* cd0 = 
          dbinfo_feature_data<vbl_array_2d<float> >::ptr(nconst_d0);

        dbinfo_feature_data<vbl_array_2d<float> >* cd1 = 
          dbinfo_feature_data<vbl_array_2d<float> >::ptr(nconst_d1);

        vbl_array_2d<float>& v0 = cd0->single_data();
        vbl_array_2d<float>& v1 = cd1->single_data();
        //unsigned npts0 = v0.rows();
        //unsigned npts1 = v1.rows();
        //if(npts0!=npts1)
        //  return 0;
        bsta_histogram<float> h0(range, bins), h1(range, bins);
        bsta_joint_histogram<float> jh(range, bins);
        for(unsigned i =0; i<correspondences.size(); ++i)
          {
            int region0_id = correspondences[i].first;
            int region1_id = correspondences[i].second;
            float Ix0 = v0[region0_id][0], Iy0 = v0[region0_id][1];
            float Ix1 = v1[region1_id][0], Iy1 = v1[region1_id][1];
            float ang0 = deg_rad*vcl_atan2(Iy0, Ix0) + 180.0f;
            float ang1 = deg_rad*vcl_atan2(Iy1, Ix1) + 180.0f;
            float mag0 = vcl_abs(Ix0)+vcl_abs(Iy0);
            float mag1 = vcl_abs(Ix1)+vcl_abs(Iy1);
            h0.upcount(ang0, mag0); h1.upcount(ang1, mag1);
            jh.upcount(ang0, mag0, ang1, mag1);
          }
        
        float H0 = h0.entropy(), H1 = h1.entropy();
        float H01 = jh.entropy();
        float mi = H0 + H1 - H01;
        if(verbose)
          {
            vcl_cout << "\nGradient Dir Histogram 0\n";
            h0.print();
            vcl_cout << "\nGradient Dir Histogram 1\n";
            h1.print();
            vcl_cout << "\nJoint Gradient Dir Histogram 01\n";
            jh.print();
            vcl_cout << "MI = " << mi << " = H0(" << H0 << ") + H1(" 
                     << H1 << ") - H01(" << H01 << ")\n";
          }
        return mi;
      }
    case DBINFO_IHS_FEATURE://vector IHS color angle, single frame history
      {
        float range = 360.0;
        unsigned bins = 8;
        //unused float deg_rad = (float)(180.0/vnl_math::pi);
        dbinfo_feature_data<vbl_array_2d<float> >* cd0 = 
          dbinfo_feature_data<vbl_array_2d<float> >::ptr(nconst_d0);

        dbinfo_feature_data<vbl_array_2d<float> >* cd1 = 
          dbinfo_feature_data<vbl_array_2d<float> >::ptr(nconst_d1);

        vbl_array_2d<float>& v0 = cd0->single_data();
        vbl_array_2d<float>& v1 = cd1->single_data();
        //unsigned npts0 = v0.rows();
        //unsigned npts1 = v1.rows();
        //if(npts0!=npts1)
        //  return 0;
        bsta_histogram<float> h0(range, bins), h1(range, bins);
        bsta_joint_histogram<float> jh(range, bins);
        for(unsigned i =0; i<correspondences.size(); ++i)
          {
            int region0_id = correspondences[i].first;
            int region1_id = correspondences[i].second;
            float hue0 = v0[region0_id][0], sat0 = v0[region0_id][1];
            float hue1 = v1[region1_id][0], sat1 = v1[region1_id][1];
            h0.upcount(hue0, sat0); h1.upcount(hue1, sat1);
            jh.upcount(hue0, sat0, hue1, sat1);
          }
        
        float H0 = h0.entropy(), H1 = h1.entropy();
        float H01 = jh.entropy();
        float mi = H0 + H1 - H01;
        if(verbose)
          {
            vcl_cout << "\nHue Histogram 0\n";
            h0.print();
            vcl_cout << "\nHue Histogram 1\n";
            h1.print();
            vcl_cout << "\nJoint hue Histogram 01\n";
            jh.print();
            vcl_cout << "MI = " << mi << " = H0(" << H0 << ") + H1(" 
                     << H1 << ") - H01(" << H01 << ")\n";
          }
        return mi;
      }
    default:
      return 0;
    }
}

//: this version takes an explicit correspondence between samples of the two regions
//  e.g. we have correspondences.size() many correspondences
//       correspondences[i].first is the id of the sample in region0 that 
//       is mapped to the region1 sample with id correspondences[i].second 
float dbinfo_observation_matcher::minfo(dbinfo_observation_sptr const& obs0,
                                        dbinfo_observation_sptr const& obsi,
                                        vcl_vector<vcl_pair<unsigned, unsigned> > const& correspondences,
                                        bool verbose, float max_value)
{
  if(!(obs0&&obsi))
    return 0;

  vcl_vector<dbinfo_feature_base_sptr> feat0 = obs0->features();
  vcl_vector<dbinfo_feature_base_sptr> feati = obsi->features();
  unsigned n0 = feat0.size(), ni = feati.size();
  if(n0!=ni)
    return 0;
  float sum = 0;
  for(unsigned i = 0; i<n0; ++i)
    {
      dbinfo_feature_data_base_sptr d0 = feat0[i]->data();
      dbinfo_feature_data_base_sptr di = feati[i]->data();
      sum += dbinfo_observation_matcher::minfo(d0, di, correspondences, verbose, max_value);
    }
  return sum;
}

//--------------------------------------------------------------
// Process the input set of observations into an appropriate number of 
// clusters.  The cluster distance is defined by the Euclidean distance
// between centers of each observation.
bool dbinfo_observation_matcher::
cluster(vcl_vector<dbinfo_observation_sptr> const& obvs,
        vcl_vector<vcl_vector<dbinfo_observation_sptr> >& clusters,
        bool verbose)
{
  //Get the centers of each observation
  vcl_vector<vsol_point_2d_sptr> cogs;
  for(vcl_vector<dbinfo_observation_sptr>::const_iterator oit = obvs.begin();
      oit != obvs.end(); ++oit)
    cogs.push_back((*oit)->geometry()->cog());
  unsigned n = cogs.size();
  if(!n)
    return false;
  //Create a bsta_k_medoid clustering process
  bsta_k_medoid km(n);
  for(unsigned i = 0; i<n; ++i)
    for(unsigned j = i+1; j<n; ++j)
      km.insert_distance(i, j, cogs[i]->distance(cogs[j]));

  for(unsigned k = 1; k<3; ++k)
    {
      vcl_cout << "\nForming " << k << " clusters\n";
      km.do_clustering(k);
      for(unsigned j = 0; j<k;++j)
        vcl_cout << "Clustering Observations  D[" << j << "] = " 
                 <<  km.total_distance(j) << '\n';
    }
  return true;
}
// Note that frame index is defaults to zero
float dbinfo_observation_matcher::minfo(vil_image_resource_sptr const& imgr0,
                                        vsol_polygon_2d_sptr const& region0,
                                        vil_image_resource_sptr const& imgr1,
                                        vsol_polygon_2d_sptr const& region1,
                                        bool intensity_info,
                                        bool gradient_info,
                                        bool color_info)
{
  dbinfo_observation_sptr obs0 =
    new dbinfo_observation(0, imgr0, region0,
                           intensity_info, gradient_info, color_info);

  dbinfo_observation_sptr obs1 =
    new dbinfo_observation(0, imgr1, region1,
                           intensity_info, gradient_info, color_info);
  return dbinfo_observation_matcher::minfo(obs0, obs1);
}

float dbinfo_observation_matcher::minfo(dbinfo_observation_sptr const& obs,
                                        vil_image_resource_sptr const& imgr,
                                        vsol_polygon_2d_sptr const& region)

{
  //find transformation between region and obs' geometry
  // assume translation for now
  vsol_point_2d_sptr obs_geom = obs->geometry()->cog();
  vsol_point_2d_sptr region_geom = region->centroid();
  float tx = static_cast<float>(region_geom->x()),
    ty = static_cast<float>(region_geom->y());
  tx -= static_cast<float>(obs_geom->x());
  ty -= static_cast<float>(obs_geom->y());
  vcl_cout << "Cog Translation = (" << tx << ' ' << ty << ")\n";
  vgl_h_matrix_2d<float> H;
  H.set_identity();
  H.set_translation(tx, ty);
  dbinfo_observation_sptr obs1 = 
    dbinfo_observation_generator::generate(obs, H);
  obs1->scan(obs->frame(), imgr);
  return dbinfo_observation_matcher::minfo(obs, obs1);
}
