#include<dbrct/dbrct_wide_dense_match_params.h>
//: default constructor
dbrct_wide_dense_match_params::dbrct_wide_dense_match_params()
{
        levels=1;
        coarsestscale=1;

        paramslist.push_back(vsrl_parameters());

}
//: constructor using number of levels
dbrct_wide_dense_match_params::dbrct_wide_dense_match_params(int no_of_levels,int downsampling)
{
        levels=no_of_levels;
        for(int i=0;i<levels;i++)
        {
                paramslist.push_back(vsrl_parameters());
        }
        coarsestscale=downsampling;
}
dbrct_wide_dense_match_params::~dbrct_wide_dense_match_params()
{
}

