// This is dbsksp/pro/dbsks_compute_symmetry_points_process.cxx

//:
// \file

#include "dbsks_compute_symmetry_points_process.h"
#include <bpro1/bpro1_parameters.h>


#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbsks/dbsks_detect_symmetry.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

//: Constructor
dbsks_compute_symmetry_points_process::
dbsks_compute_symmetry_points_process()
{
  if( 
    !parameters()->add("Threshold: " , "-threshold", 0.01f)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsks_compute_symmetry_points_process::
~dbsks_compute_symmetry_points_process()
{
}


//: Clone the process
bpro1_process* dbsks_compute_symmetry_points_process::
clone() const
{
  return new dbsks_compute_symmetry_points_process(*this);
}

//: Returns the name of this process
vcl_string dbsks_compute_symmetry_points_process::
name()
{ 
  return "Compute Symmetry Points"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_compute_symmetry_points_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "edge_map" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_compute_symmetry_points_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back("image");
  
  return to_return;
}

//: Return the number of input frames for this process
int dbsks_compute_symmetry_points_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_compute_symmetry_points_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsks_compute_symmetry_points_process::
execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << "In dbsks_compute_symmetry_points_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }


  // USER PARAMS --------------------------------------
  float threshold = 0;
  parameters()->get_value( "-threshold", threshold);


  // STORAGE CLASSES --------------------------------------

  // get the shock graph from storage classes
  dbdet_edgemap_storage_sptr edgemap_storage;
  edgemap_storage.vertical_cast(input_data_[0][0]);

  // PROCESS DATA -----------------------------------------

  // Traverse through all pairs of edge points and check
  dbdet_edgemap_sptr edgemap=edgemap_storage->get_edgemap();

  dbsks_detect_symmetry detector;
  detector.edgemap_ = edgemap;
  detector.sym_score_threshold_ = threshold;
  detector.analyze();

  // write output table to files
  vcl_string sscore_file = "d:/vision/data/symmetry_score/sscore_dist.txt";
  detector.write_sym_score_distribution_to_file(sscore_file);
  
  //// turn the medial points with symmetry score < 0.01 to vsol points
  //vcl_vector<vsol_spatial_object_2d_sptr > medial_pts;
  //for (unsigned i=0; i<detector.medial_pts_.size(); ++i)
  //{
  //  vgl_point_2d<double > pt = detector.medial_pts_[i].pt_;
  //  double psi = detector.medial_pts_[i].psi_;
  //  vgl_vector_2d<double > dir(3 * vcl_cos(psi), 2 * vcl_sin(psi));
  //  vsol_line_2d_sptr line = new vsol_line_2d(dir, pt);
  //  medial_pts.push_back(line->cast_to_spatial_object());
  //}

  //vcl_cout << "Number of medial points = " << medial_pts.size() << vcl_endl;

   //// create the output storage class
  //vidpro1_vsol2D_storage_sptr vsol_storage = vidpro1_vsol2D_storage_new();
  //output_data_[0].push_back(vsol_storage);
  //vsol_storage->add_objects(medial_pts, "medial points");


  vil_image_view<vxl_byte > medial_image(edgemap->width(), edgemap->height());
  medial_image.fill(0);

  // turn the medial points with symmetry score < 0.01 to image point
  int count = 0;
  for (unsigned i=0; i<detector.medial_pts_.size(); ++i)
  {
    vgl_point_2d<double > pt = detector.medial_pts_[i].pt_;

    int pt_i = vnl_math::rnd(pt.x());
    int pt_j = vnl_math::rnd(pt.y());

    if (pt_i >= (int) medial_image.ni() || pt_j >= (int)medial_image.nj())
      continue;

    medial_image(pt_i,pt_j) = vxl_byte(255);
    ++count;
  }

  //vcl_cout << "Number of medial points = " << medial_pts.size() << vcl_endl;


  // create the output storage class
  vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();
  output_data_[0].push_back(image_storage);
  image_storage->set_image(vil_new_image_resource_of_view(medial_image));
  
  return true;
}


// ----------------------------------------------------------------------------
bool dbsks_compute_symmetry_points_process::
finish()
{
  return true;
}
