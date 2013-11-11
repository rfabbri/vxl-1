#include"smw_node_gauss_f1.h"

unsigned smw_node_gauss_f1::next_id_ = 0;

bool smw_node_gauss_f1::update(float const& obs)
{
    //when rho = 1/nobs_, we have a cumulative online average
    ++nobs_;
    rho_ = 1/nobs_;
    bsta_update_gaussian<float>(gauss_,rho_,obs);
    return true;
}


