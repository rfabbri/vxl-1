// This is basic/dbgl/algo/dbmsh3d_isosurface.h

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif



#ifndef dbmsh3d_isosurface_h_
#define dbmsh3d_isosurface_h_

//:
// \file
// \brief Algorithms to find isosurface of a 3D structure
//
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date July 8, 2006
//
// \verbatim  
//  Modifications:
// \endverbatim

#include <vil3d/vil3d_image_view.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

//: This function takes in a 3D image and computes its isosurface at `val'
bool dbmsh3d_isosurface(dbmsh3d_mesh& mesh, 
                        const vil3d_image_view<vxl_byte >& img3d,
                        vxl_byte val = 127,
                        double data_spacing_dx = 1,
                        double data_spacing_dy = 1,
                        double data_spacing_dz = 1
                        );

#endif // dbmsh3d_isosurface_h_
