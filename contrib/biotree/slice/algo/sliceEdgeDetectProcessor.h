#if !defined(SLICE_EDGEDETECT_PROCESSOR_H_)
#define SLICE_EDGEDETECT_PROCESSOR_H_ 

#include <vcl_vector.h>
#include <vcl_string.h>
#include <slice/sliceProcessor.h>
#include <vil3d/vil3d_image_view.h>
#include <vil/vil_image_view.h>

class sliceEdgeDetectProcessor  : public sliceProcessor<float>
{
  public:
    sliceEdgeDetectProcessor() {};
    virtual vcl_vector<float*> process(const vcl_vector< vcl_vector<float*> >& slice_sets, 
        int w, int h, int slice);
    virtual const int nstreams() const { return 3;}
    virtual const int nslices() const{ return 5;}
    virtual const int noutputs() const{ return 4;}
    virtual const vcl_string name() const{ return vcl_string("sliceEdgeDetectProcessor");}
  protected:
  private:
};

#endif 
