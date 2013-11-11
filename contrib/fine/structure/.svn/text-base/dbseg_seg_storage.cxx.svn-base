// This is contrid/fine/gui/dbseg_seg_storage.cxx

//:
// \file

#include "dbseg_seg_storage.h"

/*#include <vcl_utility.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>
#include <vsl/vsl_map_io.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_triangle_2d.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_group_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vdgl/vdgl_digital_curve.h>
*/

//bool dbseg_seg_storage::registered_ = false;

//: Constructor
dbseg_seg_storage::dbseg_seg_storage()
{
    object_=0;
}

dbseg_seg_storage::dbseg_seg_storage(dbseg_seg_object_base *object): object_(object) {}

//: Destructor
dbseg_seg_storage::~dbseg_seg_storage()
{
    delete object_;
}

dbseg_seg_object_base* dbseg_seg_storage::get_object() {
    return object_;

}

  
//: Register vsol_spatial_object_2d types for I/O

/*void 
dbseg_seg_storage::register_binary_io() const
{
  if(!registered_){
    vsl_add_to_binary_loader(vsol_point_2d());
    vsl_add_to_binary_loader(vsol_line_2d());
    vsl_add_to_binary_loader(vsol_polyline_2d());
    vsl_add_to_binary_loader(vsol_polygon_2d());
    vsl_add_to_binary_loader(vsol_rectangle_2d());
    vsl_add_to_binary_loader(vsol_triangle_2d());
    vsl_add_to_binary_loader(vsol_conic_2d());
    vsl_add_to_binary_loader(vsol_group_2d());
    vsl_add_to_binary_loader(vsol_digital_curve_2d());
    vsl_add_to_binary_loader(vdgl_digital_curve());

    registered_ = true;
  }
}*/


//: Create a copy of the object on the heap.
// The caller is responsible for deletion

bpro1_storage* 
dbseg_seg_storage::clone() const
{
  return new dbseg_seg_storage(*this);
}


//: Return IO version number;
short 
dbseg_seg_storage::version() const
{
  return 3;
}


//: Binary save self to stream.
/*
void 
dbseg_seg_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  vsl_b_write(os, vsol_map_);
  vsl_b_write(os, attr_map_);
}


//: Binary load self from stream.
void 
dbseg_seg_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
  {
    int count; // not used anymore
    bpro1_storage::b_read(is);
    vsl_b_read(is, count);
    vsl_b_read(is, vsol_map_);
    break;
  }
  case 2:
  {
    bpro1_storage::b_read(is);
    vsl_b_read(is, vsol_map_);
    break;
  }
  case 3:
  {
    bpro1_storage::b_read(is);
    vsl_b_read(is, vsol_map_);
    vsl_b_read(is, attr_map_);
    if(attr_map_.size()>0)
      has_attributes_ = true;
    break;
  }

  default:
    vcl_cerr << "I/O ERROR: dbseg_seg_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
*/

//: Remove all vsol2D


/*//: Add a vector of vsol2D objects to the group named \p which
void
dbseg_seg_storage::add_objects( const vcl_vector< vsol_spatial_object_2d_sptr >& objects,  
                                    const vcl_string& which)
{
  data_map::iterator result = vsol_map_.find( which );

  if( result == vsol_map_.end() ) {
    typedef vcl_pair<vcl_string, vcl_vector<vsol_spatial_object_2d_sptr> > data_pair;
    vsol_map_.insert( data_pair( which, objects ) );
  }
  else {
    for( vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator 
         it = objects.begin();  it != objects.end();  ++it )
      result->second.push_back( *it );
  }
}
*/

//: Add an object to the group named \p which
void
dbseg_seg_storage::add_object(dbseg_seg_object_base* object)
{
    object_=object; //*which, object));
    //vcl_cout << "area: " << object_->get_area(6) << vcl_endl;
}


//: Search for the object in all groups and remove it
// \retval The name of the group removed from of the empty string if not found

/*
bool 
dbseg_seg_storage::remove_object( const seg_object& object )
{
  for( data_map::const_iterator it = object_list_.begin();
       it != object_list.end();  ++it ) {
    if(remove_object(object, it->first))
      return true;
  }
  return false;
}
*/

//: Search for the object in the given group only and remove it
// \return true if the object was successfully removed
// \return false if the object was not found



//: Return a vector of all vsol objects in the group named \p which



/*
//: Return a vector of all vsol objects in all groups
// note that an object can be duplicated if it exists in separate groups
vcl_vector< vsol_spatial_object_2d_sptr > 
dbseg_seg_storage::all_data() const
{
  vcl_vector<vsol_spatial_object_2d_sptr> all_data;
  for( data_map::const_iterator it = vsol_map_.begin();
       it != vsol_map_.end();  ++it ) 
  {
    for( vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator 
         it2 = it->second.begin();  it2 != it->second.end();  ++it2 )
    { 
      all_data.push_back( *it2 );
    }
  }
  return all_data;
}
*/

//: Return the names of all groups 


/*
// Merge two storage instances 
bpro1_storage* dbseg_seg_storage::merge(const bpro1_storage* sa,
                                           const bpro1_storage* sb)
{
  if(!sa||!sb)
    return 0;
  if(!((sa->type()==this->type())&&(sa->type()==this->type())))
    return 0;
  dbseg_seg_storage* vsola = (dbseg_seg_storage*)sa;
  dbseg_seg_storage* vsolb = (dbseg_seg_storage*)sb;

 dbseg_seg_storage* ret = new dbseg_seg_storage();

  data_map::const_iterator ita = vsola->vsol_map_.begin(), 
    itb = vsolb->vsol_map_.begin(); 
  for( ; (ita != vsola->vsol_map_.end())&&(itb != vsolb->vsol_map_.end());
       ++ita, ++itb ) 
  {
    //merge the names
    const vcl_string& namea = ita->first, nameb = itb->first;
    vcl_string merged_name = namea + '+' + nameb;

    //merge the spatial object sets
    vcl_vector<vsol_spatial_object_2d_sptr> sosa = ita->second,
      sosb = itb->second, merged_sos;
    vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator sita =
      sosa.begin(), sitb = sosb.begin();
    for(; (sita!=sosa.end())&&(sitb!=sosb.end()); ++sita, ++sitb)
      merged_sos.push_back(*sita);  merged_sos.push_back(*sitb);
    
    ret->add_objects(merged_sos, merged_name);
  }
  return ret;
}

void dbseg_seg_storage::
add_objects( const vcl_vector< vsol_spatial_object_2d_sptr >& objects ,
             const vcl_vector< double > attributes, 
             const vcl_string& which)
{
  assert(objects.size()==attributes.size());

  //Add the objects into the data map
  this->add_objects(objects, which);

    //Add the attributes into the group name map
  attribute_map::iterator result = attr_map_.find( which );

  if( result == attr_map_.end() ) {
    typedef vcl_pair<vcl_string, vcl_vector<double> > attr_pair;
    attr_map_.insert( attr_pair( which, attributes ) );
  }
  else {
    for( vcl_vector<double>::const_iterator 
         it = attributes.begin();  it != attributes.end();  ++it )
      result->second.push_back( *it );
  }
  has_attributes_ = true;
}

//: Add an object to the group named \p which
void 
dbseg_seg_storage::add_object( const vsol_spatial_object_2d_sptr& object, 
                                   const double attr,
                                   const vcl_string& which)
{
  this->add_object(object, which);
  attr_map_[which].push_back(attr);  
}

//: Return a vector of attributes in the group named \p which
vcl_vector<double> dbseg_seg_storage::
attributes_named(const vcl_string& which) const
{
  if(!has_attributes_)
    return vcl_vector<double>();
  
  attribute_map::const_iterator it = attr_map_.find( which );

  if( it == attr_map_.end() ) {
    return vcl_vector< double >();//an empty vector of attributes
  }
  return it->second;
}

//: Return a vector of attributes associated with all vsol objects
vcl_vector< double > dbseg_seg_storage::all_attributes() const
{
  if(!has_attributes_)
    return vcl_vector<double>();

  vcl_vector<double> all_data;
  if(attr_map_.size()==0)
    return all_data;

  for( attribute_map::const_iterator it = attr_map_.begin();
       it != attr_map_.end();  ++it ) 
  {
    for( vcl_vector<double>::const_iterator 
         it2 = it->second.begin();  it2 != it->second.end();  ++it2 )
    { 
      all_data.push_back( *it2 );
    }
  }
  return all_data;
}
*/



