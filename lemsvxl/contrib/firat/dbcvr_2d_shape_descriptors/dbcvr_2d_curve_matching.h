// This is /lemsvxl/contrib/firat/dbcvr_2d_shape_descriptors/dbcvr_2d_curve_matching.h.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date May 23, 2011

#ifndef DBCVR_2D_CURVE_MATCHING_H_
#define DBCVR_2D_CURVE_MATCHING_H_

#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>


class dbcvr_curve_match
{
public:
	dbcvr_curve_match(int location, int num_sample_points, double score, bool reversed);
	bool operator<(dbcvr_curve_match rhs) const;
	void print();
private:
	double score_;
	double m_;
	double L_;
	bool rev_;
};


double dbcvr_score_diff_chord_angles(int m, vnl_matrix<double>& alpha_prime, vnl_matrix<double>& beta_prime, vcl_vector<unsigned>& indices);
bool dbcvr_get_best_matches_single_scale(vsol_digital_curve_2d_sptr exemplar, vsol_digital_curve_2d_sptr query, vcl_vector<unsigned>& indices,  vcl_vector<dbcvr_curve_match>& matches, bool reversed = false, int num_detections = 30, int center_step = 2);
bool dbcvr_get_best_matches_single_scale(vnl_matrix<double>& alpha_prime, vsol_digital_curve_2d_sptr query, vcl_vector<unsigned>& indices,  vcl_vector<dbcvr_curve_match>& matches, bool reversed = false, int num_detections = 30, int center_step = 2);
bool dbcvr_get_best_matches_multi_scale(vsol_digital_curve_2d_sptr exemplar, vsol_digital_curve_2d_sptr query, vcl_vector<unsigned>& indices,  vcl_vector<dbcvr_curve_match>& matches, vcl_vector<int>& sample_points, int num_detections = 30, int center_step = 2);

#endif /* DBCVR_2D_CURVE_MATCHING_H_ */
