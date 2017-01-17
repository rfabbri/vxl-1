// This is brl/blem/brcv/trk/dbinfo/pro/dbinfo_mutual_info_process.cxx
//:
// \file
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsl/vsl_binary_io.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>
#include <dbinfo/dbinfo_ihs_feature.h>
#include <dbinfo/dbinfo_observation_generator.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <dbinfo/pro/dbinfo_mutual_info_process.h>

//: Constructor
dbinfo_mutual_info_process::dbinfo_mutual_info_process()
  : bpro1_process()
{
  if( !parameters()->add( "step",  "-step", (unsigned) 1)                   ||
      !parameters()->add( "sigma",    "-sigma", (float) 1.0)                ||
      !parameters()->add( "Intensity", "-int", true)                        ||
      !parameters()->add( "Gradient", "-grad", true)                        ||
      !parameters()->add( "Color", "-color", true))                          
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
} 

//: Copy Constructor
dbinfo_mutual_info_process::
dbinfo_mutual_info_process(const dbinfo_mutual_info_process& other) :
  bpro1_process(other){}

//: Destructor
dbinfo_mutual_info_process::~dbinfo_mutual_info_process()
{
}


//: Clone the process
bpro1_process* 
dbinfo_mutual_info_process::clone() const
{
  return new dbinfo_mutual_info_process(*this);
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbinfo_mutual_info_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbinfo_mutual_info_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Returns the number of input frames to this process
int
dbinfo_mutual_info_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbinfo_mutual_info_process::output_frames()
{
  return 1;
}

static bool 
extract_polygons_from_storage(vidpro1_vsol2D_storage_sptr const& region_storage,
                              vcl_vector<vsol_polygon_2d_sptr>& regions)
{
  regions.clear();
  vcl_vector< vsol_spatial_object_2d_sptr > region_sos = 
    region_storage->all_data(); 
  if(!region_sos.size())
    {
      vcl_cout << "In dbinfo_mutual_info_process::execute() -"
               << " no source regions to match\n";
      return false;
    }
  //Cast to polygons
  for(vcl_vector< vsol_spatial_object_2d_sptr >::iterator rit =
        region_sos.begin(); rit != region_sos.end(); ++rit)
    {
      vsol_region_2d* r = (*rit)->cast_to_region();
      vsol_polygon_2d_sptr poly = r->cast_to_polygon();
      if(!poly)
        {
          vcl_cout << "In dbinfo_mutual_info_process::execute() -"
                   << " null polygon in input storage\n";
          return false;
        }
      regions.push_back(poly);
    }
  if(!regions.size())
    {
      vcl_cout << "In dbinfo_mutual_info_process::execute() -"
               << " no input regions\n";
      return false;
    }
  return true;
}
// Construct an observation
static dbinfo_observation_sptr 
construct_observation(vil_image_resource_sptr const& imgr,
                      vsol_polygon_2d_sptr const& region,
                      bool intensity_info,
                      bool gradient_info,
                      bool color_info)
{
  return new dbinfo_observation(0, imgr, region, intensity_info,
                                gradient_info, color_info);
}

static 
bool find_match(dbinfo_observation_sptr const& obs,
                vil_image_resource_sptr const& imgr, 
                const unsigned step,
                vsol_polygon_2d_sptr& match
                )
{
  //get bounds of observation
  dbinfo_region_geometry_sptr rg = obs->geometry();
  double diameter = rg->diameter();
  int radius = static_cast<int>(diameter/2);
  int ni = static_cast<int>(imgr->ni());
  int nj = static_cast<int>(imgr->nj());
  vsol_point_2d_sptr cog = rg->cog();
  float xc = static_cast<float>(cog->x()), yc = static_cast<float>(cog->y());
  float mi_max = 0;
  dbinfo_observation_sptr best_hypo;
  //search over translation (for now)
  for(int i =  radius; i<(ni-radius); i+=step)
    for(int j = radius; j<(nj-radius); j+=step)
      {
        vgl_h_matrix_2d<float> H;
        H.set_identity();
        float xi = static_cast<float>(i), yi = static_cast<float>(j);
        H.set_translation(xi-xc, yi-yc);
        dbinfo_observation_sptr hypo = 
          dbinfo_observation_generator::generate(obs, H);
        hypo->scan(0,imgr);
        float mi = dbinfo_observation_matcher::minfo(obs, hypo);
        vcl_cout << "MI(" << i << ' ' << j << ")= " << mi << '\n';
        if(mi>mi_max)
          {
            mi_max = mi;
            best_hypo = hypo;
          }
      }
  if(!best_hypo)
    return false;
  dbinfo_region_geometry_sptr geo = best_hypo->geometry();
  vcl_cout << "Best Hypothesis " << *(geo->cog()) << '\n';
  vcl_cout << "Compared with Original Region " << *(rg->cog()) << '\n';
  match = geo->poly(0);
  return true;
}

static 
bool find_matches(vcl_vector<dbinfo_observation_sptr> const& observs,
                  vil_image_resource_sptr const& imgr1, 
                  const unsigned step,
                  vcl_vector<vsol_polygon_2d_sptr>& matches)
{
  matches.clear();
  for(vcl_vector<dbinfo_observation_sptr>::const_iterator oit = observs.begin();
      oit != observs.end(); ++oit)
    {
      vsol_polygon_2d_sptr match;
      if(!find_match(*oit,imgr1, step, match))
        continue;
      matches.push_back(match);
    }
  return matches.size()>0;
}
static 
void package_output_regions(vcl_vector<vsol_polygon_2d_sptr> const& regions,
                            vidpro1_vsol2D_storage_sptr& output_storage)
{
  //cast to spatial objects
  vcl_vector<vsol_spatial_object_2d_sptr> sos;
  for(vcl_vector<vsol_polygon_2d_sptr>::const_iterator rit = regions.begin();
      rit != regions.end(); ++rit)
    {
      vsol_region_2d* r = (*rit)->cast_to_region();
      sos.push_back(r->cast_to_spatial_object());
    }
  output_storage->add_objects(sos);
}

//: Run the process on the current frame
bool
dbinfo_mutual_info_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << __FILE__ << " - not exactly one input frame" << vcl_endl;
    return false;
  }
  unsigned step;
  float sigma;
  bool inten, grad, color;
  parameters()->get_value( "-step",       step);
  parameters()->get_value( "-sigma",      sigma);
  parameters()->get_value( "-int",        inten);
  parameters()->get_value( "-grad",       grad);
  parameters()->get_value( "-color",      color );

  //Unpack the storage elements from the repository
  vidpro1_image_storage_sptr image_storage0;
  image_storage0.vertical_cast(input_data_[0][0]);

  vidpro1_image_storage_sptr image_storage1;
  image_storage1.vertical_cast(input_data_[0][1]);

  vidpro1_vsol2D_storage_sptr source_region_storage;
  source_region_storage.vertical_cast(input_data_[0][2]);
  
  if(!image_storage0||!image_storage1||!source_region_storage)
    return false;
  //get the input images
  vil_image_resource_sptr imgr0 = image_storage0->get_image();
  vil_image_resource_sptr imgr1 = image_storage1->get_image();

  //get the input spatial objects
  vcl_vector<vsol_polygon_2d_sptr> regions;
  if(!extract_polygons_from_storage(source_region_storage, regions))
    return false;
  
  // generate observations from the input polygons on image0;
  vcl_vector<dbinfo_observation_sptr> source_observations;
  for(vcl_vector<vsol_polygon_2d_sptr>::iterator rit = regions.begin();  
      rit != regions.end(); ++rit)
    source_observations.push_back(construct_observation(imgr0, *rit,
                                                        inten, grad, color));
  //find the optimum transformation of each region in image1
  //i.e. find the transformation that maximizes the total mutual information
  //and the resulting region polygon
  vcl_vector<vsol_polygon_2d_sptr> matched_regions;
  if(!find_matches(source_observations, imgr1,  step, matched_regions))
    return false;
  
  //form output data
  vidpro1_vsol2D_storage_sptr output_storage = vidpro1_vsol2D_storage_new();
  package_output_regions(matched_regions, output_storage);   
  output_data_[0].push_back(output_storage);
  return true;
}

//: Finish
bool
dbinfo_mutual_info_process::finish()
{
  return true;
}
