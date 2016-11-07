gama1 = myreadv('/tmp/gama1.dat');
gama1_img = load('/tmp/gama1_img.txt');
gama2 = myreadv('/tmp/gama2.dat');
gama2_img = load('/tmp/gama2_img.txt');
k     = myread('/tmp/k.dat');
kdot  = myread('/tmp/kdot.dat');
theta = myread('/tmp/theta.dat');
C = myreadv('/tmp/Gama.dat');
g1 = myread('/tmp/g1.dat');
K_rec = myread('/tmp/rec_K.dat');
Kdot_rec = myread('/tmp/rec_Kdot.dat');
Tau_rec = myread('/tmp/rec_Tau.dat');
T_rec   = myreadv('/tmp/rec_T.dat');
C_rec   = myreadv('/tmp/rec.dat');
N_rec   = myreadv('/tmp/rec_N.dat');
B_rec   = myreadv('/tmp/rec_B.dat');
gama1_round = load('dat/gama1_round.txt');
t_angle_g = myread('dat/t_angle_geno.dat');
t_g = myreadv2('dat/tgt_geno.dat');
gama1_g  = myreadv2('dat/gama1_geno.dat');
gama1_sm = myreadv2('dat/gama1-sm.dat');

t_angle_sm = myread('dat/t_angle_geno-sm.dat');
t_sm = myreadv2('dat/tgt_geno-sm.dat');
gama1_g_sm = myreadv2('dat/gama1_geno-sm.dat');
k_sm     = myread('dat/k_geno-sm.dat');
kdot_sm  = myread('dat/kdot_geno-sm.dat');
len_sm   = myread('dat/len_geno-sm.dat');
dkdot_sm = myread('dat/dkdot_geno-sm.dat');
dk_sm    = myread('dat/dk_geno-sm.dat');
dt_sm    = myread('dat/dt_geno-sm.dat');

t_angle_sm_arc = myread('dat/t_angle_geno-sm-arc.dat');
t_sm_arc   = myreadv2('dat/tgt_geno-sm-arc.dat');
gama1_g_sm = myreadv2('dat/gama1_geno-sm-arc.dat');
k_sm_arc     = myread('dat/k_geno-sm-arc.dat');
kdot_sm_arc  = myread('dat/kdot_geno-sm-arc.dat');
len_sm_arc   = myread('dat/len_geno-sm-arc.dat');
dkdot_sm_arc = myread('dat/dkdot_geno-sm-arc.dat');
dk_sm_arc    = myread('dat/dk_geno-sm-arc.dat');
dt_sm_arc    = myread('dat/dt_geno-sm-arc.dat');

n_sm_arc     = [-t_sm_arc(:,2) t_sm_arc(:,1)];

gama1_sm_circ = myreadv2('dat/gama1_geno-super-sample-circle-sm.dat');
gama1_sm_arc  = gama1_sm_circ;
gama1_sm_ss   = myreadv2('dat/gama1_geno-super-sample-sm.dat');

gama1_g_ref = myreadv2('dat/gama1_geno-ref.dat');
t_ref       = myreadv2('dat/tgt_geno-ref.dat');
t_angle_ref = myread('dat/t_angle_geno-ref.dat');
k_ref       = myread('dat/k_geno-ref.dat');
kdot_ref    = myread('dat/kdot_geno-ref.dat');
len_ref     = myread('dat/len_geno-ref.dat');
dk_ref      = myread('dat/dk_geno-ref.dat');
dt_ref      = myread('dat/dt_geno-ref.dat');
dkdot_ref   = myread('dat/dkdot_geno-ref.dat');
gama1_ref_ss= myreadv2('dat/gama1_geno-super-sample-ref.dat');

n_sm     = [-t_sm(:,2) t_sm(:,1)];
n_ref    = [-t_ref(:,2) t_ref(:,1)];
k_vec_ref= n_ref.*[k_ref k_ref];
k_vec_sm = n_sm.*[k_sm k_sm];

% Read positional gradient descent data
gama1_g_posref = myreadv2('dat/gama1_geno-posref.dat');
t_posref       = myreadv2('dat/tgt_geno-posref.dat');
t_angle_posref = myread('dat/t_angle_geno-posref.dat');
k_posref       = myread('dat/k_geno-posref.dat');
kdot_posref    = myread('dat/kdot_geno-posref.dat');
len_posref     = myread('dat/len_geno-posref.dat');
dk_posref      = myread('dat/dk_geno-posref.dat');
dt_posref      = myread('dat/dt_geno-posref.dat');
dkdot_posref   = myread('dat/dkdot_geno-posref.dat');
gama1_posref_ss = myreadv2('dat/gama1_geno-super-sample-posref.dat');
gama1_posref_circ = myreadv2('dat/gama1_geno-super-sample-posref-circle.dat');

gama1_g_posref_arc = myreadv2('dat/gama1_geno-posref-arc.dat');
t_angle_posref_arc = myread('dat/t_angle_geno-posref-arc.dat');
k_posref_arc       = myread('dat/k_geno-posref-arc.dat');
len_posref_arc     = myread('dat/len_geno-posref-arc.dat');
dk_posref_arc      = myread('dat/dk_geno-posref-arc.dat');
dt_posref_arc      = myread('dat/dt_geno-posref-arc.dat');
grad_pos = myread('dat/grad-pos.dat');

n_posref    = [-t_posref(:,2) t_posref(:,1)];
k_vec_posref= n_posref.*[k_posref k_posref];



dk_vec_arc = n_ref.*[dk_sm_arc dk_sm_arc];
dt_vec_arc = n_ref.*[dt_sm_arc dt_sm_arc];

dk_vec_posref_arc = n_ref.*[dk_posref_arc dk_posref_arc];
dt_vec_posref_arc = n_ref.*[dt_posref_arc dt_posref_arc];

grad_vec_pos = n_ref.*[grad_pos grad_pos];
