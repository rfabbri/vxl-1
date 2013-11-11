==================================

MULTIVIEW STEREO EXPERIMENTS
----------------------------

FILES

sexp - main C++ command
sexp*.sh - shell scripts
matlab/plot_sexp_batch.m - plot results



==================================

Gaussian smoothing/derivative test:

1) define f, t (see beginning of plot_derivs.m)
2) define sigma
3) plot_derivs;


==================================

To run estimation on dataset2

1)smooth_compute_ds2; gplot_perturb;

==================================

Evolution along normal wrt. gaussian-smoothed curvature signal.

+ 1st, csm curve with optimal parameter (6, according to notes)
  - put in smooth_compute.m and properly alter this file
  - edit gplot_perturb;
  - smooth_compute; gplot_perturb;

- variables k_vec_ref and k_vec_sm are defined in load_all.m

- Gaussian-smooth k_ref (or k_sm):
  
  These are defined **in smooth_compute** :

  k_gsm = gsmi(k_ref,2,len_sm,0);
  k_vec_gsm= n_ref.*[k_gsm k_gsm];

- Evolve:

  cs_k=csm2(mycs,0.5,k_vec_gsm);

  cplot2(cs_k);

  - make sure you modify smooth_compute to save cs_k as input to geno 

  smooth_compute; gplot_perturb;

- there is also k_sm_iterative. We have to know well what are the I/O's!

- Parameters of this method:
  Critical:
  - ammount of gsm of curvature
  - stepsize to which to evolve along curvature
  - number of steps/iteration on this

  - ammount of gsm of curvature
  - step to which to evolve along curvature
  - number of steps/iteration on this
  Others:
  - ammount of initial csm
  - number of steps in gradient descent, qsi
