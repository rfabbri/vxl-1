#include <dbskfg/algo/dbskfg_app_curve_match.h>
#include <dbskr/dbskr_scurve.h>

dbskfg_app_curve_match::dbskfg_app_curve_match(
    vnl_matrix<vl_sift_pix> sift_c1,
    vnl_matrix<vl_sift_pix> sift_c2)
    : dbcvr_cvmatch_even(),
      sift_c1_(sift_c1),
      sift_c2_(sift_c2)
{
    _n1=sift_c1_.cols();
    _n2=sift_c2_.cols();
    setTemplateSize(5);

}


dbskfg_app_curve_match::~dbskfg_app_curve_match() 
{ 
    sift_c1_.clear();
    sift_c2_.clear();
    clear();
}

void dbskfg_app_curve_match::initializeDPCosts()
{
    _DPCost.clear();
    _DPMap.clear();
  
    assert (_n1>0);
    assert (_n2>0);

    for (int i=0;i<_n1;i++) 
    {
        vcl_vector<double> tmp1(_n2,DP_VERY_LARGE_COST);
        _DPCost.push_back(tmp1);
        vcl_pair <int,int> tmp3(0,0);
        vcl_vector< vcl_pair <int,int> > tmp2(_n2,tmp3);
        _DPMap.push_back(tmp2);
    }

    //Cost Matrix Initialization
    _finalCost = DP_VERY_LARGE_COST;
    for (int n=0;n<_n1;n++) 
    {
        for (int m=0;m<_n2;m++) 
        {
            _DPCost[n][m]=DP_VERY_LARGE_COST;
        }
    }
    _DPCost[0][0]=0.0;

}

// Cost of matching the  interval [x(i),x(ip)] to the interval [y(j),y(jp)].
double dbskfg_app_curve_match::computeIntervalCost(int i, int ip, int j, int jp) 
{

    double dA1_cont(0.0),dA2_cont(0.0);

    vnl_vector<vl_sift_pix>  sift_i  = sift_c1_.get_column(i);
    vnl_vector<vl_sift_pix>  sift_ip = sift_c1_.get_column(ip);
    double dapp1=descr_cost(sift_i,sift_ip);

    vnl_vector<vl_sift_pix>  sift_j = sift_c2_.get_column(j);
    vnl_vector<vl_sift_pix>  sift_jp= sift_c2_.get_column(jp);
    double dapp2=descr_cost(sift_j,sift_jp);
    

    double app_cont=vcl_fabs(dapp1-dapp2);

    double app_diff=vcl_fabs(descr_cost(sift_i,sift_j)+
                             descr_cost(sift_ip,sift_jp));

    double cost=app_cont+app_diff;

    return cost;
} 




