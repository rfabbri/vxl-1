#include <vcl_cmath.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_1d.h>
#include <brip/brip_vil_float_ops.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_gradient_feature.h>

void dbinfo_gradient_feature::set_margin()
{
  margin_ = brip_vil_float_ops::gaussian_radius(sigma_);
  margin_++; //increase to account for 3x3 derivative operator
}

//Default constructor
dbinfo_gradient_feature::dbinfo_gradient_feature() : sigma_(1.0f)
{
  format_ = DBINFO_GRADIENT_FEATURE;
  this->set_margin();
}

dbinfo_gradient_feature::
dbinfo_gradient_feature(dbinfo_feature_data_base_sptr const& data) 
  : dbinfo_feature_base(data), sigma_(1.0f)
{
  format_ = DBINFO_GRADIENT_FEATURE;
  this->set_margin();
}

//:scan gradient data from the current frame into the feature data store
bool 
dbinfo_gradient_feature::scan(const unsigned frame, 
                            vcl_vector<vgl_point_2d<unsigned> > const& points,
                            vcl_vector<bool> const& valid,
                            vil_image_resource_sptr const& image)
{

  unsigned n_pts = points.size();
  vbl_array_2d<float> vector_data(n_pts, 2, 0.0);


  //standard image info describing the cached data

  vil_pixel_format pix_format = image->pixel_format();
  unsigned nplanes = image->nplanes();

  //the greyscale float image
  vil_image_view<float> fimage = brip_vil_float_ops::convert_to_float(*image);
  
  //smooth the image
  vil_image_view<float> gimage = brip_vil_float_ops::gaussian(fimage, sigma_);
  
  //extract the gradient components
  unsigned ni = fimage.ni(), nj = fimage.nj();
  vil_image_view<float> Ix(ni, nj), Iy(ni, nj);
  brip_vil_float_ops::gradient_3x3(gimage, Ix, Iy);

  //scan the result and put into the data store
  for(unsigned i = 0; i<n_pts; ++i)
    {
      bool temp = valid[i];
      if(temp)
        {
          vector_data[i][0]= Ix(points[i].x(), points[i].y());
          vector_data[i][1]= Iy(points[i].x(), points[i].y());
        }
      else
        {vector_data[i][0]= 0.0f; vector_data[i][1]= 0.0f;}
    }

  //construct feature data base
  dbinfo_feature_data<vbl_array_2d<float> >* fdata = 
    new dbinfo_feature_data<vbl_array_2d<float> >(pix_format, nplanes,
                                                DBINFO_GRADIENT_FEATURE, 1);
  //set the sample to the database
  fdata->set_single_sample(vector_data);
  data_ = fdata;
  return true;
}

