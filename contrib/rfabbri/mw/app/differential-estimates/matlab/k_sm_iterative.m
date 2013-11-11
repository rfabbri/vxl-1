% iteratively evolve

% Make sure that smooth_compute is properly set up


% Input: cs_k == initial curve. It uses variables given by load_all

nit = 5

for i=1:nit
   
  cs_k = csm2(cs_k,0.02,k_vec_gsm);
  smooth_compute;
end
gplot_perturb;
