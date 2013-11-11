// This is brl/bpro/bpro_parameters.cxx

//:
// \file

#include <bpro/bpro_parameters.h>

#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>


//: Output stream operator for bpro_params
vcl_ostream& operator<<(vcl_ostream& os, const bpro_param& p)
{
  os << "parameter{\n  Description: " << p.description();
  if(p.has_bounds())
    os << "\n  Range: " << p.min_str() << " to " << p.max_str();
  os << "\n  Default: " << p.default_str();
  os << "\n  Value: " << p.value_str() << "\n}\n";

  return os;
}

//===========================================================================================

//: Constructor
bpro_parameters::bpro_parameters()
{
}


//: Destructor
bpro_parameters::~bpro_parameters()
{
  for( vcl_vector< bpro_param * >::iterator it = param_list_.begin();
       it != param_list_.end();
       it++ ) {
    delete (*it);
  }
}

//: Deep copy constructor
bpro_parameters::bpro_parameters(const bpro_parameters_sptr& old_params)
{
  for( vcl_vector< bpro_param * >::iterator it = old_params->param_list_.begin();
      it != old_params->param_list_.end();
      it++ ) {

    //deep copy this param
    bpro_param * new_param = (*it)->clone();

    param_list_.push_back( new_param );
    name_param_map_.insert( vcl_pair< vcl_string , bpro_param* >( new_param->name() , new_param ) );
  }
}


//: Returns true if a parameter exists with \p flag
bool
bpro_parameters::valid_parameter( const vcl_string& name ) const
{
  vcl_map< vcl_string , bpro_param * >::const_iterator itr = name_param_map_.find( name );
  return (itr != name_param_map_.end());
}


//: Reset all parameters to their default values
bool
bpro_parameters::reset_all()
{
  for( vcl_vector< bpro_param * >::iterator it = param_list_.begin();
       it != param_list_.end();
       it++ ) {
    (*it)->reset();
  }
  return true;
}


//: Reset the parameter named \p name to its default value
bool
bpro_parameters::reset( const vcl_string& name )
{
  vcl_map< vcl_string , bpro_param * >::iterator it = name_param_map_.find( name );
  if( it == name_param_map_.end() ) {
    return false;
  }

  it->second->reset();

  return true;
}


//: Return a vector of base class pointers to the parameters
vcl_vector< bpro_param* >
bpro_parameters::get_param_list() const
{
  return param_list_;
}


//: Return the description of the parameter named \p name
vcl_string
bpro_parameters::get_desc( const vcl_string& name ) const
{
  vcl_map< vcl_string , bpro_param * >::const_iterator it = name_param_map_.find( name );
  if( it == name_param_map_.end() ) {
    return "";
  }
  return it->second->description();
}


//: Print all parameters to \p os
void
bpro_parameters::print_all(vcl_ostream& os) const
{
  for( vcl_vector< bpro_param * >::const_iterator it = param_list_.begin();
       it != param_list_.end();
       it++ ) {
    os << *it;
  }
}


//: Add parameter helper function
bool 
bpro_parameters::add( bpro_param* param )
{
  if( !param )
    return false;
  vcl_string name = param->name();
  vcl_string desc = param->description();
  if( name_param_map_.find( name ) != name_param_map_.end() ||
      desc == "" || name == "" ) {
    delete param;
    return false;
  }

  param_list_.push_back( param );
  name_param_map_.insert( vcl_pair< vcl_string , bpro_param* >( name , param ) );

  return true;
}

//===========================================================================================

//: Less than operator for bpro_filepath objects
bool operator<( const bpro_filepath& lhs, const bpro_filepath& rhs )
{
  return lhs.path < rhs.path;
}

//: Less than or equal to operator for bpro_filepath objects
bool operator<=( const bpro_filepath& lhs, const bpro_filepath& rhs )
{
  return lhs.path <= rhs.path;
}

//: Output stream operator for bpro_filepath objects
vcl_ostream& operator<<( vcl_ostream& strm, const bpro_filepath& fp )
{
  strm << fp.path << '\n' << fp.ext << vcl_ends;
  return strm;
}

//: Input stream operator for bpro_filepath objects
vcl_istream& operator>>( vcl_istream& strm, const bpro_filepath& fp )
{
  strm >> fp.path >> fp.ext;
  return strm;
}

