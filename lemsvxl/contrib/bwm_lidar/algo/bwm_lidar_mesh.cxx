// This is bwm_lidar_mesh.cxx
//   Ming-Ching Chang
//   Dec 13, 2007.
//:
// \file

#include "bwm_lidar_mesh.h"

#include <list>
#include <algorithm>
#include <iostream>
#include <utility>
#include <queue>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_face_algo.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_bnd.h>
#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>

//============================================================
//Meshing lidar image

bool load_lidar_label_file (const char* file, lidar_range_data* LRD)
{

  FILE* fp;
  if ((fp = std::fopen (file, "r")) == NULL) {
    ///vul_printf (std::cout, "  can't open input .XYZ file %s\n", file);
    return false; 
  }
  assert (LRD->data_.size() == 0);

  //Read in file header
  ///std::fscanf (fp, "# Lidar image labeling data file v 1.0.\n");
  std::fscanf (fp, "#3D Urban Model\n");
  int column, row;
  ///std::fscanf (fp, "%d\n", &column);
  ///std::fscanf (fp, "%d\n", &row);
  std::fscanf (fp, "Size: %d %d\n", &column, &row);
  

  //Read in (x, y, z) points.
  for (int y=0; y<row; y++) {
    std::vector<lidar_pixel*> pixel_row;
    LRD->data_.push_back (pixel_row);
    for (int x=0; x<column; x++) {
      int label;
      double height;
      float r, g, b;
      std::fscanf (fp, "%d %lf %f %f %f\n", &label, &height, &r, &g, &b);
      lidar_pixel* p = new lidar_pixel (label, height, r, g, b);
      LRD->data_[y].push_back (p);
    }
  }

  vul_printf (std::cout, " %d x %d point data loaded from %s.\n", 
              column, row, file);

  fclose (fp);
  return true;
}

