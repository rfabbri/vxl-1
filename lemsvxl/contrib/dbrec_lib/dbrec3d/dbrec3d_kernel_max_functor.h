// This is dbrec3d_kernel_max_functor.h
#ifndef dbrec3d_kernel_max_functor_h
#define dbrec3d_kernel_max_functor_h

//:
// \file
// \brief A set of specialized functors used to perfom non maxima suppression of parts.
// \author Isabel Restrepo mir@lems.brown.edu
// \date  12-Aug-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vgl/vgl_box_3d.h>

template <class T>
class dbrec3d_kernel_max_functor{
  
public:
  //: Default constructor
  dbrec3d_kernel_max_functor(){ init_called_ = false;  max_pos_=0;}
  
  //: Initializes all local class variables
  bool init(T& part) 
  {
    if(part.type_id()<0)
      return false;
    
    float max_pos_ = 0.0f;
    central_part_ = part;
    init_called_ = true;
    return true;
  }
  
  //: Apply a given operation to value val, depending on the dispatch character
  void apply(T& part);
  
  //: Returns the final operation of this functor
  T result();
  
protected:
  //: max_value over neighborhood. This is a float because we are evaluating posteriors
  float max_pos_;
  
  //: This part is stored because we need it to be able to define the result
  T central_part_;
  
  //: A flag to warn the userd that this functor hasn't been initialized
  bool init_called_;
  
  vgl_box_3d<double> roi_;
  
};

//: Apply a new value
template<class T>
void dbrec3d_kernel_max_functor<T>::apply(T& part)
{
  //missing to check that this is not the central part
  if(!init_called_){
    vcl_cerr << "Cannot apply functor without initializing first" << vcl_endl;
    return;
  }
  if(part.type_id()<0)
    return;
  
  float p = part.posterior();
  
  if((p - max_pos_) > 1.0e-7)
    max_pos_ = p;
    
  return;
}

//: Compute result of the functor. If current part is more probable than its neighbors keep it otherwise, make itself invalid. 
template<class T>
T dbrec3d_kernel_max_functor<T>::result()
{
  T result;
  
  if(central_part_.posterior() >= max_pos_  && central_part_.posterior()> 1e-7) 
    result = central_part_;
  
  init_called_ = false;
  max_pos_ = 0.0f;
  return result;
}

#endif


