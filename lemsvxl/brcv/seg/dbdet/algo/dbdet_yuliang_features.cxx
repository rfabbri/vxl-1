#include "dbdet_yuliang_features.h"

double constexpr dbdet_yuliang_const::diag_of_train; // ???
unsigned constexpr dbdet_yuliang_const::nbr_num_edges;  // # of edges close to connecting points
unsigned constexpr dbdet_yuliang_const::nbr_len_th; // short curve under this length will be grouped due to geometry.
double constexpr dbdet_yuliang_const::merge_th_sem;
double constexpr dbdet_yuliang_const::merge_th_geom;
double constexpr dbdet_yuliang_const::epsilon;
