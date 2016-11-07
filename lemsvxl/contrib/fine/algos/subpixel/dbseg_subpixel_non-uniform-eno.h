#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NON_UNIFORM_ENO_H_
#define _NON_UNIFORM_ENO_H_


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dbseg_subpixel_utils.h"
#include "dbseg_subpixel_graphics.h"
/*#include <SPCedtGeno.h>*/
#include "dbseg_subpixel_1d_eno_types.h"



#define LEFT_INT 0
#define MID_INT 1
#define RIGHT_INT 2

#define NU_ENOMaxOrder 2
#define NU_ENOMaxZerox 2


/*typedef struct{
  int n;
  float *fx;
  float *x;
  float *first_NDD;
  float *second_NDD;
}NDD_Info;*/

typedef struct{
  double coeff[NU_ENOMaxOrder+1];
  int order;
}NU_ENO_Polynomial;

typedef struct{
  TwoD_DPoint start,end;
  int forward;
  NU_ENO_Polynomial poly;
  double start_tangent, end_tangent;
  double start_curvature, end_curvature;
  double total_curvature;
}NU_ENO_Interpolant;


typedef struct{
  double loc[NU_ENOMaxZerox+1];
  double slope[NU_ENOMaxZerox+1];
}NU_ENO_Zerox;


typedef struct{
  NU_ENO_Interpolant forward,backward;
  double shock_loc;
}NU_ENO_Shock_Interpolant;


typedef struct{
  TwoD_DPoint pt[4];
  NU_ENO_Interpolant interp;
  int need_shock_flag;
  int shock_cnt;
  NU_ENO_Shock_Interpolant shock;
  int zerox_cnt;
  NU_ENO_Zerox zerox;
}NU_ENO_Interval;

/******************************************************************/
/************************** nonunif_eno.c *************************/
/******************************************************************/


void fill_eno_interval_info(NU_ENO_Interval *ei, double *x, double *y);


void fill_eno_interval_info_general(NU_ENO_Interval *ei, double *x, 
                                    double *y, int N);

void find_nu_eno_interp_coeff(NU_ENO_Interval *ei, int N);

void  print_all_nueno_second_coeffs(NU_ENO_Interval *ei, int N);

void  add_nu_eno_shock_interp_coeff(NU_ENO_Interval *ei, int N);

void nu_enointv_copy(NU_ENO_Interval *source, NU_ENO_Interval *dest);


/******************************************************************/
/**********************nonunif_eno_interpolant.c*******************/
/******************************************************************/

void compute_second_eno_coefficients(NU_ENO_Interval *ei);

void compute_second_eno_coeffs_one_sided(NU_ENO_Interval *ei, int forward);

void compute_second_eno_coefficients_all(NU_ENO_Interval *ei);

void print_second_eno_coefficients(NU_ENO_Interval *ei);


void print_just_second_eno_coefficients(NU_ENO_Interval *ei);

void compute_second_eno_geometric_values(NU_ENO_Interpolant *interp);

void nu_enointerp_copy(NU_ENO_Interpolant *source, NU_ENO_Interpolant *dest
);


double eno_1derivative(ENO_Interval_t *ei, double loc);

double eno_2derivative(ENO_Interval_t *ei, double loc);

double nu_eno_interpolation(NU_ENO_Interval *ei, double loc);

double nu_eno_interp_1derivative(NU_ENO_Interval *ei, double loc);

double nu_eno_interp_2derivative(NU_ENO_Interval *ei, double loc);

void compute_second_eno_coeffs_linear(NU_ENO_Interval *ei);

void initialize_nu_eno_intervals(NU_ENO_Interval *ei,int N);


/******************************************************************/
/*************************nonunif_eno_shock.c**********************/
/******************************************************************/


int second_eno_check_for_shock(NU_ENO_Interval *prev, 
                               NU_ENO_Interval *cur, 
                               NU_ENO_Interval *next);

int second_eno_make_shock(NU_ENO_Interval *prev,
                          NU_ENO_Interval *cur,
                          NU_ENO_Interval *next);

int nu_eno_shock_print(NU_ENO_Interval *ei);

int free_nu_eno_shock(NU_ENO_Interval *ei);

/******************************************************************/
/******************************************************************/

#endif

#ifdef __cplusplus
}
#endif

