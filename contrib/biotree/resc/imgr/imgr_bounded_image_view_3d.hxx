// This is biotree/resc/imgr_bounded_image_view_3d
#ifndef imgr_bounded_image_view_3d_txx_
#define imgr_bounded_image_view_3d_txx_

#include <imgr/imgr_bounded_image_view_3d.h>
#include <dbil/dbil_bounded_image_view.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/io/vgl_io_box_3d.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_ostream.h>
#include <vil/vil_pixel_format.h>



//=======================================================================
//constructors

template<class T>
imgr_bounded_image_view_3d<T>::
imgr_bounded_image_view_3d(vcl_vector<dbil_bounded_image_view<T>* > const& views,
                           vgl_box_3d<double> const& bounds)
  :  bounds_(bounds) 
{
   view_data_2d_=new imgr_view_data_base();
  
  for(typename vcl_vector<dbil_bounded_image_view<T>* >::const_iterator vit = views.begin();
     vit != views.end(); ++vit)
 view_data_2d_->add_view_2d((vil_image_view<T>*)(*vit));
  
  vil_image_view_base_sptr base = view_data_2d_->view_2d(0);
  if(!views[0])
    {
      ni_ = 0;
      nj_ = 0;
      nk_ = 0;
      nplanes_ = 1;
      format_ = VIL_PIXEL_FORMAT_UNKNOWN;
      return;
    }
  ni_ = views[0]->ni();
  nj_ = views[0]->nj();
  nk_ = views.size();
  nplanes_ = views[0]->nplanes();
  format_ = views[0]->pixel_format();
}


template<class T>
imgr_bounded_image_view_3d<T>::
imgr_bounded_image_view_3d(vgl_box_3d<double> const& bounds,
                           unsigned ni, unsigned nj, unsigned nk,
                           unsigned nplanes)
  : imgr_image_view_3d_base(ni, nj,  nk,  nplanes, vil_pixel_format_of(T())),
    view_data_2d_(new imgr_view_data_base())
{
}

//: copy constructor
template<class T>
imgr_bounded_image_view_3d<T>::
imgr_bounded_image_view_3d(const imgr_bounded_image_view_3d<T>& rhs)
  : imgr_image_view_3d_base(rhs.ni(), rhs.nj(), rhs.nk(), rhs.nplanes(), rhs.pixel_format()),
  view_data_2d_(rhs.view_data_2d_),bounds_(rhs.bounds_)
     
{
}
//:constructor from abstract base
template<class T> imgr_bounded_image_view_3d<T>::
imgr_bounded_image_view_3d(const imgr_image_view_3d_base& rhs)
  : imgr_image_view_3d_base(rhs)
{

  const imgr_bounded_image_view_3d<T>& bv = 
    static_cast<const imgr_bounded_image_view_3d<T>& >(rhs);
  view_data_2d_ = bv.view_data_2d_;
  bounds_ = bv.bounds_;
}

template<class T>
short 
imgr_bounded_image_view_3d<T>::version() const {
  return 1;
}

template<class T>
void 
imgr_bounded_image_view_3d<T>::b_write(vsl_b_ostream &os) const  {
  vsl_b_write(os, version());
  vsl_b_write(os, bounds_);
  vsl_b_write(os, this->nk());
  for(unsigned int i=0; i<this->nk(); i++){
    dbil_bounded_image_view<T> view = view_data_2d_->view_2d(i);
    view.b_write(os);
  }
}

template<class T>
void 
imgr_bounded_image_view_3d<T>::b_read(vsl_b_istream &is){
  short ver;
  unsigned num_views;
  vgl_box_3d<double> box;
  vcl_vector<dbil_bounded_image_view<T>* > views;
  
  if (!is)
    return;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    vsl_b_read(is, box);
    vsl_b_read(is, num_views);
    for(unsigned i=0; i<num_views; i++){
      dbil_bounded_image_view<T> *dbil_img_view_ptr = new dbil_bounded_image_view<T> ();
      dbil_img_view_ptr->b_read(is);
      views.push_back(dbil_img_view_ptr);
    }
    *this = *(new imgr_bounded_image_view_3d<T>(views, box));
    break;

  default:
    vcl_cerr << "I/O ERROR: imgr_bounded_image_view_3d<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream*/
    return;
 }
 }

#define IMGR_BOUNDED_IMAGE_VIEW_3D_INSTANTIATE(T) \
template class imgr_bounded_image_view_3d<T >; 

#endif // imgr_bounded_image_view_3d_txx_
