//:
// \file
// \author Isabel Restrepo
// \date 26-Oct-2010

#include "dbrec3d_scene_model.h"

#include <bvpl/bvpl_octree/bvpl_scene_statistics.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_apm_traits.h>

dbrec3d_scene_model::dbrec3d_scene_model(boxm_scene_base_sptr scene_base):scene_base_(scene_base)
{
  
  //perfom scene casting to determine what kind of scene we are visualizing
  
  //cast the gradient scene
  typedef vnl_vector_fixed< float,3 > datatype;
  typedef boct_tree<short, datatype > grad_tree_type;
  typedef vnl_vector_fixed< float,10 > sf_datatype;
  typedef boct_tree<short, sf_datatype > sf_tree_type;
  if (scene_base->appearence_model() == BOXM_FLOAT)
  {
    boxm_scene<boct_tree<short, float > > *scene = static_cast<boxm_scene< boct_tree<short, float > >* >(scene_base_.as_pointer());
    
    //get its histograms
    bsta_histogram<float> magnitude_hist;
    bvpl_compute_scene_statistics(scene, magnitude_hist);
    hist_ = magnitude_hist;
    hist_.print_log();
  }
  else if ( scene_base->appearence_model() == BOXM_SHORT )
  {
    boxm_scene<boct_tree<short, short > > *scene = static_cast<boxm_scene< boct_tree<short, short > >* >(scene_base_.as_pointer());
    
    //get its histograms
    bsta_histogram<float> id_hist;
    bvpl_compute_scene_statistics(scene, id_hist,21);
    hist_ = id_hist;
    hist_.print_log();
  }
  else if (scene_base->appearence_model() == VNL_FLOAT_3)
  {
    boxm_scene<grad_tree_type> *scene = dynamic_cast<boxm_scene< grad_tree_type >* >(scene_base_.as_pointer());
    //get its histograms
    bsta_histogram<float> magnitude_hist;
    bvpl_compute_scene_statistics(scene, magnitude_hist);
    hist_ = magnitude_hist;
    hist_.print_log();
    
    vcl_cout << "Values with area above: \n";
    
    for (int i = 0 ; i <=20; i++) {
      vcl_cout << float(i)/20.0  << " : " << hist_.value_with_area_above(float(i)/20.0) << "bin: " << hist_.bin_at_val(hist_.value_with_area_above(float(i)/20.0)) << vcl_endl;
    }
    

  }
  else if (scene_base->appearence_model() == VNL_FLOAT_10)
  {
    boxm_scene<sf_tree_type> *scene = dynamic_cast<boxm_scene< sf_tree_type >* >(scene_base_.as_pointer());
    //get its histograms
    bsta_histogram<float> magnitude_hist;
    bvpl_compute_scene_statistics(scene, magnitude_hist);
    hist_ = magnitude_hist;
    hist_.pretty_print();
  }
  
}

QImage dbrec3d_scene_model::histogram_image()
{
  
  vcl_cout << " Creating Histogram Image: \n Number of bins: " << hist_.nbins() 
  << " \n Hist : " << hist_ << vcl_endl;
  QImage hist_image(hist_.nbins(), hist_.nbins(), QImage::Format_Indexed8);
  hist_image.fill(255);
  
  float max_height = 0.0;
  
  for(unsigned i = 0; i < hist_.nbins(); i++)
  {
    if(hist_.counts(i) > max_height)
      max_height = hist_.counts(i);
  }
  
  for (unsigned i=0; i<hist_.nbins(); i++)
	{	  
	  int h = (hist_.counts(i)/max_height) * hist_.nbins();
	  for (int j=0; j<h; j++)
    {
      *(hist_image.scanLine(j) + i) =  0;
    }
	}
  //hist_image = hist_image->mirrored();
  return hist_image.mirrored();
}
