% This script load and plot the distribution of OCM cost of fragments in a
% xshock graph model of giraffes
% (c) Nhon Trinh
% Date: Nov 15, 2008

clear all;
close all;

%% Compute ccm distribution for object classes from collected ccm costs

%%
%input_folder = 'D:\vision\projects\symseg\xshock'; % should never use
input_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph';

% %--------------------------------------------------------------------------
% % giraffes
% input_xgraph_foldername = 'giraffes-xgraph'
% ccm_cost_filename = 'giraffes_ccm_cost-closest_oriented_edge-kovesi-I_15-len_10_pos-2009feb22.txt';
% num_contour_frags = 24;

% %--------------------------------------------------------------------------
% % bottles
% input_xgraph_foldername = 'bottles-xgraph'
% ccm_cost_filename = 'bottles_ccm_cost-closest_oriented_edge-kovesi_len_10_thresh_15-2009mar01_pos.txt';
% num_contour_frags = 16;

% %--------------------------------------------------------------------------
% % swans
% input_xgraph_foldername = 'swans-xgraph'
% ccm_cost_filename = 'swans_ccm_cost-closest_oriented_edge-kovesi_len_10_thresh_15-2009mar09_pos.txt';
% num_contour_frags = 18;

%--------------------------------------------------------------------------
% mugs
%input_xgraph_foldername = 'mugs-xgraph'
%ccm_cost_filename = 'mugs_ccm_cost-closest_oriented_edge-kovesi_len_10_thresh_15-2009april15_pos.txt';
%ccm_cost_filename = 'mugs_ccm_cost-root_vid-20-closest_oriented_edge-kovesi_len_10_thresh_15-2009april16_pos.txt';
%num_contour_frags = 28;



%--------------------------------------------------------------------------
% applelogos
input_xgraph_foldername = 'applelogos-xgraph'
ccm_cost_filename = 'applelogos_prototype1_ccm_cost-closest_oriented_edge-kovesi_len_10_thresh_15-2009april20_pos.txt';
%ccm_cost_filename = 'applelogos_prototype2_ccm_cost-closest_oriented_edge-kovesi_len_10_thresh_15-2009april20_pos.txt';
num_contour_frags = 18;



% output_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph\giraffes-xgraph';
output_folder = 'D:\vision\projects\symseg\xshock\xshock-graph';




%% Process

% extract the name used for other output files
[pathstr, ccm_model_filename, ext, versn] = fileparts(ccm_cost_filename);

% read data file
fid = fopen(fullfile(input_folder, input_xgraph_foldername, ccm_cost_filename), 'r');

C = textscan(fid, '#Contour-Chamfer-Matching\n', 1);

C = textscan(fid, '#tol_near_zero %f\n', 1);
tol_near_zero = C{1}(1);

C = textscan(fid, '#edge_threshold %f\n', 1);
edge_threshold = C{1}(1);

C = textscan(fid, '#distance_threshold %f\n', 1);
distance_threshold = C{1}(1);

C = textscan(fid, '#ccm_gamma %f\n', 1);
ocm_gamma = C{1}(1);

C = textscan(fid, '#ccm_lambda %f\n', 1);
ocm_lambda = C{1}(1);

C = textscan(fid, '#nbins_0topi %d\n', 1);
nbins_0topi = C{1}(1);


C = textscan(fid, '#root_vid %d\n', 1);
root_vid = C{1}(1);

C = textscan(fid, '#major_child_eid %d\n', 1);
major_child_eid = C{1}(1);

C = textscan(fid, '#contour_frag_id ');
C = textscan(fid, '%s', num_contour_frags);
labels = C{1};



% read data
C = textscan(fid, '%f');
data = C{1};
fclose(fid);

% reformat the data in a table format
data = reshape(data, num_contour_frags, []);
data = data';
data(find(data <= 0)) = 0.001;


%% Fit Weibull distribution to the data

fprintf(1, 'Fit Weibull distribution to contour ocm data...\n');

wbl_k = zeros(num_contour_frags, 1);
wbl_lambda = zeros(num_contour_frags, 1);
wbl_avg_logl = zeros(num_contour_frags, 1);
for i = 1 : num_contour_frags
  r = data(:,i);
  [parmhat, parmci] = wblfit(r);
  [nlogL, avar] = wbllike(parmhat, r);
  fprintf(1, 'contour fragment %s: lambda=%g, k=%g, nlogL=%g\n', char(labels(i)), ... 
    parmhat(1), parmhat(2), nlogL);
  wbl_lambda(i) = parmhat(1);
  wbl_k(i) = parmhat(2);
  wbl_avg_logl(i) = -nlogL / length(r);
end;


%% Plot the costs

% compute max value in the histograms
max_count = 0;
for i = 1 : num_contour_frags
  aa = hist(data(:, i), 0.05 : 0.05 : 0.95);
  max_count = max(max_count, max(aa));
end;

% round it to 100
max_count = 100 * ceil ( max_count / 100);

h = figure(1);
for i = 1 : num_contour_frags
  subplot(ceil(num_contour_frags / 4), 4, i, 'replace');
  
  % plot the histogram
  hist(data(:, i), 0.05 : 0.05 : 0.95);

  % plot the fitted Weibull distribution
  x = [0 : 0.01 : 1];
  y = wblpdf(x, wbl_lambda(i), wbl_k(i));
  
  % need to scale the distribution to fit the histogram
  scale_factor = length(data(:, i)) * 0.05;
  y = y * scale_factor;
  hold on;
  plot(x, y, 'r-', 'linewidth', 2);
  hold off;
  
  grid off;
  title([labels(i), 'wbllike= ', num2str(wbl_avg_logl(i))]);

  axis([0, 1, 0, max_count]);
%  title([labels(i)]);
  h = findobj(gca,'Type','patch');
  set(h,'FaceColor','b','EdgeColor','w')
end;

%%
h2 = figure(2);
for i = 1 : num_contour_frags
  subplot(ceil(num_contour_frags / 4), 4, i, 'replace');
  
  % plot the log-likelihood of the fitted Weibull distributions
  x = [0 : 0.01 : 1];
  y = log(wblpdf(x, wbl_lambda(i), wbl_k(i)));
  
  plot(x, y, 'g-', 'linewidth', 2);
  
  grid off;
  title([labels(i), 'wbllike= ', num2str(wbl_avg_logl(i))]);
  axis([0, 1, -5, 5]);

  h = findobj(gca,'Type','patch');
  set(h,'FaceColor','b','EdgeColor','w')
end;










%% Fit beta-distribution
beta_a = zeros(num_contour_frags, 1);
beta_lambda = zeros(num_contour_frags, 1);
beta_avg_logl = zeros(num_contour_frags, 1);
for i = 1 : num_contour_frags
  r = data(:,i);
  r = max(r, 1e-5 * ones(size(r)));
  r = min(r, (1-1e-5) * ones(size(r)));
  
  if (length(unique(r)) == 1)
      beta_a(i) = 1;
      beta_b(i) = 1;
      beta_avg_logl(i) = 0;
    continue;
  end;
  
  phat = betafit(r);
  [nlogL, avar] = betalike(phat, r);
  fprintf(1, 'contour fragment %s: a=%g, a=%g, nlogL=%g\n', char(labels(i)), ... 
    phat(1), phat(2), nlogL);
  beta_a(i) = phat(1);
  beta_b(i) = phat(2);
  beta_avg_logl(i) = -nlogL / length(r);
end;


%% Plot the cost histogram and the fitted beta distribution

h3 = figure(3);
for i = 1 : num_contour_frags
  subplot(ceil(num_contour_frags / 4), 4, i, 'replace');
  
  % plot the histogram
  hist(data(:, i), 0.05 : 0.05 : 0.95);

  % plot the fitted Weibull distribution
  x = [0 : 0.01 : 1];
  y = betapdf(x, beta_a(i), beta_b(i));
  
  % need to scale the distribution to fit the histogram
  scale_factor = length(data(:, i)) * 0.05;
  y = y * scale_factor;
  hold on;
  plot(x, y, 'r-', 'linewidth', 2);
  hold off;
  
  grid off;
  title([labels(i), 'beta-logl= ', num2str(beta_avg_logl(i))]);

  axis([0, 1, 0, max_count]);
%  title([labels(i)]);
  h = findobj(gca,'Type','patch');
  set(h,'FaceColor','b','EdgeColor','w')
end;

%%
h4 = figure(4);
for i = 1 : num_contour_frags
  subplot(ceil(num_contour_frags / 4), 4, i, 'replace');
  
  % plot the log-likelihood of the fitted Weibull distributions
  x = [0 : 0.01 : 1];
  y = log(betapdf(x, beta_a(i), beta_b(i)));
  
  plot(x, y, 'g-', 'linewidth', 2);
  
  axis([0, 1, -5, 5]);
  grid off;
  title([labels(i), 'beta-logl= ', num2str(beta_avg_logl(i))]);
  h = findobj(gca,'Type','patch');
  set(h,'FaceColor','b','EdgeColor','w')
end;



%% Print fitting results to a file
if (0)
ocm_model_file = fullfile(output_folder, [ccm_model_filename, '.txt']);
fid = fopen(ocm_model_file, 'w');
fprintf(fid, 'Contour-OCM-Model\n');
fprintf(fid, 'version 1.0\n');
fprintf(fid, 'num-contour-frags %d\n', length(wbl_k));

for i = 1 : num_contour_frags
  fprintf(fid, '\ncontour-identifier %s\n', char(labels(i)));
  fprintf(fid, 'distribution-type %s\n', 'Weibull');
  fprintf(fid, 'scale %g\n', wbl_lambda(i));
  fprintf(fid, 'shape %g\n', wbl_k(i));
  fprintf(fid, 'location %g\n', 0);
  fprintf(fid, 'avg-log-likelihood %g\n', wbl_avg_logl(i));
end;

fclose(fid);

end;

%% Print fitting results to an XML file

% sort the data
edge_id = zeros(length(labels), 1);
side_id = zeros(length(labels), 1);

for i = 1 : num_contour_frags
  label = char(labels(i));
  % parse the label
  A = sscanf(label, '%d-%c');
  edge_id(i) = A(1);
  side_id(i) = char(A(2));
end;

[edge_id, idx] = sortrows(edge_id);
side_id = side_id(idx);
wbl_lambda = wbl_lambda(idx);
wbl_k = wbl_k(idx);
wbl_avg_logl = wbl_avg_logl(idx);


% form filename of ccm model
xml_file = fullfile(output_folder, [ccm_model_filename, '.xml']);

dbsks_xgraph_ccm_model = [];
dbsks_xgraph_ccm_model.ATTRIBUTE.version = 1;
dbsks_xgraph_ccm_model.ATTRIBUTE.root_vid = root_vid;
dbsks_xgraph_ccm_model.ATTRIBUTE.tol_near_zero = tol_near_zero;
dbsks_xgraph_ccm_model.ATTRIBUTE.edge_threshold = edge_threshold;
dbsks_xgraph_ccm_model.ATTRIBUTE.distance_threshold = distance_threshold;
dbsks_xgraph_ccm_model.ATTRIBUTE.ccm_gamma = ocm_gamma;
dbsks_xgraph_ccm_model.ATTRIBUTE.ccm_lambda = ocm_lambda;
dbsks_xgraph_ccm_model.ATTRIBUTE.nbins_0topi = nbins_0topi;


% distribution for each fragment
dbsks_xgraph_ccm_model.ATTRIBUTE.dist_type = 'Weibull';
dbsks_xgraph_ccm_model.dbsks_xfrag_ccm_model = [];

% write the fragments model
for i = 1 : num_contour_frags
  
  % add a new edge if necessary
  if (isempty(dbsks_xgraph_ccm_model.dbsks_xfrag_ccm_model) || ...
    dbsks_xgraph_ccm_model.dbsks_xfrag_ccm_model(end).ATTRIBUTE.edge_id ~= edge_id(i))
    dbsks_xgraph_ccm_model.dbsks_xfrag_ccm_model(end+1).ATTRIBUTE.edge_id = edge_id(i);
  end;
  
  wbl = [];
  wbl.ATTRIBUTE.lambda = wbl_lambda(i);
  wbl.ATTRIBUTE.k = wbl_k(i);
  wbl.ATTRIBUTE.avg_logl = wbl_avg_logl(i);
  
  
  % put the distribution at proper side
  if (side_id(i) == 'L')
    dbsks_xgraph_ccm_model.dbsks_xfrag_ccm_model(end).fg_dist(1).weibull = wbl;
    dbsks_xgraph_ccm_model.dbsks_xfrag_ccm_model(end).fg_dist(1).ATTRIBUTE.side_id = 0;
  else
    dbsks_xgraph_ccm_model.dbsks_xfrag_ccm_model(end).fg_dist(2).weibull = wbl;
    dbsks_xgraph_ccm_model.dbsks_xfrag_ccm_model(end).fg_dist(2).ATTRIBUTE.side_id = 1;
  end;
end;


xml_write(xml_file, dbsks_xgraph_ccm_model);


% plot the scales
