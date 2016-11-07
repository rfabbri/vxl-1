// This is brcv/seg/dbsta/dbsta_histogram.h
#ifndef dbsta_histogram_h_
#define dbsta_histogram_h_

//:
// \file
// \brief A histogram as a probability distribution 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 08/17/05
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsta_discrete.h"
#include "dbsta_updater.h"


//: A histogram as a probability distribution 
// A discrete distribution that counts samples
// \note every bin is represented in memory, even if it is empty
template <class T>
class dbsta_histogram : public dbsta_discrete_base<T>
{
 public:
  //: Constructor - each dimension has the same range and number of bins
  dbsta_histogram<T>(unsigned int dim, T min, T max, 
                     unsigned int bins);
  
  //: Constructor - each dimension has different parameters
  dbsta_histogram<T>(unsigned int dim, const vcl_vector<T>& min, 
                     const vcl_vector<T>& max, 
                     const vcl_vector<unsigned int>& bins);
  
  //: Destructor
  virtual ~dbsta_histogram<T>(){}
   
  //: Clone
  // allocate a new copy of this object.
  // the caller is responsible for deletion 
  virtual dbsta_distribution<T>* clone() const
  { return new dbsta_histogram<T>(*this); }
  
  //: Update the count in the bin that this point fall into
  // \returns false if the point is out of bounds
  virtual bool update(const vnl_vector<T>& pt);

   //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

   //: Return IO version number;
  short version() const;

    virtual vcl_string is_a() const{return "dbsta_histogram";}

  
 protected:
  //: return the probability for a given index
  virtual T prob_idx(unsigned int index) const
  {
    assert(index >=0 && index < counts_.size());
    if(total_ == 0) 
      return T(0);
    return static_cast<T>(counts_[index])/total_; 
  }
   
 private:
  vcl_vector<unsigned int> counts_;
  unsigned int total_;
};


//: An updater for histograms
template <class T>
class dbsta_histogram_updater : public dbsta_data_updater<T>
{
 public:
  
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample ) const
  {
    assert(d.dim() == sample.size());
    assert(dynamic_cast<dbsta_histogram<T>* >(&d));
    dbsta_histogram<T>& h = static_cast<dbsta_histogram<T>& >(d);
    h.update(sample);
  }

};



#endif // dbsta_histogram_h_
