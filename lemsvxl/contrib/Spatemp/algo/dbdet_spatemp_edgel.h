// This is dbdet_spatemp_edgel.h
#ifndef dbdet_spatemp_edgel_h
#define dbdet_spatemp_edgel_h
//:
//\file
//\brief Spatio-temporal Edgel class for storing curvelts in spatial and temporal domain.
//\author Vishal Jain
//\date 12/15/07
//
//\verbatim
//\endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_deque.h>
#include <vcl_list.h>

#include <vgl/vgl_point_2d.h>
#include <dbdet/dbdet_edgel.h>

//forward class definitions
class dbdet_spatemp_edgel;
class dbdet_curvelet;

//useful type definitions
typedef vcl_list<dbdet_spatemp_edgel* > edgel_list;
typedef vcl_list<dbdet_spatemp_edgel* >::iterator edgel_list_iter;
typedef vcl_list<dbdet_spatemp_edgel* >::const_iterator edgel_list_const_iter;

typedef vcl_list<dbdet_curvelet* > t_curvelet_list;
typedef vcl_list<dbdet_curvelet* >::iterator curvelet_list_iter;
typedef vcl_list<dbdet_curvelet* >::const_iterator curvelet_list_const_iter;

//: edgel class: contains pt, tangent and collection of all the groupings around it
class dbdet_spatemp_edgel:public dbdet_edgel
{
public:

  //: the set of local curve hypotheses through this edgel
  t_curvelet_list curvelets;

};


#endif // dbdet_spatemp_edgel_h
