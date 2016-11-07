% Compares 8pt algthm vs. centroid of polygons . To be run after epi;
%
%
% FOR NOW - run epi.m before this file
%
% - Selects many subset of 8 points (as would be done in ransac), and show the
% epipole positions for 8 points and from the best of my top-ranking polygons.
%

if ~exist('n_fig_plot_exp')
  n_fig = 55;
else
  n_fig = n_fig_plot_exp;
end

n_fig_hists = 15;

if ~exist('nruns')
  nruns = 1177
  %nruns = 29;
  %nruns = 100;
  %nruns = 5;
end

stdev_err = 0.5+0.5; % 0.5 for perturbation; 0.5 for the initial data noise
if ~exist('sig')
  sig = 0.5;
end

outlier_fraction = 0.5

if ~exist('datascript')
  datascript = 'synth_point_data';
  %datascript = 'oxford_corridor_point_data';
end
eval(datascript)

minset_npts = 8;

% square-rooted distance
distance_threshold = 1.96*stdev_err

npts = size(ap0,1);
n_inlier_threshold = (1-outlier_fraction)*npts

%% ------- Main code -------

e0_lst    = zeros(nruns,2);
e1_lst    = zeros(nruns,2);
vmeancost_newalg_minset = zeros(nruns,1);
vmaxcost_newalg_minset = zeros(nruns,1);
vmeancost_newalg = zeros(nruns,1);
vmaxcost_newalg = zeros(nruns,1);
vpass    = zeros(nruns,1); % pass consensus?
ninliers = zeros(nruns,1); 
valid_run = zeros(nruns,1);
n_valid = 0;
max_nruns = 2*nruns;
r = 1
not_enough_runs = true;
while not_enough_runs
  disp(['run #' num2str(r)]);

  eval(datascript);
  if ~exist('dont_set_p_id')
    p_id = p_id(1:minset_npts);
    pts0 = ap0(p_id,:);
    pts1 = ap1(p_id,:);
  end

  try
    [epi_s,top_poly_idx,polys0,polys1,vcost,isheet_vcost] = epi_estimate(pts0,pts1,box);
  catch
    disp(['Error message epipole_position_exp_noise_newalg: ' lasterror.message]);
    disp('Degeneracy occured -- skipping this run.'); 
    continue;
  end
  valid_run(r) = 1;
  n_valid = n_valid +1;

  [vcost_s,sorted_ip] = sort(vcost,'descend');

%  plot_polys;

  e0_lst(r,:) = epi_s.e(1,:);
  e1_lst(r,:) = epi_s.e(2,:);


%  if norm(e0_lst(r,:) - e0) + norm(e1_lst(r,:) - e1) > 1000
%    [d,retval,n_inst,vv_newalg] = epi_geometric_error(epi_s,pts0,pts1);
%    if d < sig  
%      disp('found far epipole w low cost');
%      keyboard;
%    end
%  end

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
  vmaxcost_newalg(r)  = max(vv_newalg);

  [d,retval,n_inst,vv_newalg_minset] = epi_geometric_error(epi_s,pts0,pts1);
  vmeancost_newalg_minset(r) = mean(vv_newalg_minset);
  vmaxcost_newalg_minset(r)  = max(vv_newalg_minset);
  r = r+1;

  if n_valid == nruns
    not_enough_runs = false; 
  elseif r == max_nruns+1
      not_enough_runs = false; 
      warning(['Not enough runs - only ' num2str(n_valid) ' are valid.']);
  end
end

v_id= find(valid_run == 1);
e0_lst    = e0_lst(v_id,:);
e1_lst    = e1_lst(v_id,:);

vmeancost_newalg = vmeancost_newalg(v_id);
vmaxcost_newalg  = vmaxcost_newalg(v_id);

vmeancost_newalg_minset = vmeancost_newalg_minset(v_id);
vmaxcost_newalg_minset  = vmaxcost_newalg_minset(v_id);
vmaxcost = vmaxcost_newalg_minset; 
vmeancost = vmeancost_newalg_minset; 

vpass    = vpass(v_id); % pass consensus?
ninliers = ninliers(v_id);

[m,min_id] = min(vmeancost_newalg);

%pts0 = ap0(p_id,:);
%pts1 = ap1(p_id,:);

%epi_s = epi_estimate(pts0,pts1,box);

%[d,retval,n_inst,vv_newalg] = epi_geometric_error(epi_s,ap0,ap1);
%d = mean(vv_newalg);
%geom_err_newalg = max(vv_newalg);
%geom_err_newalg_max = geom_err_newalg;
%geom_err_newalg_avg = mean(vv_newalg)

%epipole_position_exp_plot_newalg;
