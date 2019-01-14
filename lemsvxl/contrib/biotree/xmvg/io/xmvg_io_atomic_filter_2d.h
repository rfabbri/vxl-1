#ifndef xmvg_io_atomic_filter_2d_h_
#define xmvg_io_atomic_filter_2d_h_

//: 
// \file  xmvg_io_atomic_filter_2d.h
// \brief  
// \author    Kongbin Kang
// \date        2005-03-28
// 


#include <vsl/vsl_fwd.h>
#include <xmvg/xmvg_atomic_filter_2d.h>
#include <iostream>

//: Binary save
template<class T>
void vsl_b_write(vsl_b_ostream & os, const xmvg_atomic_filter_2d<T> & f);

//: Binary load
template<class T>
void vsl_b_read(vsl_b_istream & is, xmvg_atomic_filter_2d<T>& f);

//: print
template<class T>
void vsl_print_summary(std::ostream& os, const xmvg_atomic_filter_2d<T> &f);
#endif
