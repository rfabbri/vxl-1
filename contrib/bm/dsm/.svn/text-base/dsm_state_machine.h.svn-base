//this is /contrib/bm/dsm/dsm_state_machine.h
#ifndef DSM_STATE_MACHINE_H_
#define DSM_STATE_MACHINE_H_
//:
// \file
// \date August 9, 2010
// \author Brandon A. Mayer
//
// This is the graph class which will hold dsm_nodes.
//
// \verbatim
//  Modifications
// \endverbatim

#include"dsm_node_base_sptr.h"
#include"dsm_node.h"
#include"dsm_state_machine_base_sptr.h"
#include"dsm_map_utilities.h"

#include<vbl/vbl_ref_count.h>

#include<vnl/vnl_diag_matrix.h>
#include<vnl/vnl_matrix.h>
#include<vnl/vnl_matrix_fixed.h>
#include<vnl/vnl_vector.h>
#include<vnl/vnl_vector_fixed.h>
#include<vnl/io/vnl_io_diag_matrix.h>
#include<vnl/io/vnl_io_matrix.h>
#include<vnl/io/vnl_io_vector.h>

template<int T>
class dsm_state_machine: public dsm_state_machine_base
{
public:
	//dsm_state_machine():dsm_state_machine_base(){}

	dsm_state_machine( unsigned classifier_id = 0,
                           unsigned const& t_forget = 30,
			   double const& prob_thresh = 0.0002,
			   double const& mahalan_dist_factor = 1,
			   vnl_matrix<double> const& init_covar 
                               = vnl_diag_matrix<double>(2,1.0),
			   vnl_matrix<double> const& min_covar 
                               = vnl_matrix<double>(2,2,0.0001) ):
                           dsm_state_machine_base(t_forget, 
                                   prob_thresh, 
                                   mahalan_dist_factor, 
                                   init_covar, min_covar), 
                           init_covar_(init_covar), min_covar_(min_covar)
         {this->classifier_id_=classifier_id;}

	~dsm_state_machine(){}

	//processes
	virtual bool classify( vnl_vector<double> const& obs );

	virtual bool update_manhalanobis_distance( 
            vnl_vector<double> const& obs );

	virtual bool classify( dsm_feature_sptr feature_sptr );

	//mutators
    void set_init_covar( vnl_matrix_fixed<double,T,T> const& init_covar)
    { this->init_covar_ = init_covar; }

    void set_init_covar( vnl_matrix<double> const& init_covar)
    { this->init_covar_ = init_covar; }

	//accessors
	virtual int ndims(){return T;}


	//i/o
	virtual void b_write(vsl_b_ostream& os) const; 

	virtual void b_read(vsl_b_istream& is);

	virtual void write_dot_file( vcl_string const& filename );

	virtual void write_dot_file_full( vcl_string const& filename );

protected:
	
	virtual vcl_map<unsigned, dsm_node_base_sptr>::iterator add_node();

	virtual bool remove_node( unsigned const& node_id );

	virtual void prune_graph();

	vnl_matrix_fixed<double,T,T> min_covar_;

	vnl_matrix_fixed<double,T,T> init_covar_;
	
};

#endif //DSM_STATE_MACHINE_H_
