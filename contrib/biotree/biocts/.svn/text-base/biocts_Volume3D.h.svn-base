// This is contrib/biotree/biocts/biocts_Volume3D.h

#ifndef biocts_Volume3D_h
#define biocts_Volume3D_h

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

#include <vcl_cmath.h>
#include <vcl_string.h>

class biocts_Volume3D;

class biocts_Volume3D
{
public:
  int xdim, ydim, zdim;
  int Dia;
  int ***data;

  biocts_Volume3D(int xdim=0, int ydim=0, int zdim=0);
  biocts_Volume3D(vcl_string volumedatafile);

  ~biocts_Volume3D();

  void rotate_volume(biocts_Volume3D* new_ob, double ang);
  void saveVolumeInFile(vcl_string filename);
};

#endif
