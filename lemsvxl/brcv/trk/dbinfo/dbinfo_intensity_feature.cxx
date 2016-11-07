#include <vil/vil_pixel_format.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_convert.h>
#include <vbl/vbl_array_1d.h>
#include <vcl_cstdlib.h>
#include <brip/brip_vil_float_ops.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_intensity_feature.h>

//:scan intensity data from the current frame into the current data store
bool 
dbinfo_intensity_feature::scan(const unsigned frame,
                             vcl_vector<vgl_point_2d<unsigned> > const& points,
                             vcl_vector<bool> const& valid,
                             vil_image_resource_sptr const& image)
{

#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  unsigned ni = image->ni(), nj = image->nj();
#endif
  unsigned n_pts = points.size();
  vbl_array_1d<float> scalar_data(n_pts, 0.0f);



  //standard image info describing the cached data
  vil_pixel_format pix_format = image->pixel_format();
  unsigned nplanes = image->nplanes();

  //for now
  vil_image_view<float> fimage = brip_vil_float_ops::convert_to_float(*image);


  //scan the view extracting image intensities
  //DEBUG JLM 
  for(unsigned i = 0; i<n_pts; ++i)
    {
      if(valid[i])
        scalar_data[i] = fimage(points[i].x(), points[i].y());
      else
        scalar_data[i] = -1;
    }

  //construct feature data base
  dbinfo_feature_data<vbl_array_1d<float> >* fdata = 
    new dbinfo_feature_data<vbl_array_1d<float> >(pix_format, nplanes,
                                                  DBINFO_INTENSITY_FEATURE, 1);
  //set the sample to the database
  fdata->set_single_sample(scalar_data);

  data_ = fdata;
  return true;
}
vil_image_resource_sptr dbinfo_intensity_feature::
image(vcl_vector<vgl_point_2d<float> > const& points,
      vcl_vector<bool> const& valid,
      unsigned n_i, unsigned n_j, float i0, float j0, bool background_noise)
{
  if(!data_)
    return 0;
  //Cast to correct feature data type
  dbinfo_feature_data<vbl_array_1d<float> >* fdata = 
    (dbinfo_feature_data<vbl_array_1d<float> >*)data_.ptr();
  vbl_array_1d<float>& idata = fdata->single_data();
  unsigned ndata = idata.size();
  //construct the image
  vil_image_view<float> image_out(n_i, n_j, 1);  // number of planes = 1

  // fill with random noise so the mutual information is zero
  // 255 satisifies the least possible dynamic range
  if (background_noise) {
    for (unsigned j = 0; j<n_j; j++ ) 
      for (unsigned i = 0; i<n_i; i++) 
        image_out(i,j) = 255*float(vcl_rand()/(RAND_MAX+1.0));     
  } else {
    for (unsigned j = 0; j<n_j; j++ ) 
      for (unsigned i = 0; i<n_i; i++) 
        image_out(i,j) = 255;     
  }

  unsigned npts = points.size();
  for(unsigned k = 0; k<npts; ++k)
    if(valid[k])
      if(k<ndata)
        {
          float pfi = i0 + points[k].x();
          float pfj = j0 + points[k].y();
          if(pfi<0||pfj<0)
            continue;
          unsigned pi = static_cast<unsigned>(vcl_floor(pfi+0.5f));
          unsigned pj = static_cast<unsigned>(vcl_floor(pfj+0.5f));
          if(pi>=n_i||pj>=n_j)
            continue;
          float v = idata[k];
          if(v>=0)
            image_out(pi, pj) = v;
        }
  vil_pixel_format fmt = data_->pix_format();
  switch(fmt)
  {
  case VIL_PIXEL_FORMAT_BYTE:
   {
      vil_image_view<unsigned char> outview =
        brip_vil_float_ops::convert_to_byte(image_out);
      return vil_new_image_resource_of_view(outview);
    }
  case VIL_PIXEL_FORMAT_UINT_16:
   {
      vil_image_view<unsigned short> outview = 
        brip_vil_float_ops::convert_to_short(image_out);
      return vil_new_image_resource_of_view(outview);
    }
  case VIL_PIXEL_FORMAT_FLOAT:
   {
      return vil_new_image_resource_of_view(image_out);
    }
  default:
    vcl_cout << "Unknown format in image(..)\n";
    return 0;
  }
}

