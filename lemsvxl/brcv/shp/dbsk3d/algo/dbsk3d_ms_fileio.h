#ifndef dbsk3d_ms_fileio_h_
#define dbsk3d_ms_fileio_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbsk3d/dbsk3d_ms_fileio.h
//:
// \file
// \brief Coarse-scale medial scaffold file (.ms) I/O
//
// \author
//  MingChing Chang  Jan 11, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/dbsk3d_sg_sa.h>


//: File I/O for medial scaffold file (*.cms)
bool load_from_cms (dbsk3d_ms_hypg* ms_hypg, const char *file, const int verbose = 1);
void save_to_cms (dbsk3d_ms_hypg* ms_hypg, const char *file, const int verbose = 1);

//: File I/O for stand alone shock graph file (*.sg)
void sc_save_text_file_sg (dbsk3d_sg_sa* sg_sa, 
                           dbsk3d_ms_curve* scurve, FILE* fp);
void sc_load_text_file_sg (dbsk3d_sg_sa* sg_sa, 
                           dbsk3d_ms_curve* scurve, FILE* fp);

void save_to_sg (dbsk3d_sg_sa* sg_sa, const char *file);
bool load_from_sg (dbsk3d_sg_sa* sg_sa, const char *file);


#endif

