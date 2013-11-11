#ifndef dbmsh3d_pt_normal_h_
#define dbmsh3d_pt_normal_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/algo/dbmsh3d_pt_normal.h
//:
// \file
// \brief Estimate point normal using vxl/conytib/rpl/rgrl and rsdl.
//        See vxl/contrib/rpl/rgrl/examples/estimate_normals_3d.cxx
//
// \author
//  MingChing Chang  Apr 22, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_vector_fixed.h>

typedef vnl_vector_fixed<double,3>       vector_3d;

//: Compute point normal and store in the vectors.
void compute_point_normal (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& oripts);

//: Estimate the normal of a point set via the rgrl code using Lagrange Multiplier.
//  The normal of the fitted plane corresponds to the eigenvector of the least eigenvalue.
void estimate_point_normal (const vcl_vector<vnl_vector_fixed<double,3> >& pts,
                            vcl_vector<vnl_vector_fixed<double,3> >& normals);

#endif

