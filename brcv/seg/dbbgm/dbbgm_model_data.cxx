

#include <dbbgm/dbbgm_model_data.h>
#include <vsl/vsl_vector_io.h>
#include <vbl/io/vbl_io_array_2d.h>

dbbgm_model_data::dbbgm_model_data()
{
}

dbbgm_model_data::~dbbgm_model_data()
{

}

void dbbgm_model_data::initialize(int no_of_modes,int ni,int nj)
{

    nmodes=   no_of_modes;
    for(int i=0; i<no_of_modes; i++)
    {
        mean.push_back(vbl_array_2d<int>(nj,ni,0));
        standarddev.push_back(vbl_array_2d<float>(nj,ni,0));
        weight.push_back(vbl_array_2d<float>(nj,ni,0.0));
        samplecount.push_back(vbl_array_2d<int>(nj,ni,0));
    };

    no_of_observations.resize(nj,ni);
    usedmodels.resize(nj,ni);
    isobserved.resize(nj,ni);
    no_of_observations.fill(0);
    usedmodels.fill(0);
    isobserved.fill(false);
}
void dbbgm_model_data::print_summary(vcl_ostream &os) const
{

}

void dbbgm_model_data::b_write(vsl_b_ostream &os) const
{
    vsl_b_write(os, version());
    vsl_b_write(os, this->nmodes);
    for(int i=0;i<nmodes;i++)
    {
        vsl_b_write(os, this->mean[i]);
        vsl_b_write(os, this->standarddev[i]);
        vsl_b_write(os, this->weight[i]);
        vsl_b_write(os, this->samplecount[i]);
    }
    vsl_b_write(os, this->usedmodels);
    vsl_b_write(os, this->no_of_observations);
    vsl_b_write(os, this->isobserved);


}

short dbbgm_model_data::version() const
{
    return 1;
}


void dbbgm_model_data::b_read(vsl_b_istream &is) 
{
    if (!is) return;
    vbl_array_2d<int> temp1;
    vbl_array_2d<float> temp2;
    short ver;
    vsl_b_read(is, ver);
    switch(ver)
    {
    case 1:

        vsl_b_read(is, this->nmodes);
        for(int i=0;i<nmodes;i++)
        {
            vsl_b_read(is, temp1);
            mean.push_back(temp1);
            vsl_b_read(is, temp2);
            standarddev.push_back(temp2);
            vsl_b_read(is, temp2);
            weight.push_back(temp2);
            vsl_b_read(is, temp1);
            samplecount.push_back(temp1);
        }
        vsl_b_read(is, this->usedmodels);
        vsl_b_read(is, this->no_of_observations);
        vsl_b_read(is, this->isobserved);



        break;
    default:
        vcl_cerr << "I/O ERROR: dbctrk_tracker_curve::b_read(vsl_b_istream&)\n"
            << "           Unknown version number "<< ver << '\n';
        is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
        return;
    }


}
