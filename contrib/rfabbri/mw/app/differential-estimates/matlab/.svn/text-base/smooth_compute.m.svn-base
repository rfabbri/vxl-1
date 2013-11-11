% crv=gama1_round;
% [cs,nrm]=csm(crv,0.3,7);


% ==== ELLIPSE ===

[fel_fine,el_fine,mytheta_fine]=ellipse(0.002,10,30,60,60);
[k_el_fine,kdot_el_fine,t_el_fine,t_angle_el_fine]=kellipse(mytheta_fine,10,30);

[fel,el,mytheta]=ellipse(0.08,10,30,60,60);
[k_el,kdot_el,t_el,t_angle_el,nrm_el]=kellipse(mytheta,10,30);

k_el = k_el';
k_vec_el = nrm_el.*[k_el k_el];

%mywrite('dat/t_angle_el.dat',t_angle_el);
% ---

% === DATASET 2 ===
%[fel_fine,el_fine,mytheta_fine,k_el_fine,kdot_el_fine,t_el_fine,t_angle_el_fine,nrm_el_fine]=dataset2(1000);


%% WARNING: pel will change all time (random) bellow. Uncomment only once
%pel=perturb_normal(el,nrm_el); 
pel = myreadv2('dat/perturbed-el79-20050922.dat');
%% ---

%pel = fel_fine;

%[mycs,nrm]=csm(fel_fine,1,100);
%[mycs,nrm]=csm(fel_fine,1,5);
%[mycs,nrm]=csm(fel_fine,1,7);

%[mycs,nrm]=csm(pel,1,6);   %%%: pel = output from perturb*.m
%[mycs,nrm]=csm(pel,1,3);  
[mycs,nrm]=csm(pel,1,5);  
%s = myarclength(pel);
%mycs = el;

% ISOLATE FOR A SEGMENT before computation:
idx = 36:42;
%mycs = mycs(idx,:);
%k_el = k_el(idx);
%kdot_el = kdot_el(idx);
%el = el(idx,:);

%k_vec_el = k_vec_el(idx,:);


%mywrite('dat/gama1-sm.dat',cs_k); 
%mywrite('dat/gama1-sm.dat',mycs);
mywrite('dat/gama1-sm.dat',mycs);
%mywrite('dat/gama1-sm.dat',el);
%mywrite('dat/gama1-sm.dat',el_fine);
%mywrite('dat/gama1-sm.dat',mycss);


% === RUN & LOAD RESULTS ===
%n_it_pos = '4000';
%n_it_pos = '1';
unix(['./cmd 5 ' n_it_pos ' -arc']);
%unix('./cmd 5 4000 -arc');
% THIS IS BEST, BUT IS FAILING FOR dataset2: unix('./cmd 5');
%unix('./cmd 20');
load_all
% ISOLATE FOR A SEGMENT 
%pel  = pel(idx,:);

k_gsm = k_ref; %gsmi(k_ref,1,len_sm,0);

k_vec_gsm = n_ref.*[k_gsm k_gsm];

%sum(dk_posref)
%sum(dt_posref.*dt_posref + dk_posref.*dk_posref)
sum(dt_posref_arc.*dt_posref_arc)

