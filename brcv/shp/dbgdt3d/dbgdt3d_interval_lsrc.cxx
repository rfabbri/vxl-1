//: Dec 14, 2005 MingChing Chang
//  

#include <dbgdt3d/dbgdt3d_interval.h>
#include <dbgdt3d/dbgdt3d_vertex.h>
#include <dbgdt3d/dbgdt3d_path.h>

gdt_lsrc_interval::gdt_lsrc_interval (const dbmsh3d_halfedge* he,
                     const dbmsh3d_gdt_vertex_3d* srcline_s, 
                     const dbmsh3d_gdt_vertex_3d* srcline_e, 
                     const double stau, const double etau,
                     const gdt_interval* prev,
                     const double Hs, const double Ls,
                     const double He, const double Le) :
  gdt_interval (ITYPE_LSRC, stau, etau, he, srcline_s, Hs, Ls, prev)
{
  srcline_e_  = (dbmsh3d_gdt_vertex_3d*) srcline_e;
  he_         = (double) He;
  le_         = (double) Le;
}
