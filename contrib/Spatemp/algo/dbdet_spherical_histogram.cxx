#include "dbdet_spherical_histogram.h"
#include <vnl/vnl_math.h>
dbdet_spherical_histogram::dbdet_spherical_histogram()
{

    theta_min_=-vnl_math::pi_over_2;
    theta_max_=vnl_math::pi_over_2;

    phi_min_=-vnl_math::pi;
    phi_max_=vnl_math::pi;

    theta_partitions_=8;

    int factor=3;
    phi_partitions_[0]=1*factor;
    phi_partitions_[1]=3*factor;
    phi_partitions_[2]=4*factor;
    phi_partitions_[3]=5*factor;
    phi_partitions_[4]=5*factor;
    phi_partitions_[5]=4*factor;
    phi_partitions_[6]=3*factor;
    phi_partitions_[7]=1*factor;

    for(unsigned i=0;i<theta_partitions_;i++)
    {
        hist_[i]=vcl_vector<float>(phi_partitions_[i],0.0);
    }
}


bool
dbdet_spherical_histogram::update(float theta,float phi)
{
    int theta_index=vcl_floor(theta_partitions_*(theta-theta_min_)/(vnl_math::pi));
    int phi_index=vcl_floor(phi_partitions_[theta_index]*(phi-phi_min_)/(2*vnl_math::pi));
    if(hist_[theta_index][phi_index]==0.0)
        hist_[theta_index][phi_index]=1;

    return true;
}

void dbdet_spherical_histogram::print()
{
    for(unsigned i=0;i<theta_partitions_;i++)
    {
        for(unsigned j=0;j<hist_[i].size();j++)
        {
            vcl_cout<<hist_[i][j]<<" ";
        }
        vcl_cout<<"\n";
    }

}
float 
dbdet_spherical_histogram::get_theta(int i)
{
    if(i<theta_partitions_)
        return theta_min_+i*(theta_max_-theta_min_)/theta_partitions_;
    else return 0;
}
float 
dbdet_spherical_histogram::get_phi(int i,int j)
{
    if(j<phi_partitions_[i])
        return phi_min_+j*(phi_max_-phi_min_)/phi_partitions_[i];
    else 
        return 0;
}
dbdet_spherical_histogram &
dbdet_spherical_histogram::operator+=(dbdet_spherical_histogram & c)
{
    for(unsigned i=0;i<theta_partitions_;i++)
    {
        for(unsigned j=0;j<hist_[i].size();j++)
        {
            this->hist_[i][j]+=c.hist_[i][j];
        }
    }


    return (*this);
}
