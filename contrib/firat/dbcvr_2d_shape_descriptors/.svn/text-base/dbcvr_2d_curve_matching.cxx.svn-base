// This is /lemsvxl/contrib/firat/dbcvr_2d_shape_descriptors/dbcvr_2d_curve_matching.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date May 23, 2011



#include "dbcvr_2d_curve_matching.h"
#include "dbcvr_2d_shape_descriptor_utils.h"
#include "dbcvr_2d_shape_descriptors.h"
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_algorithm.h>
#include <vsol/vsol_digital_curve_2d.h>

#define KAPPA_FUNCTION(i,m,L,M) (mymod((i+m-(L-1)/2) , M))

int mymod(int A, int b)
{
	if(A < 0)
	{
		return mymod(A+b, b);
	}
	return A % b;
}

dbcvr_curve_match::dbcvr_curve_match(int location, int num_sample_points, double score, bool reversed)
{
	m_ = location;
	L_ = num_sample_points;
	score_ = score;
	rev_ = reversed;
}

bool dbcvr_curve_match::operator<(dbcvr_curve_match rhs) const
{
	return score_ > rhs.score_;
}

void dbcvr_curve_match::print()
{
	vcl_cout << "rev:" << rev_ << " m: " << m_ << " L: " << L_ << " S: " << score_ << vcl_endl;
}


double dbcvr_score_diff_chord_angles(int m, vnl_matrix<double>& alpha_prime, vnl_matrix<double>& beta_prime, vcl_vector<unsigned>& indices)
{
	int M = alpha_prime.columns();
	int L = beta_prime.columns();
	int qrows = indices.size();
	double diff = 0;
	for(int i = 0; i < qrows; i++)
	{
		int k1_P = KAPPA_FUNCTION(indices[i], m, L, M);
		for(int j = 0; j < L-2; j++)
		{
			int k2_P = KAPPA_FUNCTION(j, m, L-2, M);
			diff += vcl_acos(vcl_cos(alpha_prime(k1_P, k2_P) - beta_prime(i, j+1)));
		}
	}
	return 1 - diff/vnl_math::pi/(L-2)/qrows;
}

bool dbcvr_get_best_matches_single_scale(vnl_matrix<double>& alpha_prime, vsol_digital_curve_2d_sptr query, vcl_vector<unsigned>& indices, vcl_vector<dbcvr_curve_match>& matches, bool reversed, int num_detections, int center_step)
{
	int M = alpha_prime.columns();
	int L = query->size();
	vnl_matrix<double> beta_prime;
	dbcvr_diff_chord_angles(query, false, indices, beta_prime);
	for(int i = 0; i < M; i+=center_step)
	{
		matches.push_back(dbcvr_curve_match(i, L, dbcvr_score_diff_chord_angles(i, alpha_prime, beta_prime, indices), reversed));
	}
	vcl_sort(matches.begin(), matches.end());
	if(matches.size() > num_detections)
	{
		matches.erase(matches.begin()+num_detections, matches.end());
	}
	return true;
}

bool dbcvr_get_best_matches_single_scale(vsol_digital_curve_2d_sptr exemplar, vsol_digital_curve_2d_sptr query, vcl_vector<unsigned>& indices, vcl_vector<dbcvr_curve_match>& matches, bool reversed, int num_detections, int center_step)
{
	vcl_vector<unsigned> exemplar_indices;
	vnl_matrix<double> alpha_prime;
	dbcvr_diff_chord_angles(exemplar, true, exemplar_indices, alpha_prime);
	return dbcvr_get_best_matches_single_scale(alpha_prime, query, indices, matches, reversed, num_detections, center_step);
}

bool dbcvr_get_best_matches_multi_scale(vsol_digital_curve_2d_sptr exemplar, vsol_digital_curve_2d_sptr query, vcl_vector<unsigned>& indices,  vcl_vector<dbcvr_curve_match>& matches, vcl_vector<int>& sample_points, int num_detections, int center_step)
{
	vcl_vector<unsigned> exemplar_indices;
	vnl_matrix<double> alpha_prime;
	dbcvr_diff_chord_angles(exemplar, true, exemplar_indices, alpha_prime);
	for(int i = 0; i < sample_points.size(); i++)
	{
		vsol_digital_curve_2d_sptr qL = dbcvr_uniform_sampling_open(query, sample_points[i]);
		vsol_digital_curve_2d_sptr qL_rev = dbcvr_get_reversed_curve(qL);
		vcl_vector<dbcvr_curve_match> matchesL, matchesL_rev;
		dbcvr_get_best_matches_single_scale(alpha_prime, qL, indices, matchesL, false, num_detections, center_step);
		dbcvr_get_best_matches_single_scale(alpha_prime, qL_rev, indices, matchesL_rev, true, num_detections, center_step);
		matches.insert(matches.end(), matchesL.begin(), matchesL.end());
		matches.insert(matches.end(), matchesL_rev.begin(), matchesL_rev.end());
	}
	vcl_sort(matches.begin(), matches.end());
	if(matches.size() > num_detections)
	{
		matches.erase(matches.begin()+num_detections, matches.end());
	}
	return true;
}
