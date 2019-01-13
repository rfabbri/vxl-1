// This is brld/bpro1/bpro1_parameters.cxx

//:
// \file

#include <bpro1/bpro1_parameters.h>

#include <utility>
#include <iostream>
#include <sstream>


//: Output stream operator for bpro1_params
std::ostream& operator<<(std::ostream& os, const bpro1_param& p)
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
bpro1_parameters::bpro1_parameters()
{
}


//: Destructor
bpro1_parameters::~bpro1_parameters()
{
  for( std::vector< bpro1_param * >::iterator it = param_list_.begin();
       it != param_list_.end();
       it++ ) {
    delete (*it);
  }
}

//: Deep copy constructor
bpro1_parameters::bpro1_parameters(const bpro1_parameters_sptr& old_params)
{
  for( std::vector< bpro1_param * >::iterator it = old_params->param_list_.begin();
      it != old_params->param_list_.end();
      it++ ) {

    //deep copy this param
    bpro1_param * new_param = (*it)->clone();

    param_list_.push_back( new_param );
    name_param_map_.insert( std::pair< std::string , bpro1_param* >( new_param->name() , new_param ) );
  }
}


//: Returns true if a parameter exists with \p flag
bool
bpro1_parameters::valid_parameter( const std::string& name ) const
{
  std::map< std::string , bpro1_param * >::const_iterator itr = name_param_map_.find( name );
  return (itr != name_param_map_.end());
}


//: Reset all parameters to their default values
bool
bpro1_parameters::reset_all()
{
  for( std::vector< bpro1_param * >::iterator it = param_list_.begin();
       it != param_list_.end();
       it++ ) {
    (*it)->reset();
  }
  return true;
}


//: Reset the parameter named \p name to its default value
bool
bpro1_parameters::reset( const std::string& name )
{
  std::map< std::string , bpro1_param * >::iterator it = name_param_map_.find( name );
  if( it == name_param_map_.end() ) {
    return false;
  }

  it->second->reset();

  return true;
}


//: Return a vector of base class pointers to the parameters
std::vector< bpro1_param* >
bpro1_parameters::get_param_list() const
{
  return param_list_;
}


//: Return the description of the parameter named \p name
std::string
bpro1_parameters::get_desc( const std::string& name ) const
{
  std::map< std::string , bpro1_param * >::const_iterator it = name_param_map_.find( name );
  if( it == name_param_map_.end() ) {
    return "";
  }
  return it->second->description();
}


//: Print all parameters to \p os
void
bpro1_parameters::print_all(std::ostream& os) const
{
  for( std::vector< bpro1_param * >::const_iterator it = param_list_.begin();
       it != param_list_.end();
       it++ ) {
    os << *(*it);
  }
}


//: Add parameter helper function
bool 
bpro1_parameters::add( bpro1_param* param )
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
  name_param_map_.insert( std::pair< std::string , bpro1_param* >( name , param ) );

  return true;
}

//: Set the value of the existing parameter named \p name
bool bpro1_parameters_set_value_from_str(bpro1_parameters& pars, const std::string& name , const std::string& value_str)
{
  std::map< std::string , bpro1_param* >::iterator it = pars.get_param_map().find(name);
  if (it == pars.get_param_map().end()) {
    return false;
  }
  
  bpro1_param* param = it->second;
  if( param ){
    return param->parse_value_str(value_str);
  }

  return false;
}



//===========================================================================================

//: Less than operator for bpro1_filepath objects
bool operator<( const bpro1_filepath& lhs, const bpro1_filepath& rhs )
{
  return lhs.path < rhs.path;
}

//: Less than or equal to operator for bpro1_filepath objects
bool operator<=( const bpro1_filepath& lhs, const bpro1_filepath& rhs )
{
  return lhs.path <= rhs.path;
}

//: Output stream operator for bpro1_filepath objects
std::ostream& operator<<( std::ostream& strm, const bpro1_filepath& fp )
{
  strm << fp.path << '\n' << fp.ext << std::ends;
  return strm;
}

//: Input stream operator for bpro1_filepath objects
std::istream& operator>>( std::istream& strm, const bpro1_filepath& fp )
{
  strm >> fp.path >> fp.ext;
  return strm;
}

