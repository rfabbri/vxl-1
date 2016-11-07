% Compares 8pt algthm vs. centroid of polygons . To be run after epi;
%
%
% - Selects many subset of 8 points (as would be done in ransac), and show the
% epipole positions for 8 points and from the best of my top-ranking polygons.
%

npts = size(ap0,1)
n_fig = 75;

n_fig_hists = 15;

%nruns = 1177
%nruns = 100;
nruns = 3;
%nruns = 20;

stdev_err = 0.5
outlier_fraction = 0.5

minset_npts = 7;

% square-rooted distance
distance_threshold = 1.96*stdev_err
n_inlier_threshold = (1-outlier_fraction)*npts


p_id_minset_lst = zeros(nruns,minset_npts);
e0_lst    = zeros(nruns,2);
e1_lst    = zeros(nruns,2);
vmeancost_newalg = zeros(nruns,1);
vpass    = zeros(nruns,1); % pass consensus?
ninliers = zeros(nruns,1); 
valid_run = zeros(nruns,1);
for r=1:nruns
  disp(['run #' num2str(r)]);
  p_id_minset = randperm(npts);
  p_id_minset = p_id_minset(1:minset_npts)
  p_id_minset_lst(r,:) = p_id_minset;
  pts0 = ap0(p_id_minset,:);
  pts1 = ap1(p_id_minset,:);


  try
    [epi_s,top_poly_idx,polys0,polys1,vcost,isheet_vcost] = epi_estimate(pts0,pts1,box);
  catch
    disp('Degeneracy occured -- skipping this run.'); 
    disp(['Error message: ' lasterror.message]);
    continue;
  end
  valid_run(r) = 1;
  [vcost_s,sorted_ip] = sort(vcost,'descend');

  plot_polys;

  e0_lst(r,:) = epi_s.e(1,:);
  e1_lst(r,:) = epi_s.e(2,:);


  [d,retval,n_inst,vv_newalg] = epi_geometric_error(epi_s,ap0,ap1);

  inlier_id = find(vv_newalg < distance_threshold);

  ninliers(r) = max(size(inlier_id));
  if ninliers(r) >= n_inlier_threshold
    vpass(r) = 1;
    figure(n_fig);
    hold on;
    cplot2(e0_lst(r,:),'gx');
    figure(n_fig+1);
    hold on;
    cplot2(e1_lst(r,:),'gx');
  else
    figure(n_fig);
    hold on;
    cplot2(e0_lst(r,:),'rx');
    figure(n_fig+1);
    hold on;
    cplot2(e1_lst(r,:),'rx');
  end


  geom_err_newalg = mean(vv_newalg);

  vmeancost_newalg(r) = geom_err_newalg;
%  keyboard;
end

disp('Finished main runs - now finalizing.');
keyboard;

v_id= find(valid_run == 1);
e0_lst    = e0_lst(v_id,:);
e1_lst    = e1_lst(v_id,:);
vmeancost_newalg = vmeancost_newalg(v_id);
vpass    = vpass(v_id); % pass consensus?
ninliers = ninliers(v_id);
p_id_minset_lst = p_id_minset_lst(v_id,:);



[m,min_id] = min(vmeancost_newalg);

p_id_minset = p_id_minset_lst(min_id,:)


pts0 = ap0(p_id_minset,:);
pts1 = ap1(p_id_minset,:);

if (size(pts0,1) < 3 || size(pts1,1) < 3)
  warning('Weird points');
  keyboard;
else
  disp('Points are ok.');
  epi_s = epi_estimate(pts0,pts1,box);
end

[d,retval,n_inst,vv_newalg] = epi_geometric_error(epi_s,ap0,ap1);
d = mean(vv_newalg);
geom_err_newalg = max(vv_newalg);
geom_err_newalg_max = geom_err_newalg;
geom_err_newalg_avg = mean(vv_newalg)

epipole_position_exp_plot_newalg;
