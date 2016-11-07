% SAME AS SMOOTH_COMPUTE, BUT THIS IS A SHORTCUT for running on dataset2 using
% the best parameters.


% crv=gama1_round;
% [cs,nrm]=csm(crv,0.3,7);


% ==== ELLIPSE ===

%[fel_fine,el_fine,mytheta_fine]=ellipse(0.002,10,30,60,60);
%[k_el_fine,kdot_el_fine,t_el_fine,t_angle_el_fine]=kellipse(mytheta_fine,10,30);

%[fel,el,mytheta]=ellipse(0.08,10,30,60,60);
%[k_el,kdot_el,t_el,t_angle_el,nrm_el]=kellipse(mytheta,10,30);
%mywrite('dat/t_angle_el.dat',t_angle_el);
% ---

% === DATASET 2 ===
[fel_fine,el_fine,mytheta_fine,k_el_fine,kdot_el_fine,t_el_fine,t_angle_el_fine,nrm_el_fine]...
   = dataset2(10000);

[fel,el,mytheta,k_el,kdot_el,t_el,t_angle_el,nrm_el]...
   = dataset2(200);


%% WARNING: pel will change all time (random) bellow. Uncomment only once
%pel=perturb_normal(el,nrm_el); 
%% ---

%[mycs,nrm]=csm(fel_fine,1,100);
%[mycs,nrm]=csm(fel_fine,1,5);
%[mycs,nrm]=csm(fel_fine,1,7);

%[mycs,nrm]=csm(pel,1,6);   %%%: pel = output from perturb*.m
%mycs = gsmi2d(pel,5);
%mycs = gsmi2d(el,5);
%mycs = el;
s = myarclength(pel);
%mycs = gsmi2d(pel,7,s,0);
mycs = gsmi2d(pel,5,(1:max(size(pel)))',0);

%mywrite('dat/gama1-sm.dat',cs_k); 
mywrite('dat/gama1-sm.dat',mycs);
%mywrite('dat/gama1-sm.dat',el);
%mywrite('dat/gama1-sm.dat',el_fine);
%mywrite('dat/gama1-sm.dat',mycss);


% === RUN & LOAD RESULTS ===
unix('./cmd 1');
% THIS IS BEST, BUT IS FAILING FOR dataset2: unix('./cmd 5');
%unix('./cmd 20');
load_all
