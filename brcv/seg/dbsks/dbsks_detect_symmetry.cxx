// This is file seg/dbsks/dbsks_detect_symmetry.cxx

//:
// \file

#include "dbsks_detect_symmetry.h"
#include <dbdet/edge/dbdet_edgemap.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_fstream.h>

//: Compute symmetry points from every pair of edge points
// Pick those that are below the threshold and save to sym_points;
void dbsks_detect_symmetry::
analyze()
{

  dbdet_edgemap_sptr edgemap = this->edgemap_;
  // Traverse through all pairs of edge points and check
  vcl_cout << " number of edges " << "\n";

  // Collect the list of edges and put in a vector
  vcl_vector<dbdet_edgel* > edgel_list;
  // edgel_list.reserve(edgemap->num_edgels);

  for(int i=0; i<edgemap->edge_cells.rows(); ++i)
  {
    for(int j=0; j<edgemap->edge_cells.cols(); ++j)
    {
      vcl_vector<dbdet_edgel*> es=edgemap->edge_cells(i,j);
      for(unsigned k=0; k<es.size(); k++)
      {
        edgel_list.push_back(es[k]);
      }
    }
  }

  // Compute symmetry score for every pair and record the distribution of scores
  // The symmetry score is in [0, 2]. We devide this into 100 regions, each 0.02.
  vnl_vector<double > sscore_distribution(100, 0);
  
  int num_edgels_per_percent = vnl_math::rnd(edgel_list.size() / 100.0);
  vgl_box_2d<double > bbox_edgemap;
  bbox_edgemap.add(vgl_point_2d<double >(0, 0));
  bbox_edgemap.add(vgl_point_2d<double >(edgemap->width(), edgemap->height()));
  
  for (int percent_count=0; percent_count < 100; ++percent_count)
  {
    vcl_cout << percent_count << "%  ";
    int start_count = percent_count * num_edgels_per_percent;
    int end_count = (percent_count+1) * num_edgels_per_percent; 
    end_count = vnl_math::min(end_count, int(edgel_list.size()));
    
 
    for (int i=start_count; i < end_count; ++i)
    {
      dbdet_edgel* edge1 = edgel_list[i];
      vgl_point_2d<double > pt1 = edge1->pt;
      vgl_vector_2d<double > t1(vcl_cos(edge1->tangent), vcl_sin(edge1->tangent));
      for (unsigned j=i+1; j < edgel_list.size(); ++j)
      {
        dbdet_edgel* edge2 = edgel_list[j];

        // relevant vectors
        vgl_point_2d<double > pt2 = edge2->pt;
        vgl_vector_2d<double > t2(vcl_cos(edge2->tangent), vcl_sin(edge2->tangent));
        vgl_vector_2d<double > v12 = normalized(pt2 - pt1);

        // compute symmetry scores
               

        // force p1 to be the ``left'' and p2 to be the ``right'' boundary points
        t1 = cross_product(v12, t1) > 0 ? -t1 : t1;
        t2 = cross_product(v12, t2) < 0 ? -t2 : t2;
        double symmetry_score = vnl_math::abs(inner_product(v12, t2-t1));

        // update distribution
        int index = int(vcl_floor(symmetry_score / 0.02));
        
        // special handle for score == 2
        if (symmetry_score == 2) index = 99;

        if (index >= 100)
        {
          vcl_cout << "ERROR: symmetry score > 2\n";
          continue;
        }

        ++ sscore_distribution[index];


        // compute the symmetry points when the score is below the threshold
        if (symmetry_score < this->sym_score_threshold_)
        {
          dbsksp_xshock_node_descriptor xnode = 
            this->compute_medial_point(pt1, t1, pt2, t2);

          // only add when the medial point is inside the image area
          if (bbox_edgemap.contains(xnode.pt_))
          {
            this->medial_pts_.push_back(xnode);
          }
        }
      }
    }
  }

  // update member variable with the computed distribution
  this->sym_score_distr_ = sscore_distribution;
  vcl_cout << "Done.\n";
  
  return;
}


//: write the distribution to a file
void dbsks_detect_symmetry::
write_sym_score_distribution_to_file(const vcl_string& filename)
{
  vcl_cout << "Now save the distribution to a file: \n";

  vnl_vector<double > sscore_distribution = this->sym_score_distr_;

  // write output table to files
  //vcl_string sscore_file = "d:/vision/data/symmetry_score/sscore_dist.txt";
  vcl_cout << "Symmetry score distribution file = " << filename << vcl_endl;
  vcl_ofstream outfile(filename.c_str());

  for (unsigned i=0; i<sscore_distribution.size(); ++i)
  {
    outfile << 0.02 * (i+1) << " " << sscore_distribution(i) << "\n";
  }
  
  
  outfile.close();
  return;
}


//: Compute the medial axis point given two point-tangent.
// This is only an estimate of the symmetry point because the two point-tangents may not 
// form a medial point.
dbsksp_xshock_node_descriptor dbsks_detect_symmetry::
compute_medial_point(const vgl_point_2d<double >& pt1, const vgl_vector_2d<double >& t1,
                     const vgl_point_2d<double >& pt2, const vgl_vector_2d<double >& t2)
{
  // suppose pt1 is left and pt2 is right
  // force tangents of pt1 and pt2 to be consistent with this assumption
  vgl_vector_2d<double > v12 = pt2 - pt1;
  vgl_vector_2d<double > tan1 = cross_product(v12, t1) > 0 ? -t1 : t1;
  vgl_vector_2d<double > tan2 = cross_product(v12, t2) < 0 ? -t2 : t2;

  // tangent
  vgl_vector_2d<double > medial_tan = normalized(rotated(v12, vnl_math::pi_over_2));
  double angle_t2_t1 = signed_angle(tan2, tan1);
  if (angle_t2_t1 < 0) // assignment of left and right is wrong
  {
    medial_tan = -medial_tan;
  }

  // phi angle
  double phi = vnl_math::pi_over_2 + vnl_math::abs(angle_t2_t1) / 2;

  // Now compute the medial point
  double d = vgl_distance(pt1, pt2);
  vgl_point_2d<double > medial_pt = 
    midpoint(pt1, pt2) + medial_tan * (d/2) / vcl_tan(phi - vnl_math::pi_over_2);

  double radius = (d/2) / vcl_cos(phi-vnl_math::pi_over_2);

  // Return extrinsic shock node
  return dbsksp_xshock_node_descriptor(medial_pt.x(), medial_pt.y(), 
    vcl_atan2(medial_tan.y(), medial_tan.x()), phi, radius);
}
