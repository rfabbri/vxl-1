// This is brcv/seg/dbsta/dbsta_mixture.h
#ifndef dbsta_mixture_h_
#define dbsta_mixture_h_

//:
// \file
// \brief A mixture of distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/12/05
//
// \verbatim
//  Modifications
// \endverbatim


#include "dbsta_distribution.h"
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_typeinfo.h>

//: A mixture of distributions
template <class T>
class dbsta_mixture : public dbsta_distribution<T>
{
 private:
  //: A struct to hold the component distributions and weights
  // This class is private and should not be used outside of the mixture.
  // Dynamic memory is used to allow for polymorphic distributions.
  // However, this use of memory is self-contained and private so the user
  // should not be able to introduce a memory leak
  struct component : public vbl_ref_count
  {
    //: Constructor
    component(): distribution(NULL), weight(T(0)) {}
    //: Constructor
    component(const dbsta_distribution<T>& dist, 
              const T& w = T(0) ) : distribution(dist.clone()), weight(w) {}    
    //: Copy constructor
    component(const component& other) 
     : distribution((other.distribution)?other.distribution->clone():NULL), 
       weight(other.weight) {}

    //: Destructor
    ~component() { delete distribution; }

    //: Used to sort by decreasing weight
    bool operator< (const component& rhs) const 
    { return this->weight > rhs.weight; }
    
    // ============ Data =============
    //: The distribution
    dbsta_distribution<T> *distribution;
    //: The weight
    T weight;

    //: Return IO version number;
    short version() const{return 1;};
    //: Binary save self to stream.
    void b_write(vsl_b_ostream &os) const {
        vsl_b_write(os, version());
        vsl_b_write(os, distribution);
        vsl_b_write(os, weight);
        };

    //: Binary load self from stream.
    void b_read(vsl_b_istream &is){
        if (!is)
            return;
        short ver;
        vsl_b_read(is, ver);
        switch (ver)
            {
            case 1:
                vsl_b_read(is, distribution);
                vsl_b_read(is, weight);
                break;

            default:
                vcl_cerr << "component: unknown I/O version " << ver << '\n';
            }
        };

  };
  //: The component smart pointer is for internal use only
  typedef vbl_smart_ptr<component> component_sptr;




  
  //: This functor is used by default for sorting with STL
  // The default sorting is decreasing by weight
  class sort_weight
  {
   public:
    bool operator() (const component_sptr& c1, const component_sptr& c2) const
      { return c1->weight > c2->weight; }
  };
  
  //: This adaptor allows users to define ordering functors on the components
  //  without accessing the components directly
  template <class _comp_type>
  class sort_adaptor
  {
   public:
    sort_adaptor(_comp_type c) : comp(c) {}
    bool operator() (const component_sptr& c1, const component_sptr& c2) const
      { return comp(*c1->distribution, c1->weight, *c2->distribution, c2->weight); }
    _comp_type comp;
  };
  
  //: The vector of components
  vcl_vector<component_sptr> components_;  
  
 public:
  //: Constructor
  dbsta_mixture<T>(){}
  
  //: Copy Constructor
  dbsta_mixture<T>(const dbsta_mixture<T>& other);
  
  //: Clone
  // allocate a new copy of this object.
  // the caller is responsible for deletion 
  virtual dbsta_distribution<T>* clone() const
  { return new dbsta_mixture<T>(*this); }
  
  //: Return the number of dimensions in the space
  virtual unsigned int dim() const;
  
  //: Return the number of components in the mixture
  unsigned int num_components() const { return components_.size(); }
  
  //: Access (const) a component distribution of the mixture
  const dbsta_distribution<T>& distribution(unsigned int index) const 
  { return *components_[index]->distribution; }
  
  //: Access a component distribution of the mixture
  dbsta_distribution<T>& distribution(unsigned int index) 
  { return *components_[index]->distribution; }
 
  //: Return the weight of a component in the mixture
  T weight(unsigned int index) const { return components_[index]->weight; }
  
  //: Set the weight of a component in the mixture
  void set_weight(unsigned int index, const T& w) { components_[index]->weight = w; }
  
  //: Insert a new component at the end of the vector
  void insert(const dbsta_distribution<T>& d, const T& weight = T(0))
  { components_.push_back(new component(d, weight)); }
  
  //: Remove the last component in the vector
  void remove_last() { components_.pop_back(); }
    
  //: Compute the probablity of this point
  // \note assumes weights have been normalized
  virtual T probability(const vnl_vector<T>& pt) const;

  //: Normalize the weights of the components to add to 1.
  void normalize_weights();  
  
  //: Sort the components in order of decreasing weight
  void sort() { vcl_sort(components_.begin(), components_.end(), sort_weight() ); } 
  

   //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

   //: Return IO version number;
  short version() const;

  virtual vcl_string is_a() const{ 
      return vcl_string("dbsta_gaussian_indep"+vcl_string(typeid(T).name()));}
  //: Sort the components using any StrictWeakOrdering function
  // the prototype should be
  // \code
  // template <class T>
  // bool functor(const dbsta_distribution<T>& d1, const T& w1,
  //              const dbsta_distribution<T>& d2, const T& w2);
  // \endcode
  template <class _comp_type>
  void sort(_comp_type comp) 
  { vcl_sort(components_.begin(), components_.end(), sort_adaptor<_comp_type>(comp)); }

};


#endif // dbsta_mixture_h_
