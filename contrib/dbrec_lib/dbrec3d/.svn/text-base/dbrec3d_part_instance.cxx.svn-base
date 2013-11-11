//:
// \file
// \author Isabel Restrepo
// \date 3-Jun-2010

#include "dbrec3d_part_instance.h"

vcl_ostream& operator << (vcl_ostream& os, const dbrec3d_part_instance &part)
{
  os << "This is dbrec3d_part_instance\n";
  os << "type id: " << part.type_id() << "\n";
  os << "location: " << part.location() << "\n";
  os << "response: " << part.posterior() <<vcl_endl;
  
  return os;
}

void vsl_b_write(vsl_b_ostream & os, dbrec3d_part_instance const &part)
{
  vsl_b_write(os, part.version_no());
  vsl_b_write(os, part.type_id());
  vsl_b_write(os, part.location());
  vsl_b_write(os, part.posterior());
  
}

void vsl_b_write(vsl_b_ostream & os, dbrec3d_part_instance const * &part)
{
  if (part) {
    vsl_b_write(os, *part);
  }
}

void vsl_b_read(vsl_b_istream & is, dbrec3d_part_instance &part)
{
  if (!is) return;
  
  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
    {
      
      int type_id;
      vsl_b_read(is, type_id);
      part.set_type_id(type_id);
      vgl_point_3d<double> location;
      vsl_b_read(is, location);
      part.set_location(location);
      float posterior;
      vsl_b_read(is,posterior);
      part.set_posterior(posterior);
      
      break;
    }
      
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, dbrec3d_part_instance&)\n"
      << "           Unknown version number "<< version << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

void vsl_b_read(vsl_b_istream & is, dbrec3d_part_instance *&part)
{
  vsl_b_read(is, *part);
}

vcl_ostream& operator << (vcl_ostream& os, const dbrec3d_pair_composite_instance &part)
{
  os << "This is dbrec3d_pair_composite_instance\n";
  os << "type id: " << part.type_id() << "\n";
  os << "location: " << part.location() << "\n";
  os << "response: " << part.posterior() <<"\n";
  os << "location child 1: " << part.location_child1() <<  "\n";
  os << "location child 2: " << part.location_child2() <<  vcl_endl;
  return os;
}

void vsl_b_write(vsl_b_ostream & os, dbrec3d_pair_composite_instance const &part)
{
  vsl_b_write(os, part.version_no());
  vsl_b_write(os, part.type_id());
  vsl_b_write(os, part.location());
  vsl_b_write(os, part.posterior());
  vsl_b_write(os, part.location_child1());
  vsl_b_write(os, part.location_child2());

  
}

void vsl_b_write(vsl_b_ostream & os, dbrec3d_pair_composite_instance const * &part)
{
  if (part) {
    vsl_b_write(os, *part);
  }
}

void vsl_b_read(vsl_b_istream & is, dbrec3d_pair_composite_instance &part)
{
  if (!is) return;
  
  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
    {
      
      int type_id;
      vsl_b_read(is, type_id);
      part.set_type_id(type_id);
      vgl_point_3d<double> location;
      vsl_b_read(is, location);
      part.set_location(location);
      float posterior;
      vsl_b_read(is,posterior);
      part.set_posterior(posterior);
      vgl_point_3d<double> location_child1;
      vsl_b_read(is, location_child1);
      part.set_location_child1(location_child1);
      vgl_point_3d<double> location_child2;
      vsl_b_read(is, location_child2);
      part.set_location_child2(location_child2);
      break;
    }
      
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, dbrec3d_part_instance&)\n"
      << "           Unknown version number "<< version << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

void vsl_b_read(vsl_b_istream & is, dbrec3d_pair_composite_instance *&part)
{
  vsl_b_read(is, *part);
}


//
//#define DBREC3D_PART_INSTANCE_INSTANTIATE(T) \
//template class dbrec3d_part_instance<T >; \
//template void vsl_b_write(vsl_b_ostream &, dbrec3d_part_instance<T > const &); \
//template void vsl_b_write(vsl_b_ostream &, dbrec3d_part_instance<T > const *&); \
//template void vsl_b_read(vsl_b_istream &, dbrec3d_part_instance<T > &); \
//template void vsl_b_read(vsl_b_istream &, dbrec3d_part_instance<T > *&); \
//template vcl_ostream& operator << (vcl_ostream&, const dbrec3d_part_instance<T >&)
//#endif
