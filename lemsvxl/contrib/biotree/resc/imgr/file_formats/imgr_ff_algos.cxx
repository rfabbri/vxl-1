#include "imgr_ff_algos.h"
#include <vil/vil_image_resource.h>
#include <vcl_cmath.h>

dbil_bounded_image_view<double>* 
imgr_ff_algos:: calibrate_image(vil_image_resource_sptr const &v,
    vil_image_resource_sptr const &dark,
    vil_image_resource_sptr const &bright,
    vgl_box_2d<double> const& box)
{
  //extract the bounds
  unsigned i0 = (unsigned)box.min_x(); 
  unsigned j0 = (unsigned)box.min_y();
  unsigned imax = (unsigned)box.max_x(); 
  unsigned jmax = (unsigned)box.max_y(); 
  unsigned bni = imax-i0+1;   unsigned bnj = jmax-j0+1;

  //The file-based resource
  //vil_image_view<unsigned short> v = r->get_view(i0, bni, j0, bnj);
  vil_memory_chunk_sptr chunk = new vil_memory_chunk(sizeof(double) * bni * bnj, VIL_PIXEL_FORMAT_DOUBLE);

  //data calibration
  unsigned num_pixels = bni*bnj;

  double* data = reinterpret_cast<double*>( chunk->data());

  vil_image_view<vxl_uint_16> dark_image = dark->get_view(i0, bni, j0, bnj);

  vil_image_view<vxl_uint_16> bright_image = bright->get_view(i0, bni, j0, bnj);

  vil_image_view<vxl_uint_16> cur_image = v->get_view(i0, bni, j0, bnj);

  vxl_uint_16* d = dark_image.top_left_ptr();

  vxl_uint_16* b = bright_image.top_left_ptr();

  vxl_uint_16* c = cur_image.top_left_ptr();

  for(unsigned l=0; l < num_pixels; l++){
    double cd = c[l] - d[l];
    double bd = b[l] - d[l];

    if(cd == 0 && bd == 0)
      data[l] = 0;
    else
      data[l] = -vcl_log( cd / bd);
      //data[l] = cd / bd * 65535 ;
  }

  vil_image_view_base_sptr vbase = 
    new vil_image_view<double>(chunk, 
        reinterpret_cast<double *>(chunk->data()), 
        bni, bnj, 1, 1, bni, 1);


  dbil_bounded_image_view<double>* bview =
    new dbil_bounded_image_view<double>(vbase, i0, j0,
        v->ni(), v->nj());


  return bview;
}
