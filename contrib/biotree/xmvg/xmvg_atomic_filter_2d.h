// This is /contrib/biotree/xmvg/xmvg_atomic_filter_2d.h

#ifndef xmvg_atomic_filter_2d_h_
#define xmvg_atomic_filter_2d_h_

//: 
// \file   xmvg_atomic_filter_2d.h
// \brief  The base class of 2D filter which provide the interface
// \author Kongbin Kang and H. Can Aras
// \date   2005-03-17
// 
// Note that vbl_array_2d is being used with a transposed index. Normally
// vbl_array_2d has the index, array[row][column], i.e. a mathematical array.
// In this class the array is considered to be transposed so that the 
// index is array[column][row]. The consequence is that size[0] is the 
// number of image columns and size[1] is the number of image rows. This useage
// is not to be confused with vbl_array_2d::[unsigned i] which returns the 
// i+1th array row.  JLM May 29,2006
#include <vcl_vector.h>
#include <vnl/vnl_int_2.h>
#include <vbl/vbl_array_2d.h>
#include <vil/algo/vil_convolve_1d.h>
#include "xmvg_atomic_filter_2d_base.h"

template< class T>
class xmvg_atomic_filter_2d : public xmvg_atomic_filter_2d_base<T>
{
  public:
   
    //: default constructor  
    xmvg_atomic_filter_2d() : s_(0,0), loc_(0,0) {}

    //: full constructor
    xmvg_atomic_filter_2d(const vnl_int_2 &c, const vnl_int_2 &s, const vbl_array_2d<T> w) : 
      s_(s), loc_(c), weights_(w) { compute_weights_sum(w); }

    //: location 
    vnl_int_2 location() const { return loc_ ;}
    void set_location(vnl_int_2 const & loc) { loc_ = loc; }

    //: size
    vnl_int_2 size() const { return s_; }

    void set_size(vnl_int_2 const & size) { s_ = size;}

    //: weights
    vbl_array_2d<T> weights() const {return weights_; }

    void set_weights(vbl_array_2d<T> const w) { weights_ = w; compute_weights_sum(w); } 

    //; sum of weights
    T weights_sum() const { return weights_sum_; }

    //: destructor
    virtual ~xmvg_atomic_filter_2d() {}

    //: access data
    T const* operator[](unsigned i) const { return weights_[i]; }
    T      * operator[](unsigned i) { return weights_[i]; }

    //: test for equality (used in testing splat_collection)
    bool operator==(const xmvg_atomic_filter_2d<T> & other) const {
       return (loc_ == other.loc_) && (weights_ == other.weights_);
    }

    //: test for inequality (included for completeness)
    bool operator!=(const xmvg_atomic_filter_2d<T> & other) const {
        return !(this == other);
    }

    //: make residue zero
    void eliminate_residue_effect(){
      int size_x = s_(0);   int size_y = s_(1);
      T residue_per_pixel = weights_sum_ / (size_x * size_y);
      for(int i=0;i<size_x;i++)
        for(int j=0;j<size_y;j++)
          weights_[i][j] -= residue_per_pixel;
    }

    //: convolve each row of the filter by the given kernel
    void convolve_rows_with_kernel(vcl_vector<T> kernel)
    {
      int kernel_center = (kernel.size() - 1) / 2;
      int size_x = s_(0);
      int size_y = s_(1);
      vcl_vector<T> row(size_x);
      vcl_vector<T> convolved(size_x, 0.0);
      for(int j=0; j<size_y; j++)
      {
        row.clear();
        // get the row
        for(int i=0; i<size_x; i++)
          row.push_back(weights_[i][j]);

        //row.clear();          convolved.clear();
        //row.push_back(2.0);   convolved.push_back(0.0);
        //row.push_back(1.0);   convolved.push_back(0.0);
        //row.push_back(4.0);   convolved.push_back(0.0);
        //row.push_back(3.0);   convolved.push_back(0.0);
        //row.push_back(6.0);   convolved.push_back(0.0);
        //row.push_back(5.0);   convolved.push_back(0.0);
        //row.push_back(2.0);   convolved.push_back(0.0);
        /*for(int index=0; index<row.size(); index++)
          vcl_cout << row[index] << " ";
        vcl_cout << vcl_endl;*/
        // convolve it with the kernel
        vil_convolve_1d<T,T,T,T>(&row[0], row.size(), 1, &convolved[0], 1,
                        &kernel[kernel_center], -kernel_center, kernel_center,
                        T(), 
                        vil_convolve_zero_extend, vil_convolve_zero_extend);
        // put the convolved row back
       /* for(int index=0; index<convolved.size(); index++)
          vcl_cout << convolved[index] << " ";
        vcl_cout << vcl_endl;*/

        for(int i=0; i<size_x; i++)
          weights_[i][j] = convolved[i];
      }
    }

  private:
    
     //: size of the filter
    vnl_int_2 s_;

    //: location for applying the data
    vnl_int_2 loc_;
     
    //: a vector of splates
    vbl_array_2d<T> weights_;

    //: sum of weights, is not affected by eliminate_residue_effect function
    T weights_sum_;

    void compute_weights_sum(vbl_array_2d<T> const w) 
    {
      int size_x = s_(0);   int size_y = s_(1);
      weights_sum_= 0;
      for(int i=0;i<size_x;i++)
        for(int j=0;j<size_y;j++)
          weights_sum_ += weights_[i][j];
    }
};


#endif

