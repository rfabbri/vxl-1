// This is dbrec3d_part_hierarchy.h
//#ifndef dbrec3d_part_hierarchy_h
//#define dbrec3d_part_hierarchy_h
//
//:
// \file
// \brief Class to represent part hierarchies as the meta-structure of recognition grammar
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 22, 2009
//
// \verbatim
//  Modifications
//           2-Jun-2010- Isabel Restrepo: Class borrowed from dbrec. Adapted to fit design of dbrec3d
// \endverbatim
//
//#include <vbl/vbl_ref_count.h>
//
//
//: Hierarchy contains a root node per class, its just a container of roots
//class dbrec3d_hierarchy : public vbl_ref_count {
//public:
//  //dbrec3d_hierarchy() : part_table_(100, vcl_pair<dbrec_part_sptr, int>(dbrec_part_sptr(0), -1) ), table_populated_(false) {}
//  
//  //: return the part pointer with the given type id
//  dbrec_part_sptr get_part(int part_id) const;
//  
//  //: return a list of all parts in the hierarchy (usually used for visualizations)
//  void get_all_parts(vcl_vector<dbrec_part_sptr>& parts) const;
//  
//  //: get_parts method uses the part table for quick access to the depths of the parts, it calls the following method if parts are not cached yet
//  void populate_table() const;
//  
//  //: register all the current parts to the type id factory to support creation of new unique type ids
//  void register_parts(dbrec_type_id_factory* ins) const;
//  
//protected:
//  vcl_vector<dbrec_part_sptr> roots_;
//  
//  vcl_vector<dbrec_part_sptr>::iterator it_;
//  
//  //: we need fast access to the parts so cache them in a direct-address hash table using their part_id as the key (flyweight pattern: we keep a single instance of the part, accessed via its type throughout the program, but only hierarchy acts as a factory class)
//  //  the max number of parts is around 100, so reserve space for 100, but use an array so it could grow
//  //mutable vcl_map<unsigned, dbrec_part_sptr> part_map_;
//  // we also cache the depths of the parts for quick access later
//  vcl_vector<vcl_pair<dbrec_part_sptr, int> > part_table_;
//  //bool table_populated_;
//  
//};
//
//#endif
