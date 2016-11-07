#if !defined(SLICE_PROCESSOR_H_)
#define SLICE_PROCESSOR_H_ 

#include <vcl_vector.h>
#include <vcl_string.h>

template <class T> class sliceProcessor{
  public:
    virtual vcl_vector<T*> process(const vcl_vector< vcl_vector<T*> >& slice_sets, int w, int h, int slice) = 0;
    virtual const int nstreams() const = 0;
    virtual const int nslices() const = 0;
    virtual const int noutputs() const = 0;
    virtual const vcl_string name() const = 0;
  protected:
  private:
};

class dummySliceProcessor : public sliceProcessor<float>
{
  public:
    virtual vcl_vector<float*> process(const vcl_vector< vcl_vector<float*> >& slice_sets, int w, int h, int slice);
    virtual const int nstreams() const { return 1;}
    virtual const int nslices() const{ return 3;}
    virtual const int noutputs() const{ return 1;}
    virtual const vcl_string name() const{ return vcl_string("dummySliceProcessor");}
  protected:
  private:
};

#endif 
