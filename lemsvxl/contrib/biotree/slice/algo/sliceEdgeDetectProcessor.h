#if !defined(SLICE_EDGEDETECT_PROCESSOR_H_)
#define SLICE_EDGEDETECT_PROCESSOR_H_ 

#include <vector>
#include <string>
#include <slice/sliceProcessor.h>
#include <vil3d/vil3d_image_view.h>
#include <vil/vil_image_view.h>

class sliceEdgeDetectProcessor  : public sliceProcessor<float>
{
  public:
    sliceEdgeDetectProcessor() {};
    virtual std::vector<float*> process(const std::vector< std::vector<float*> >& slice_sets, 
        int w, int h, int slice);
    virtual const int nstreams() const { return 3;}
    virtual const int nslices() const{ return 5;}
    virtual const int noutputs() const{ return 4;}
    virtual const std::string name() const{ return std::string("sliceEdgeDetectProcessor");}
  protected:
  private:
};

#endif 
