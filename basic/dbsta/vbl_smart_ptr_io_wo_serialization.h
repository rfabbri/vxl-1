#ifndef _dbsta_distribution_io_h_
#define _dbsta_distribution_io_h_

#include <vsl/vsl_fwd.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_iosfwd.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vbl_smart_ptr to stream.
template <class T>
void vsl_b_write(vsl_b_ostream & os, const vbl_smart_ptr<T> & v);

//: Binary load vbl_sparse_matrix from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vbl_smart_ptr<T> & v);


#endif
