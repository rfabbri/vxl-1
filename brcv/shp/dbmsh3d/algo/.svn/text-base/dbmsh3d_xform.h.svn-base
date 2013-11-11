// This is dbmsh3d/algo/dbmsh3d_xform.h

#ifndef dbmsh3d_xform_h_
#define dbmsh3d_xform_h_

//--------------------------------------------------------------------------------
//:
// \file
// \brief Functions to transform a mesh
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date August 15, 2006
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------

#include <vcl_map.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_sg3pi.h>

//: Perform in-place rigid transformation on a 3D pt_set
bool dbmsh3d_apply_xform (vcl_vector<vgl_point_3d<double> >& pts, 
                      const vgl_h_matrix_3d<double>& H);
bool dbmsh3d_apply_xform (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& oripts, 
                      const vgl_h_matrix_3d<double>& H);

bool dbmsh3d_apply_xform (dbmsh3d_pt_set* PS, const vgl_h_matrix_3d<double>& H);
bool dbmsh3d_apply_xform (dbmsh3d_sg3pi* sg3pi, const vgl_h_matrix_3d<double>& H);

//: Read the transformation file to a vgl_h_matriz_3d<double>
bool dbmsh3d_read_xform_file (const vcl_string& file, vgl_h_matrix_3d<double>& H);

//: Write a homography matrix vgl_h_matriz_3d<double> to a transformation file (xx-af.txt)
bool dbmsh3d_write_xform_file (const vcl_string& file, vgl_h_matrix_3d<double>& H);

//: Read the alignments from list file and compute final xform
bool dbmsh3d_read_xform_listfile (const vcl_string& listfile, vgl_h_matrix_3d<double>& H);

#endif //dbmsh3d_xform_h_



