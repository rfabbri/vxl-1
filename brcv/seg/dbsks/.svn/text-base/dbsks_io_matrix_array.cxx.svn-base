// This is dbsks/dbsks_io_matrix_array.cxx

//:
// \file

#include "dbsks_io_matrix_array.h"
#include <vnl/io/vnl_io_matrix.h>
#include <vsl/vsl_binary_io.h>


// ----------------------------------------------------------------------------
//: Binary save an array of matrices to a a stream
void vsl_b_write(vsl_b_ostream & os, const vbl_array_1d<vnl_matrix<float > >& f)
{
  // 1. save version number
  const short version = 1;
  vsl_b_write(os, version);

  // 2. save the matrices one by one
  unsigned int num_planes = f.size();
  unsigned int capacity = f.capacity();

  vsl_b_write(os, num_planes);
  vsl_b_write(os, capacity);
  
  for (unsigned int i =0; i <num_planes; ++i)
  {
    vsl_b_write(os, f[i]);
  }

  return;
}





// ----------------------------------------------------------------------------
//: Binary save an array of matrices to a a stream
void vsl_b_write(vsl_b_ostream & os, const vbl_array_1d<vnl_matrix<int > >& f)
{
  // 1. save version number
  const short version = 1;
  vsl_b_write(os, version);

  // 2. save the matrices one by one
  unsigned int num_planes = f.size();
  unsigned int capacity = f.capacity();

  vsl_b_write(os, num_planes);
  vsl_b_write(os, capacity);
  
  for (unsigned int i =0; i <num_planes; ++i)
  {
    vsl_b_write(os, f[i]);
  }

  return;
}




// ----------------------------------------------------------------------------
//: Binary load an array of matrices from a stream 
void vsl_b_read(vsl_b_istream & is, vbl_array_1d<vnl_matrix<float > >& f)
{
  if (!is) return;

  // read version number
  short version;
  unsigned int num_planes;
  unsigned int capacity;

  vsl_b_read(is, version);

  switch (version)
  {
  case (1):
    vsl_b_read(is, num_planes);
    vsl_b_read(is, capacity);

    f.clear();
    f.reserve(capacity);
    
    // place holder for the matrices
    for (unsigned int i =0; i < num_planes; ++i)
    {
      f.push_back(vnl_matrix<float >());
    }

    // load the real contents of the matrices
    for (unsigned int i =0; i < num_planes; ++i)
    {
      vsl_b_read(is, f[i]);
    }
    break;

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream & is, const vbl_array_1d<vnl_matrix<float > >& f) \n"
      << "         Unknown version number " << version << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }

  return;
}



// ----------------------------------------------------------------------------
//: Binary load an array of matrices from a stream 
void vsl_b_read(vsl_b_istream & is, vbl_array_1d<vnl_matrix<int > >& f)
{
  if (!is) return;

  // read version number
  short version;
  unsigned int num_planes;
  unsigned int capacity;

  vsl_b_read(is, version);

  switch (version)
  {
  case (1):
    vsl_b_read(is, num_planes);
    vsl_b_read(is, capacity);

    f.clear();
    f.reserve(capacity);
    
    // place holder for the matrices
    for (unsigned int i =0; i < num_planes; ++i)
    {
      f.push_back(vnl_matrix<int >());
    }

    // load the real contents of the matrices
    for (unsigned int i =0; i < num_planes; ++i)
    {
      vsl_b_read(is, f[i]);
    }
    break;

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream & is, const vbl_array_1d<vnl_matrix<int > >& f) \n"
      << "         Unknown version number " << version << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}


