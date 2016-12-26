#include <vcl_cmath.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_1d.h>
#include <brip/brip_vil_float_ops.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_ihs_feature.h>


dbinfo_ihs_feature::dbinfo_ihs_feature(dbinfo_feature_data_base_sptr const& data) 
  : dbinfo_feature_base(data)
{
    format_ = DBINFO_IHS_FEATURE;
}

//:scan gradient data from the current frame into the feature data store
bool 
dbinfo_ihs_feature::scan(const unsigned frame, 
                         vcl_vector<vgl_point_2d<unsigned> > const& points,
                         vcl_vector<bool> const& valid,
                         vil_image_resource_sptr const& image)
{

  if(!image)
    return false;

  unsigned nplanes = image->nplanes();

  unsigned n_pts = points.size();
  vbl_array_2d<float> vector_data(n_pts, 2, 0.0);
  if(!n_pts)
    return false;
  
  //standard image info describing the cached data

  vil_pixel_format pix_format = image->pixel_format();
  if(!(pix_format==VIL_PIXEL_FORMAT_BYTE&&nplanes==3)&&!(pix_format == VIL_PIXEL_FORMAT_RGB_BYTE))
    return false;
  
  // convert to IHS
  vil_image_view<vil_rgb<vxl_byte> > color_view = image->get_view();
  vil_image_view<float> I, H, S;
  brip_vil_float_ops::convert_to_IHS(color_view, I, H, S);

  //scan the result and put into the data store
  for(unsigned i = 0; i<n_pts; ++i)
    {
      bool temp = valid[i];
      if(temp)
        {
          //hue
          vector_data[i][0]= H(points[i].x(), points[i].y());
          //saturation
          vector_data[i][1]= S(points[i].x(), points[i].y());
        }
      else
        {vector_data[i][0]= 0.0f; vector_data[i][1]= 0.0f;}
    }

  //construct feature data base
  dbinfo_feature_data<vbl_array_2d<float> >* fdata = 
    new dbinfo_feature_data<vbl_array_2d<float> >(pix_format, image->nplanes(),
                                                  DBINFO_IHS_FEATURE, 1);
  //set the sample to the database
  fdata->set_single_sample(vector_data);
  data_ = fdata;
  return true;
}

