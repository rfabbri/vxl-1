#include "sliceEdgeDetectProcessor.h"
#include <det/det_edge_map.h>
#include <det/det_edge_nonmaximum_suppression.h>
#include <string>
#include <cstring>



std::vector<float*> sliceEdgeDetectProcessor::process(const std::vector< std::vector<float*> >& slice_sets, 
int w, int h, int z)
{

        std::vector<float*> gx_slices = slice_sets[0];
        std::vector<float*> gy_slices = slice_sets[1];
        std::vector<float*> gz_slices = slice_sets[2];

        float* gx = new float[w*h*this->nslices()];
        float* gy = new float[w*h*this->nslices()];
        float* gz = new float[w*h*this->nslices()];
        for(int i =0 ; i < this->nslices(); i++){
                std::memcpy(gx + i*w*h,gx_slices[i],w*h*sizeof(float));
                std::memcpy(gy + i*w*h,gy_slices[i],w*h*sizeof(float));
                std::memcpy(gz + i*w*h,gz_slices[i],w*h*sizeof(float));
        }


        det_edge_map em(w,h,this->nslices(),gx,gy,gz);
        em = em.nonmaxium_suppression_for_edge_detection();

        delete [] gx;
        delete [] gy;
        delete [] gz;


        std::vector<float*> toreturn;
        toreturn.push_back(new float[w*h]);
        toreturn.push_back(new float[w*h]);
        toreturn.push_back(new float[w*h]);
        toreturn.push_back(new float[w*h]);

        int cur_slice = this->nslices()/2;

        float* xoff= toreturn[0];
        float* yoff= toreturn[1];
        float* zoff= toreturn[2];
        float* s = toreturn[3];

        for(int j = 0; j <h; j++){
                for(int i = 0; i < w; i++){
                      *xoff= static_cast<float>(em[i][j][cur_slice].location_.x());
                      *yoff= static_cast<float>(em[i][j][cur_slice].location_.y());
                      *zoff= static_cast<float>(em[i][j][cur_slice].location_.z());
                      *s = static_cast<float>(em[i][j][cur_slice].strength_);
                       xoff++;
                       yoff++;
                       zoff++;
                       s++;
                }
        }

        return toreturn;
}

