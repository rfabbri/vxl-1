//this is /contrib/bm/dbcl/dbcl_state_machine_image_classifier.h
#ifndef DBCL_STATE_MACHINE_IMAGE_CLASSIFIER_H_
#define DBCL_STATE_MACHINE_IMAGE_CLASSIFIER_H_
//:
// \file
// \date September 3, 2010
// \author Brandon A. Mayer
//
// Class to classify with temporal state machine.
//
// \verbatim
//  Modifications
// \endverbatim
#include"dbcl_classifier_factory.h"
#include"dbcl_state_machine_classifier_sptr.h"


#include"dncn_target_list_2d_sptr.h"

#include"vcl_cmath.h"



class dbcl_state_machine_image_classifier
{
public:

	typedef vcl_map<unsigned, dbcl_state_machine_classifier_sptr > classifier_map_type;

	dbcl_state_machine_image_classifier( dncn_target_list_2d_sptr target_list_sptr,
										 unsigned const& t_forget = 30,
										 double const& prob_thresh = 0.0002,
										 double const& mahalan_dist_factor = 1,
										 vnl_matrix_fixed<double,2,2> const& init_covar = vnl_diag_matrix_fixed<double,2>(1.0).as_matrix_fixed(), 
                                         vnl_matrix_fixed<double,2,2> const& min_covar = vnl_matrix_fixed<double,2,2>(0.0001));

	~dbcl_state_machine_image_classifier(){}

	void classify();

	unsigned nrows(){ return this->nrows_; }

	unsigned ncols(){ return this->ncols_; }

	unsigned nframes(){ return this->nframes_; }

	classifier_map_type classifier_map(){ return this->classifier_map_; }

	dbcl_state_machine_classifier_sptr classifier_sptr( unsigned const& linear_indx ){ return this->classifier_map_[linear_indx]; }

	dbcl_state_machine_classifier_sptr classifier_sptr( vgl_point_2d<unsigned> pt ){ return this->classifier_sptr(pt.x(),pt.y()); }

	dbcl_state_machine_classifier_sptr classifier_sptr( unsigned const& x, unsigned const& y );

	void add_classifier( vgl_point_2d<unsigned> target, dbcl_state_machine_classifier_sptr classifier_sptr );

	unsigned t_forget() { return this->t_forget_; }

	double prob_thresh(){ return this->prob_thresh_; }

	vnl_matrix_fixed<double,2,2> init_covar(){ return this->init_covar_; }

	vnl_matrix_fixed<double,2,2> min_covar(){ return this->min_covar_; }


	//functions to convert from cartesian to linear and visa versa indexing coordinates
	void linear_to_cartesian( unsigned const& linear_indx, unsigned& x, unsigned& y );
	unsigned cartesian_to_linear(unsigned const& x, unsigned const& y);
	unsigned cartesian_to_linear(vgl_point_2d<unsigned> const& point);
	vgl_point_2d<unsigned> linear_to_cartesian( unsigned const& linear_indx );

private:


	unsigned nrows_;
	unsigned ncols_;
	unsigned nframes_;
	unsigned ntargets_;

	unsigned t_forget_;
	double prob_thresh_;
	double mahalan_dist_factor_;
	vnl_matrix_fixed<double,2,2> init_covar_;
	vnl_matrix_fixed<double,2,2> min_covar_;

	classifier_map_type classifier_map_;
	

};

#endif //DBCL_STATE_MACHINE_IMAGE_CLASSIFIER_H_