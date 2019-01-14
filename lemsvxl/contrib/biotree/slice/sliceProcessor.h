#if !defined(SLICE_PROCESSOR_H_)
#define SLICE_PROCESSOR_H_ 

#include <vector>
#include <string>

template <class T> class sliceProcessor{
  public:
    virtual std::vector<T*> process(const std::vector< std::vector<T*> >& slice_sets, int w, int h, int slice) = 0;
    virtual const int nstreams() const = 0;
    virtual const int nslices() const = 0;
    virtual const int noutputs() const = 0;
    virtual const std::string name() const = 0;
  protected:
  private:
};

class dummySliceProcessor : public sliceProcessor<float>
{
  public:
    virtual std::vector<float*> process(const std::vector< std::vector<float*> >& slice_sets, int w, int h, int slice);
    virtual const int nstreams() const { return 1;}
    virtual const int nslices() const{ return 3;}
    virtual const int noutputs() const{ return 1;}
    virtual const std::string name() const{ return std::string("dummySliceProcessor");}
  protected:
  private:
};

#endif 
