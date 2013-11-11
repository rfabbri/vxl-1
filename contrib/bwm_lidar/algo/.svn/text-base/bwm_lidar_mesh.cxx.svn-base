// This is bwm_lidar_mesh.cxx
//   Ming-Ching Chang
//   Dec 13, 2007.
//:
// \file

#include "bwm_lidar_mesh.h"

#include <vcl_list.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_utility.h>
#include <vcl_queue.h>
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
  if ((fp = vcl_fopen (file, "r")) == NULL) {
    ///vul_printf (vcl_cout, "  can't open input .XYZ file %s\n", file);
    return false; 
  }
  assert (LRD->data_.size() == 0);

  //Read in file header
  ///vcl_fscanf (fp, "# Lidar image labeling data file v 1.0.\n");
  vcl_fscanf (fp, "#3D Urban Model\n");
  int column, row;
  ///vcl_fscanf (fp, "%d\n", &column);
  ///vcl_fscanf (fp, "%d\n", &row);
  vcl_fscanf (fp, "Size: %d %d\n", &column, &row);
  

  //Read in (x, y, z) points.
  for (int y=0; y<row; y++) {
    vcl_vector<lidar_pixel*> pixel_row;
    LRD->data_.push_back (pixel_row);
    for (int x=0; x<column; x++) {
      int label;
      double height;
      float r, g, b;
      vcl_fscanf (fp, "%d %lf %f %f %f\n", &label, &height, &r, &g, &b);
      lidar_pixel* p = new lidar_pixel (label, height, r, g, b);
      LRD->data_[y].push_back (p);
    }
  }

  vul_printf (vcl_cout, " %d x %d point data loaded from %s.\n", 
              column, row, file);

  fclose (fp);
  return true;
}

