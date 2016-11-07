// This is contrib/biotree/biolung/biolung_lungmodel.h

#ifndef biolung_lungmodel_h
#define biolung_lungmodel_h

//:
// \file
// \brief
//
// \author
// Rahul is the author.
// Can added the comments.
//
// \date
// 02/01/05
//
// \verbatim
//  Modifications
// \endverbatim

#include "biolung_3D_Tree.h"
#include <vcl_string.h>

class biolung_lungmodel
{
public:
  int xdim, ydim, zdim;
  biolung_Tree tree; //3D realistic lung tree model

  int ***g; //volumetric data

  biolung_lungmodel(int zdim=700);  //1 voxel corresponds to 35cm/zdim
  ~biolung_lungmodel();

  void saveVolumeInFile(vcl_string filename);

};


#endif
