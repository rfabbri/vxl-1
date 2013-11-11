% Compares 8pt algthm vs. centroid of polygons . To be run after epi;
%
%
% - Selects many subset of 8 points (as would be done in ransac), and show the
% epipole positions for 8 points and from the best of my top-ranking polygons.
%

npts = size(ap0,1)
n_fig = 25;

n_fig_hists = 15;

if ~exist('nruns')
  nruns = 1177
end

stdev_err = 0.5
outlier_fraction = 0.5

% square-rooted distance
distance_threshold = 1.96*stdev_err
n_inlier_threshold = (1-outlier_fraction)*npts





p_id8_lst =zeros(nruns,8);
e0_lst = zeros(nruns,2);
e1_lst = zeros(nruns,2);
vmeancost_8pt = zeros(nruns,1);
vpass = zeros(nruns,1); % pass consensus?
ninliers = zeros(nruns,1); 
for r=1:nruns
  p_id8 = randperm(npts);
  p_id8 = p_id8(1:8);
  p_id8_lst(r,:) = p_id8;
  pts0 = ap0(p_id8,:);
  pts1 = ap1(p_id8,:);

  fm = fmatrix(pts0,pts1,'vpgl8pt');
  fm8pt = fm;
  fm_fail = eye(3);
  fm_fail(3,3)=0;
  if (norm(fm-fm_fail) > 0.0001) 
    a = null(fm);
    e0_new = [a(1)/a(3) a(2)/a(3)];
    a = null(fm');
    e1_new = [a(1)/a(3) a(2)/a(3)];
  end

  e0_lst(r,:) = e0_new;
  e1_lst(r,:) = e1_new;


  [d,vv_8pt] = epi_geometric_error_f(fm8pt,ap0,ap1);

  inlier_id = find(vv_8pt < distance_threshold);

  ninliers(r) = max(size(inlier_id));
  if ninliers(r) >= n_inlier_threshold
    vpass(r) = 1;
  end

  geom_err_8pt = mean(vv_8pt);

  vmeancost_8pt(r) = geom_err_8pt;
end


[m,min_id] = min(vmeancost_8pt);

p_id8 = p_id8_lst(min_id,:)


pts0 = ap0(p_id8,:);
pts1 = ap1(p_id8,:);
fm = fmatrix(pts0,pts1,'vpgl8pt');
fm8pt = fm;
fm_fail = eye(3);
fm_fail(3,3)=0;
if (norm(fm-fm_fail) > 0.0001) 
  a = null(fm);
  e0_new = [a(1)/a(3) a(2)/a(3)];
  a = null(fm');
  e1_new = [a(1)/a(3) a(2)/a(3)];
end

[d,vv_8pt] = epi_geometric_error_f(fm8pt,ap0,ap1);
d = mean(vv_8pt);
geom_err_8pt = max(vv_8pt);
geom_err_8pt_max = geom_err_8pt;
geom_err_8pt_avg = mean(vv_8pt)

epipole_position_exp_plot;
