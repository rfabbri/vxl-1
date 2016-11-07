% Compares 8pt algthm vs. centroid of polygons . To be run after epi;
%
%
% FOR NOW - run epi.m before this file
%
% - Selects many subset of 8 points (as would be done in ransac), and show the
% epipole positions for 8 points and from the best of my top-ranking polygons.
%

n_fig = 55;

n_fig_hists = 15;

nruns = 1177
%nruns = 29;
%nruns = 100;
%nruns = 5;

stdev_err = 0.5+0.5; % 0.5 for perturbation; 0.5 for the initial data noise
sig = 0.5;
outlier_fraction = 0.5
datascript = 'synth_point_data';
%datascript = 'oxford_corridor_point_data';

eval(datascript)

minset_npts = 8;

% square-rooted distance
distance_threshold = 1.96*stdev_err


[d,e0_opt,e1_opt,epi_s] = epioptimize_function(pts0,pts1,[e0 e1]','precision');

e0_opt = e0_opt';
e1_opt = e1_opt';
d_nhood = norm(e0 - e0_opt) + norm(e1 - e1_opt);
optcost_nhood = d;


e0_lst    = zeros(nruns,2);
e1_lst    = zeros(nruns,2);
vmeancost_newalg = zeros(nruns,1);
vpass    = zeros(nruns,1); % pass consensus?
ninliers = zeros(nruns,1); 
valid_run = zeros(nruns,1);
for r=1:nruns
  disp(['run #' num2str(r)]);

%  angles_s = ['30,' num2str(30+180)];
%  angles_s = '30,60';
  eval(datascript);
  p_id = p_id(1:minset_npts);
  pts0 = ap0(p_id,:);
  pts1 = ap1(p_id,:);
  npts = size(ap0,1);
  n_inlier_threshold = (1-outlier_fraction)*npts

  try
  [epi_s,top_poly_idx,polys0,polys1,vcost,isheet_vcost] = epi_estimate(pts0,pts1,box);
  catch
    disp('Degeneracy occured -- skipping this run.'); 
    disp(['Error message epipole_position_exp_noise_newalg: ' lasterror.message]);
    continue;
  end
  valid_run(r) = 1;

  [vcost_s,sorted_ip] = sort(vcost,'descend');

%  plot_polys;

  e0_lst(r,:) = epi_s.e(1,:);
  e1_lst(r,:) = epi_s.e(2,:);


  if norm(e0_lst(r,:) - e0) + norm(e1_lst(r,:) - e1) > d_nhood+100;
    disp('candidate far epipole found');
    [d,retval,n_inst,vv_newalg] = epi_geometric_error(epi_s,pts0,pts1);
    if d < sig  
      % Further evaluate:
      % - optimize current solution
      % - make sure the distance is still large
      [optcost_new,e0_new_opt,e1_new_opt,epi_s_opt] = epioptimize_function(pts0,pts1,[e0_lst(r,:) e1_lst(r,:)]','precise');
      e0_new_opt = e0_new_opt';
      e1_new_opt = e1_new_opt';
      
      if optcost_new < optcost_nhood
        if norm(e0_new_opt - e0) + norm(e1_new_opt - e1) > d_nhood + 100
          disp('found far epipole w low cost');
          e0_gt = e0;
          e1_gt = e1;
          e0_gt_opt = e0_opt;
          e1_gt_opt = e1_opt;
          e0_far = e0_new_opt;
          e1_far = e1_new_opt;

          keyboard;
        end
      end
      
    end
  end

  [d,retval,n_inst,vv_newalg] = epi_geometric_error(epi_s,ap0,ap1);

  inlier_id = find(vv_newalg < distance_threshold);

  ninliers(r) = max(size(inlier_id));
  if ninliers(r) >= n_inlier_threshold
    vpass(r) = 1;
%    figure(n_fig);
%    hold on;
%    cplot2(e0_lst(r,:),'gx');
%    figure(n_fig+1);
%    hold on;
%    cplot2(e1_lst(r,:),'gx');
  else
%    figure(n_fig);
%    hold on;
%    cplot2(e0_lst(r,:),'rx');
%    figure(n_fig+1);
%    hold on;
%    cplot2(e1_lst(r,:),'rx');
  end


  geom_err_newalg = mean(vv_newalg);

  vmeancost_newalg(r) = geom_err_newalg
end

v_id= find(valid_run == 1);
e0_lst    = e0_lst(v_id,:);
e1_lst    = e1_lst(v_id,:);
vmeancost_newalg = vmeancost_newalg(v_id);
vpass    = vpass(v_id); % pass consensus?
ninliers = ninliers(v_id);
nruns = max(size(v_id));

[m,min_id] = min(vmeancost_newalg);

%pts0 = ap0(p_id,:);
%pts1 = ap1(p_id,:);

%epi_s = epi_estimate(pts0,pts1,box);

%[d,retval,n_inst,vv_newalg] = epi_geometric_error(epi_s,ap0,ap1);
%d = mean(vv_newalg);
%geom_err_newalg = max(vv_newalg);
%geom_err_newalg_max = geom_err_newalg;
%geom_err_newalg_avg = mean(vv_newalg)

epipole_position_exp_plot_newalg;
