//this is /contrib/bm/dsm2/dsm2_node_gaussian_full.txx
#ifndef DSM2_NODE_GAUSSIAN_TXX_
#define DSM2_NODE_GAUSSIAN_TXX_

#include<dsm2/dsm2_node_gaussian.h>

#include<vnl/io/vnl_io_diag_matrix.h>
#include<vnl/io/vnl_io_matrix_fixed.h>
#include<vnl/io/vnl_io_vector_fixed.h>

template<class gaussianT>
void dsm2_node_gaussian<gaussianT>::b_write( vsl_b_ostream& os ) const
{
	const short version_no = 1;
	vsl_b_write(os, version_no);
	vsl_b_write(os,this->mean());
	vsl_b_write(os,this->covar());
}//end b_write

template<class gaussianT>
void dsm2_node_gaussian<gaussianT>::b_read( vsl_b_istream& is )
{
	if(!is) return;

	short v;
	vsl_b_read(is,v);
	
	switch(v)
	{
	case 1:
		{
			//read the mean
			vector_type mean;
			vsl_b_read(is,mean);
			this->set_mean(mean);
			
			//read the covariance
			covar_type covar;
			vsl_b_read(is, covar);
			this->set_covar(covar);
			break;
		}//end case 1
	default:
		{
			vcl_cerr << "ERROR: dsm2_node_gaussian_full::b_read() -- unknown version number." << vcl_flush;
			return;
		}//end default
	}//end switch(v)
	return;
}//end b_read

#define DSM2_NODE_GAUSSIAN_INSTANTIATE(...)\
template class dsm2_node_gaussian<__VA_ARGS__>

#endif //DSM2_NODE_GAUSSIAN_FULL_TXX_