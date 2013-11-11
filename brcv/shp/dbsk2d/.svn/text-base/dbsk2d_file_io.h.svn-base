// This is dbsk2d/dbsk2d_file_io.h
#ifndef dbsk2d_file_io_h
#define dbsk2d_file_io_h

//:
// \file
// \brief This file contains functions to read/write files of shock 2d objects
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date 09/20/2005
//
// \verbatim
//  Modifications
//    Nhon Trinh   09/20/2005     Initial version
// \endverbatim


#include <vcl_vector.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

//: Place holder of all shock file i/o functions
class dbsk2d_file_io
{
public:
  //: Destructor
  ~dbsk2d_file_io(){};

  //: read a `.bnd' file and output vsol2D objects
  // return false if loading fails
  static bool load_bnd_v3_0(const vcl_string& filename, 
    vcl_vector<vsol_spatial_object_2d_sptr >& vsol_list);

  //: saves a list of vsol2D objects into a .bnd file
  // Only handle points, lines, and arcs and ignore the rest
  static bool save_bnd_v3_0(const vcl_string& filename, 
    const vcl_vector<vsol_spatial_object_2d_sptr >& vsol_list);


  

private:
  //: Constructor - depreciated because all functions are static
  dbsk2d_file_io(){};

};

#endif // dbsk2d_file_io_h
