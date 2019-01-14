#ifndef dbsk2d_exception_h_
#define dbsk2d_exception_h_
//:
// \file
// \brief Exceptions thrown by dbsk2d, and a mechanism for turning them off.
//
//        The dilemma upon detecting an error is whether to abort the program or 
//        return a value that indicates that there is an error or leave the 
//        program in an illegal state or call an explicit error handling function.
//
//        Exceptions are needed because, although it is easy to detect the errors
//        incurred during the computation, it is not easy to deal with them.
//        The goal is to alert the user to such errors by throwing an exception
//        and hoping that the user can take appropriate action. This is a more
//        sensible way to deal with these problems rather than aborting the program.
//
//
//        There are several classes of errors that can occur:
//        (a) Memory allocation fails
//        (b) Constructor cannot instantiate an object due to invalid parameters
//        (c) Logical decisions based on numeric data which have accumlated errors 
//            cause the program to be in a logically inconsistent state.
//

// \author Amir Tamrakar
// \date 10/25/06

#include <string>
#include <cstdlib>
#include <iostream>
#if VCL_HAS_EXCEPTIONS
# include <stdexcept>
#endif


//: Throw an exception indicating a real problem.
// If exceptions have been disabled, this function
// may abort.
template <class T>
void dbsk2d_exception_error(T exception)
{
  std::cerr << "\nERROR: " << exception.what() << std::endl;
#if !defined DBSK2D_EXCEPTIONS_DISABLE  && VCL_HAS_EXCEPTIONS
  throw exception;
#else
  std::abort();
#endif
}

//: Throw an exception indicating a potential problem.
// If exceptions have been disabled, this function
// may return.
template <class T>
void dbsk2d_exception_warning(T exception)
{
  std::cerr << "\nWARNING: " << exception.what() << std::endl;
#if !defined DBSK2D_EXCEPTIONS_DISABLE  && VCL_HAS_EXCEPTIONS
  throw exception;
#endif
}

#if !VCL_HAS_EXCEPTIONS

  //: General purpose - a replacement for std::abort.
  // The only point of catching this exception, is to
  // give you a chance to save your data. If this exception
  // is thrown, then the program correctness is in doubt.
  class dbsk2d_exception_abort
  {
    std::string msg_;
   public:
    dbsk2d_exception_abort(const std::string& comment);
    const char * what() const {return msg_.c_str();}
  };

#else

  //: General purpose - a replacement for std::abort.
  // The only point of catching this exception, is to
  // give you a chance to save your data. If this exception
  // is thrown, then the program correctness is in doubt.
  class dbsk2d_exception_abort : public std::logic_error
  {
   public:
    dbsk2d_exception_abort(const std::string& comment);
    virtual ~dbsk2d_exception_abort() throw() {}
  };

#endif


#if !VCL_HAS_EXCEPTIONS

  //: Indicates a problem with the topology of shocks as it appears in the 
  //  the bnd_ishock_map. This is a very serious error
  class dbsk2d_exception_topology_error
  {
    std::string msg_;
   public:
    dbsk2d_exception_topology_error(const std::string &msg)
      : msg_(msg) {}
    const char * what() const {return msg_.c_str();}
  };

#else

  //: Indicates a problem with the topology of shocks as it appears in the 
  //  the bnd_ishock_map. This is a very serious error
  class dbsk2d_exception_topology_error: public std::logic_error
  {
   public:
    dbsk2d_exception_topology_error(const std::string &msg);
    virtual ~dbsk2d_exception_topology_error() throw() {}
  };

#endif

#define THROW_TOPOLOGY_EXCEPTION(C, M) \
  if (! (C) ) \
    throw dbsk2d_exception_topology_error( (M) );

#endif // dbsk2d_exception_h_

