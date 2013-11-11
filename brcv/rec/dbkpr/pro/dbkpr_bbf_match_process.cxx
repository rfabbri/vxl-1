// This is brcv/seg/dbdet/pro/dbkpr_bbf_match_process.cxx

//:
// \file

#include "dbkpr_bbf_match_process.h"
#include <dbdet/dbdet_lowe_keypoint.h>
#include <dbnl/algo/dbnl_bbf_tree.h>

#include <dbdet/pro/dbdet_keypoint_storage.h>

#include <bpro1/bpro1_parameters.h>

#include <mvl/FMatrixComputeRANSAC.h>


//: Constructor
dbkpr_bbf_match_process::dbkpr_bbf_match_process()
{
  if( !parameters()->add( "Max bins searched" , "-max_bins"  , (int)50 ) ||
      !parameters()->add( "Max scale" ,         "-max_scale" , 10.0f   ) ||
      !parameters()->add( "Estimate F Matrix" , "-est_F"     , false   ) ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbkpr_bbf_match_process::~dbkpr_bbf_match_process()
{
}


//: Clone the process
bpro1_process*
dbkpr_bbf_match_process::clone() const
{
  return new dbkpr_bbf_match_process(*this);
}


//: Return the name of this process
vcl_string
dbkpr_bbf_match_process::name()
{
  return "Match Keypoints";
}


//: Return the number of input frame for this process
int
dbkpr_bbf_match_process::input_frames()
{
  return 2;
}


//: Return the number of output frames for this process
int
dbkpr_bbf_match_process::output_frames()
{
  return 2;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbkpr_bbf_match_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "keypoints" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbkpr_bbf_match_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "keypoints" );
  return to_return;
}


//: Execute the process
bool
dbkpr_bbf_match_process::execute()
{
  if ( input_data_.size() != 2 ){
    vcl_cout << "In dbkpr_bbf_match_process::execute() - not exactly two"
             << " input keypoint sets \n";
    return false;
  }
  clear_output();

  // cast the storage classes
  dbdet_keypoint_storage_sptr frame_keypoints1, frame_keypoints2;
  frame_keypoints1.vertical_cast(input_data_[1][0]);
  frame_keypoints2.vertical_cast(input_data_[0][0]);

  // get keypoints from the storage classes
  const vcl_vector< dbdet_keypoint_sptr >& keypoints1 = frame_keypoints1->keypoints();
  const vcl_vector< dbdet_keypoint_sptr >& keypoints2 = frame_keypoints2->keypoints();


  int ni1 = frame_keypoints1->ni();
  int nj1 = frame_keypoints1->nj();
  int ni2 = frame_keypoints2->ni();
  int nj2 = frame_keypoints2->nj();

  int max_bins = 0;
  bool est_F = false;
  parameters()->get_value( "-max_bins" , max_bins );
  parameters()->get_value( "-max_scale" , max_scale_ );
  parameters()->get_value( "-est_F" , est_F );
  // build the BBF search tree
  vcl_vector<vnl_vector_fixed<double,128> > descriptors(keypoints1.size());
  for(unsigned int i=0; i<keypoints1.size(); ++i)
    descriptors[i] = keypoints1[i]->descriptor();
  dbnl_bbf_tree<double,128> bbf(descriptors);

  vcl_vector< dbdet_keypoint_sptr > matches1, matches2;
  for (unsigned int i=0; i<keypoints2.size(); ++i){
    //find the two closest matches
    vcl_vector<int> matches;
    bbf.n_nearest(keypoints2[i]->descriptor(), matches, 2, max_bins);
    // check if the distance to the first is less than 80% of the distance to the second
    if( vnl_vector_ssd(keypoints2[i]->descriptor(),keypoints1[matches[0]]->descriptor()) <
        vnl_vector_ssd(keypoints2[i]->descriptor(),keypoints1[matches[1]]->descriptor())*.64){
      matches1.push_back(keypoints1[matches[0]]);
      matches2.push_back(keypoints2[i]);
    }
  }

  if(est_F){
    vcl_vector< vgl_homg_point_2d<double> > pts1, pts2;
    FMatrixComputeRANSAC fransac(true,0.5);
    for (unsigned int i=0; i<matches1.size(); ++i){
      pts1.push_back(vgl_homg_point_2d<double>(*matches1[i]));
      pts2.push_back(vgl_homg_point_2d<double>(*matches2[i])); 
    }
    FMatrix F = fransac.compute(pts1,pts2);
    vcl_vector<bool> inliers = fransac.get_inliers();

    vcl_vector< dbdet_keypoint_sptr > old_matches1(matches1), old_matches2(matches2);
    matches1.clear();
    matches2.clear();
    for (unsigned int i=0; i<inliers.size(); ++i){
      if(inliers[i]){ 
        matches1.push_back(old_matches1[i]);
        matches2.push_back(old_matches2[i]);
      }
    }
    
  }

  
  // create the output storage classes
  dbdet_keypoint_storage_sptr output_storage1 = dbdet_keypoint_storage_new(matches1,ni1,nj1);
  dbdet_keypoint_storage_sptr output_storage2 = dbdet_keypoint_storage_new(matches2,ni2,nj2);
  output_data_[1].push_back(output_storage1);
  output_data_[0].push_back(output_storage2);

  return true;
}



bool
dbkpr_bbf_match_process::finish()
{
  return true;
}



