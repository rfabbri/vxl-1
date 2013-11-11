//this is /contrib/bm/dsm/io/dsm_io_target_neighborhood_map.h
#ifndef DSM_IO_TARGET_NEIGHBORHOOD_MAP_H_
#define DSM_IO_TARGET_NEIGHBORHOOD_MAP_H_
//:
// \file
// \brief Binary IO for dsm_target_neighborhood_map class
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  May 30, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include<vsl/vsl_binary_io.h>
#include<vcl_iostream.h>
#include"../dsm_target_neighborhood_map.h"

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm_target_neighborhood_map& target_neighborhood_map);
void vsl_b_write(vsl_b_ostream &os, dsm_target_neighborhood_map const& target_neighborhood_map);
void vsl_b_read(vsl_b_istream &is, dsm_target_neighborhood_map* &p);
void vsl_b_write(vsl_b_ostream &os, const dsm_target_neighborhood_map* p);
void vsl_print_summary(vcl_ostream& os, const dsm_target_neighborhood_map *p);

#endif //DSM_IO_TARGET_NEIGHBORHOOD_MAP_H_