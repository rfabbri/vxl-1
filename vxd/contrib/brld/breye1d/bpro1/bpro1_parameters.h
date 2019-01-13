// This is breye1/bpro1/bpro1_parameters.h
#ifndef bpro1_parameters_h_
#define bpro1_parameters_h_
//:
// \file
// \brief classes to represent parameters to bpro1 processes
// \author based on original code by Matt Leotta
// \date 7/1/2004
//
//
// \verbatim
//  Modifications
//    Matt Leotta  12/15/04     Migrated from vidpro
//    Amir Tamrakar 09/19/06    Added a parameter type for multiple choice options (bpro1_choice_param_type)
//
// \endverbatim

#include <string>
#include <sstream>
#include <iostream>
#include <cassert>
#include <vector>
#include <map>

#include <vbl/vbl_ref_count.h>

#include <bpro1/bpro1_parameters_sptr.h>


//: The abstract base class for a parameter
class bpro1_param
{
 public:

  //: Destructor
  virtual ~bpro1_param() {}

  //: Clone this parameter
  virtual bpro1_param * clone() const = 0;

  //: Return the parameter name
  std::string name() const { return name_; }
  //: Return the parameter description
  std::string description() const { return description_; }
  //: Returns true if the valid range of parameter values is bounded
  bool has_bounds() const { return has_bounds_; }

  //: Reset the value to its default
  virtual void reset() = 0;
  //: Attempt to set the value from the temporary reference
  virtual bool set_from_temp() = 0;

  //: Return a string representation of the current value
  virtual std::string value_str() const = 0;
  //: Return a string representation of the default value
  virtual std::string default_str() const = 0;
  //: Return a string representation of the minimum value
  virtual std::string min_str() const = 0;
  //: Return a string representation of the maximium value
  virtual std::string max_str() const = 0;

  //: Set the current value by parsing a string
  virtual bool parse_value_str(const std::string& input) = 0;

 protected:
  //: Constructor
  bpro1_param(bool has_bounds, const std::string& name, const std::string& desc)
   : has_bounds_(has_bounds), name_(name), description_(desc) {}


  //: Describes whether or not the parameter has bounds
  const bool has_bounds_;
  //: Name of the parameter
  const std::string name_;
  //: Description of the parameter
  const std::string description_;
};

//: Output stream operator for bpro1_params
std::ostream& operator<<(std::ostream& os, const bpro1_param& p);

//===========================================================================================

//: A Templated parameter class
template< class T >
class bpro1_param_type : public bpro1_param
{
 public:
  // Constructor - with bounds
  bpro1_param_type<T>(const std::string& name, const std::string& desc, const T& dflt, const T& min, const T& max)
   : bpro1_param(true, name, desc), value_(dflt), default_(dflt), temp_value_(dflt),
     min_value_(min), max_value_(max) { assert( min_value_ <= value_ && value_ <= max_value_ ); }

  // Constructor - without bounds
  bpro1_param_type<T>(const std::string& name, const std::string& desc, const T& dflt)
   : bpro1_param(false, name, desc), value_(dflt), default_(dflt), temp_value_(dflt),
     min_value_(dflt), max_value_(dflt) {}

  //: Accessor for the default value;
  T default_value() const { return default_; }
  //: Accessor for the default value;
  T min_value() const { return min_value_; }
  //: Accessor for the default value;
  T max_value() const { return max_value_; }
  
  //: Accessor for the current value;
  T value() const { return value_; }
  //: A reference for temporary storage of values
  T& temp_ref() { temp_value_ = value_; return temp_value_; }
  //: Attempt to set the value from the temporary reference
  bool set_from_temp() { return set_value(temp_value_); }
  //: Set the current value to \p val
  bool set_value( const T& val );

  //: Reset the value to its default
  virtual void reset() { value_ = default_; }

  //: Clone the parameter
  virtual bpro1_param * clone() const { return new bpro1_param_type<T>(*this); }

  //: Return a string representation of the current value
  virtual std::string value_str() const { return create_string(value_); }
  //: Return a string representation of the default value
  virtual std::string default_str() const { return create_string(default_); }
  //: Return a string representation of the minimum value
  virtual std::string min_str() const { return has_bounds_? create_string(min_value_) : ""; }
  //: Return a string representation of the maximium value
  virtual std::string max_str() const { return has_bounds_? create_string(max_value_) : ""; }

  //: Set the current value by parsing a string
  virtual bool parse_value_str(const std::string& input) { return set_value(parse_string(input)); }

 private:
  //: Create a string representation of the value
  std::string create_string(const T& val) const;

  //: Parse a string representation of the value
  T parse_string(const std::string& input) const;

  //: The current parameter value
  T value_;
  //: The default parameter value
  const T default_;
  //: A temporary value for assignments by reference
  T temp_value_;
  //: The minimum allowed parameter value
  const T min_value_;
  //: The maximum allowed parameter value
  const T max_value_;
};

//===========================================================================================

//: A parameter class for handling multiple choice parameters
class bpro1_choice_param_type : public bpro1_param_type<unsigned>
{
 public:
  // Constructor
  bpro1_choice_param_type(const std::string& name, const std::string& desc,  
  const std::vector<std::string>& choices, const unsigned def_val)
   : bpro1_param_type<unsigned>(name, desc, def_val, 0, choices.size()-1), choices_(choices) {}

  //: Clone the parameter
  virtual bpro1_param * clone() const { return new bpro1_choice_param_type(*this); }

  //: Accessor for the choice list;
  std::vector<std::string> & choices() { return choices_; }

 private:

  //: Mulitple choice list
  std::vector<std::string> choices_;
};

//===========================================================================================

//: This class maintains all parameters for a process
class bpro1_parameters : public vbl_ref_count
{
 public:

  //: Constructor
  bpro1_parameters();
  //: Destructor
  ~bpro1_parameters();

  //: Deep psuedo copy constructor
  bpro1_parameters( const bpro1_parameters_sptr& old_params);

  //: Returns true if a parameter exists with \p name
  bool valid_parameter( const std::string& name ) const;

  //: Returns true if a parameter exists with \p name and type \p T
  template<class T>
  bool valid_parameter_type( const std::string& name, const T&) const
  {
    std::map< std::string, bpro1_param* >::const_iterator 
      itr = name_param_map_.find( name );
    if( itr == name_param_map_.end() ) {
      return false; // Not Found
    }
    return (dynamic_cast<bpro1_param_type<T> *>(itr->second) != NULL);
  }

  //: Add a new parameter with no bounds
  template<class T>
  bool add( const std::string& desc, const std::string& name, const T& default_val )
  { return add(new bpro1_param_type<T>(name, desc, default_val)); }

  //: Add a new parameter with bounds
  template<class T>
  bool add( const std::string& desc, const std::string& name, const T& default_val,
            const T& min_val, const T& max_val )
  { return add(new bpro1_param_type<T>(name, desc, default_val, min_val, max_val)); }

  //: Add a new parameter for multiple choice options
  template<class T>
  bool add( const std::string& desc, const std::string& name, 
            const std::vector<std::string>& choices, const T& default_val )
  { return add(new bpro1_choice_param_type(name, desc, choices, default_val)); }

  //: Set the value of the existing parameter named \p name
  template<class T>
  bool set_value( const std::string& name , const T& value )
  {
    bpro1_param_type<T> * param = NULL;
    if( get_param(name, param) && param ){
      return param->set_value(value);
    }
    return false;
  }

  //: Return the value of the parameter named \p name by reference
  template<class T>
  bool get_value( const std::string& name , T& value ) const
  {
    bpro1_param_type<T> * param = NULL;
    if( get_param(name, param) && param ){
      value = param->value();
      return true;
    }
    return false;
  }

  //: Return the default value of the parameter named \p name by reference
  template<class T>
  bool get_default( const std::string& name , T& deflt ) const
  {
    bpro1_param_type<T> * param = NULL;
    if( get_param(name, param) && param ){
      deflt = param->default_value();
      return true;
    }
    return false;
  }

  //: Return the bounds of the parameter named \p name by reference
  template<class T>
  bool get_bounds( const std::string& name, T & min, T & max ) const
  {
    bpro1_param_type<T> * param = NULL;
    if( get_param(name, param) && param ){
      min = param->min_value();
      max = param->max_value();
      return true;
    }
    return false;
  }

  //: Reset all parameters to their default values
  bool reset_all();
  //: Reset the parameter named \p name to its default value
  bool reset( const std::string& name );

  //: Return a vector of base class pointers to the parameters
  std::vector< bpro1_param* > get_param_list() const;

  //: Return a vector of base class pointers to the parameters
  std::map< std::string , bpro1_param* >& get_param_map() { return name_param_map_; }

  //: Return the description of the parameter named \p name
  std::string get_desc( const std::string& name ) const;
  //: Print all parameters to \p os
  void print_all(std::ostream& os) const;
  
 private:
  //: Add parameter helper function
  bool add( bpro1_param* param );

  template<class T>
  bool get_param( const std::string& name, 
                  bpro1_param_type<T> * &param) const
  {
    std::map< std::string, bpro1_param* >::const_iterator 
      itr = name_param_map_.find( name );
    if( itr == name_param_map_.end() ) {
      return false; // Not Found
    }
    param = dynamic_cast<bpro1_param_type<T> *>(itr->second);
    if( !param )
      std::cerr << "WARNING: parameter \""<< name 
               << "\" was found but has incorrect type" << std::endl;
    return true;
  }

  //: The map from names to parameters
  std::map< std::string , bpro1_param* > name_param_map_;
  //: The vector of parameters in order of declaration
  std::vector< bpro1_param* > param_list_;
};

//: Set the value of the existing parameter named \p name
bool bpro1_parameters_set_value_from_str(bpro1_parameters& pars, const std::string& name , const std::string& value_str);

//===========================================================================================


//: A simple class to represent a file (for use with parameters)
class bpro1_filepath
{
 public:
  //: Constructor
  bpro1_filepath(const std::string& p = "", const std::string& e = "*")
   : path(p), ext(e) {}

  std::string path;
  std::string ext;
};

//: Less than operator for bpro1_filepath objects
bool operator<( const bpro1_filepath& lhs, const bpro1_filepath& rhs );
//: Less than or equal to operator for bpro1_filepath objects
bool operator<=( const bpro1_filepath& lhs, const bpro1_filepath& rhs );
//: Output stream operator for bpro1_filepath objects
std::ostream& operator<<( std::ostream& strm, const bpro1_filepath& fp );
//: Input stream operator for bpro1_filepath objects
std::istream& operator>>( std::istream& strm, const bpro1_filepath& fp );


#endif // bpro1_parameters_h_
