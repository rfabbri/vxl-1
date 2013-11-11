#if !defined(XMVG_IO_COMPOSITE_FILTER_2D_H_)
#define XMVG_IO_COMPOSITE_FILTER_2D_H_

#include <xmvg/xmvg_composite_filter_2d.h>
#include <vsl/vsl_fwd.h>

template <class T>
void vsl_b_write(vsl_b_ostream & os, const xmvg_composite_filter_2d<T> & filter);

template <class T>
void vsl_b_read(vsl_b_istream & is, xmvg_composite_filter_2d<T> & filter);

template <class T>
void vsl_print_summary(vcl_ostream & os,const xmvg_composite_filter_2d<T> & p);

#endif
