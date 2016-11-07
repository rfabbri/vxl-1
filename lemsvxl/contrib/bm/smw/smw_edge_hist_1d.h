#ifndef SMW_EDGE_HIST_1D_H_
#define SMW_EDGE_HIST_1D_H_

#include<bsta/bsta_int_histogram_1d.h>

class edge_hist_1d
{
public:
    //default nbins artibtrary and large it really represents the number of
    //frames after which we delete a node and all edges pointing to it
    edge_hist_1d(unsigned const& nbins = 200):
        hist_(nbins){}

    //return the probability
    float prob();

private:
    bsta_int_histogram_1d hist_;
};

#endif //SMW_EDGE_HIST_1D_H_
