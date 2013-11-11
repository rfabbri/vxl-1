#ifndef dbbgm_model_data_h
#define dbbgm_model_data_h

#include <vbl/vbl_ref_count.h>
#include <vcl_iostream.h>
#include <vbl/vbl_array_2d.h>
#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>

class dbbgm_model_data
{
public:
    dbbgm_model_data();
    ~dbbgm_model_data();


    void initialize(int numofmodels, int ni,int nj);
    void print_summary(vcl_ostream &os) const;


    //: Binary save self to stream.
    void b_write(vsl_b_ostream &os) const;
    //: Binary load self from stream.
    void b_read(vsl_b_istream &is);
    //: Return IO version number;
    short version() const;

    vcl_vector<vbl_array_2d<float> > standarddev;
    vcl_vector<vbl_array_2d<float> > weight;
    vcl_vector<vbl_array_2d<int> > mean;
    vcl_vector<vbl_array_2d<int> > samplecount;
 
    vbl_array_2d<int> usedmodels;
    vbl_array_2d<int> no_of_observations;
    vbl_array_2d<bool> isobserved;

    int nmodes;
};

#endif
