#include "sliceProcessor.h"
#include <vcl_vector.h>

vcl_vector<float*> dummySliceProcessor::process(const vcl_vector< vcl_vector<float*> >& slice_sets, int w, int h,int slice)
{
        //dummyProcessor expects only 1 input set of slices
        vcl_vector<float*> slices = slice_sets[0];

        float* res = new float[w*h];
        vcl_vector<float*> toreturn;
        toreturn.push_back(res);

        //calculate the average (along Z) of the 3 slices
        for(int y = 0; y < h; y++){
                for(int x = 0; x < w; x++){
                        float sum = 0;
                        for(int slice = 0; slice < this->nslices(); slice++){
                                sum += *(slices[slice] + y*w + x);
                        }
                        sum /= this->nslices();
                        res[y*w+x] = sum;
                }
        }
        return toreturn;
}
