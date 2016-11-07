//---------------------------------------------------------------------
// This is brcv/rec/dbasn/tests/dbasn_test_artificial_def.h
//:


#ifndef dbasn_test_artificial_def_h_
#define dbasn_test_artificial_def_h_

extern float n_equal[];
extern float n_ident[];
extern float n_scale[];
extern float n_modi1[];
extern float n_modi2[];
extern float n_modi3[];

extern float n_rotat[];

extern float n_flip_id[];
extern float n_flip_m1[];
extern float n_flip_m2[];
extern float n_flip_m3[];

extern float n_pertb[];
extern float n_pertb2[];
extern float n_pert_rot[];

extern char n_types[];

extern float l_zeros[];
extern float l_equal[];
extern float l_ident[];
extern float l_scale[];
extern float l_modi1[];
extern float l_modi2[];
extern float l_modi3[];

extern float l_rotat[];
extern float l_flip_id[];

extern float l_pertb[];
extern float l_pertb2[];
extern float l_pert_rot[];

extern char l_types[];

extern float c_zeros[];
extern float c_equal[];
extern float c_ident[];
extern float c_scale[];
extern float c_modi1[];
extern float c_modi2[];
extern float c_modi3[];

extern float c_rotat[];

extern float c_pertb[];
extern float c_pertb2[];
extern float c_pert_rot[];

extern int labelgG[];

extern int labelg_rotat[];

extern int labelg_flip2[];

//##################################################################

#define G_N_CORNERS     36
#define G_N_CORNERS_2   28

extern int l_nodes[][2];
extern int c_nodes[][3];
extern int c_nodes2[][3];
extern int str_labelgG[];

#endif

