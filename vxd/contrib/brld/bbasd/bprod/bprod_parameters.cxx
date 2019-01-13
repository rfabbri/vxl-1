// This is brl/bprod/bprod_parameters.cxx

//:
// \file

#include <bprod/bprod_parameters.h>

#include <utility>
#include <iostream>
#include <sstream>


//: Output stream operator for bprod_params
std::ostream& operator<<(std::ostream& os, const bprod_param& p)
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
bprod_parameters::bprod_parameters()
{
}


//: Destructor
bprod_parameters::~bprod_parameters()
{
  for( std::vector< bprod_param * >::iterator it = param_list_.begin();
       it != param_list_.end();
       it++ ) {
    delete (*it);
  }
}

//: Deep copy constructor
bprod_parameters::bprod_parameters(const bprod_parameters_sptr& old_params)
{
  for( std::vector< bprod_param * >::iterator it = old_params->param_list_.begin();
      it != old_params->param_list_.end();
      it++ ) {

    //deep copy this param
    bprod_param * new_param = (*it)->clone();

    param_list_.push_back( new_param );
    name_param_map_.insert( std::pair< std::string , bprod_param* >( new_param->name() , new_param ) );
  }
}


//: Returns true if a parameter exists with \p flag
bool
bprod_parameters::valid_parameter( const std::string& name ) const
{
  std::map< std::string , bprod_param * >::const_iterator itr = name_param_map_.find( name );
  return (itr != name_param_map_.end());
}


//: Reset all parameters to their default values
bool
bprod_parameters::reset_all()
{
  for( std::vector< bprod_param * >::iterator it = param_list_.begin();
       it != param_list_.end();
       it++ ) {
    (*it)->reset();
  }
  return true;
}


//: Reset the parameter named \p name to its default value
bool
bprod_parameters::reset( const std::string& name )
{
  std::map< std::string , bprod_param * >::iterator it = name_param_map_.find( name );
  if( it == name_param_map_.end() ) {
    return false;
  }

  it->second->reset();

  return true;
}


//: Return a vector of base class pointers to the parameters
std::vector< bprod_param* >
bprod_parameters::get_param_list() const
{
  return param_list_;
}


//: Return the description of the parameter named \p name
std::string
bprod_parameters::get_desc( const std::string& name ) const
{
  std::map< std::string , bprod_param * >::const_iterator it = name_param_map_.find( name );
  if( it == name_param_map_.end() ) {
    return "";
  }
  return it->second->description();
}


//: Print all parameters to \p os
void
bprod_parameters::print_all(std::ostream& os) const
{
  for( std::vector< bprod_param * >::const_iterator it = param_list_.begin();
       it != param_list_.end();
       it++ ) {
    os << *it;
  }
}


//: Add parameter helper function
bool 
bprod_parameters::add( bprod_param* param )
{
  if( !param )
    return false;
  std::string name = param->name();
  std::string desc = param->description();
  if( name_param_map_.find( name ) != name_param_map_.end() ||
      desc == "" || name == "" ) {
    delete param;
    return false;
  }

  param_list_.push_back( param );
  name_param_map_.insert( std::pair< std::string , bprod_param* >( name , param ) );

  return true;
}

//===========================================================================================

//: Less than operator for bprod_filepath objects
bool operator<( const bprod_filepath& lhs, const bprod_filepath& rhs )
{
  return lhs.path < rhs.path;
}

//: Output stream operator for bprod_filepath objects
std::ostream& operator<<( std::ostream& strm, const bprod_filepath& fp )
{
  strm << fp.path << '\n' << fp.ext << std::ends;
  return strm;
}

//: Input stream operator for bprod_filepath objects
std::istream& operator>>( std::istream& strm, const bprod_filepath& fp )
{
  strm >> fp.path >> fp.ext;
  return strm;
}

