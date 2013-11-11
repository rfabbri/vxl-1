% This script load and plot the distribution of OCM cost of fragments in a
% xshock graph model of giraffes
% (c) Nhon Trinh
% Date: Nov 15, 2008

clear all;
close all;

%% Compute ccm distribution for object classes from collected ccm costs

%% Input data

% % applelogos
% input_xgraph_dirname = 'applelogos-xgraph'
% pos_ccm_cost_fname = 'applelogos_prototype1-pos_ccm_cost-2009jul13-b.xml';
% neg_ccm_cost_fname = 'applelogos_prototype1-neg_ccm_cost-2009jul15-long-range-interrupted.xml';

% % mugs
% input_xgraph_dirname = 'mugs-xgraph';
% pos_ccm_cost_fname = 'mugs-pos_ccm_cost-2009jul23-chamfer_4.xml';
% neg_ccm_cost_fname = 'mugs-neg_ccm_cost-2009jul23-chamfer_4-interrupted.xml';
% bfrags_to_ignore = '15-L,15-R,19-L,19-R,30-L,30-R,37-L,37-R,40-L,40-R';
% output_folder = 'D:\vision\projects\symseg\xshock';
% output_fname = 'mugs-xgraph_ccm_model-v2-2009aug26-chamfer_4.xml';
% 
% % giraffes
% input_xgraph_dirname = 'giraffes-xgraph';
% pos_ccm_cost_fname = 'giraffes-pos_ccm_cost-2009aug31-chamfer_4.xml';
% neg_ccm_cost_fname = 'giraffes-neg_ccm_cost-2009aug31-chamfer_4.xml';
% bfrags_to_ignore = '';
% output_folder = 'D:\vision\projects\symseg\xshock';
% output_fname = 'giraffes-xgraph_ccm_model-v2-2009aug31-chamfer_4.xml';

% % bottles
% input_xgraph_dirname = 'bottles-xgraph';
% pos_ccm_cost_fname = 'bottles-pos_ccm_cost-2009sep14-chamfer_4.xml';
% neg_ccm_cost_fname = 'bottles-neg_ccm_cost-2009sep14-chamfer_4.xml';
% bfrags_to_ignore = '';
% output_folder = 'D:\vision\projects\symseg\xshock';
% output_fname = 'bottles-xgraph_ccm_model-v2-2010jan22-chamfer_4.xml';


% swans
%xgraph_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-graph\swans-xgraph';
%training_folder = 'V:\projects\kimia\shockshape\symseg\results\ETHZ-dataset\xshock-train\swans';
%pos_ccm_cost_fname = 'swans_prototype_0-xshock_ccm_model-2010apr08-pos_data.xml';
%neg_ccm_cost_fname = 'swans_prototype_0-xshock_ccm_model-2010apr08-neg_data.xml';
%bfrags_to_ignore = {'7-L', '7-R', '19-L', '19-R', '24-L', '24-R', '13-R', '10-R'} ;
%output_folder = 'D:\vision\projects\symseg\xshock';
%output_fname = 'swans_prototype_0-xshock_ccm_model-2010apr08.xml';

% applelogos - firat
training_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/applelogos';
pos_ccm_cost_fname = 'applelogos-pos-ccm-cost-rv3-07262010.xml';
neg_ccm_cost_fname = 'applelogos-neg-ccm-cost-rv3-07262010.xml';
bfrags_to_ignore = {'9-L','9-R','28-L','28-R','34-L','34-R'};
output_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/applelogos';
output_fname = 'applelogos-ccm-model-rv3-07292010.xml';



%% Process positive cost

% read positive data file
pos_ccm_cost_file = fullfile(training_folder, pos_ccm_cost_fname);
[pos_tree, pos_treename] = xml_read(pos_ccm_cost_file);
pos_data = sscanf(pos_tree.data, '%f ');

% read negative data file
neg_ccm_cost_file = fullfile(training_folder, neg_ccm_cost_fname);
[neg_tree, neg_treename] = xml_read(neg_ccm_cost_file);
neg_data = sscanf(neg_tree.data, '%f ');


%% reformat the data in a table format
pos_data = reshape(pos_data, pos_tree.header.num_bnd_frags, []);
pos_data = pos_data';

neg_data = reshape(neg_data, neg_tree.header.num_bnd_frags, []);
neg_data = neg_data';

%% parse common data among the two trees
% (We trust the positive data file for now)
num_bfrags = pos_tree.header.num_bnd_frags;
labels = pos_tree.header.bnd_frag_labels.label;

%% Range of values to compute
xmin = 0;
xmax = 1;
dx = 0.01;
x = [dx/2 : dx : xmax]';

%% Fit non-parametric distribution to positive data
pos_pdf = [];
pos_avg_logl = zeros(num_bfrags, 1);
for i = 1 : num_bfrags
  r = pos_data(:,i);
  r = max(r, 1e-5 * ones(size(r)));
  r = min(r, (1-1e-5) * ones(size(r)));
  
  if (length(unique(r)) == 1)
      pos_pdf(:, i) = ones(size(x));
      pos_avg_logl(i) = 0;
    continue;
  end;
  
  % non-parametric estimation
  fx = ksdensity(r, x, 'kernel', 'normal', 'npoints', 100, 'width', 0.05, 'function', 'pdf');
  pos_pdf(:, i) = fx;
end;


%% Fit non-parametric distribution to negative data
fit_uniform = 0;

if ~fit_uniform
    neg_pdf = [];
    neg_avg_logl = zeros(num_bfrags, 1);
    for i = 1 : num_bfrags
      r = neg_data(:,i);
      
      % cap top and bottom of data
      r = max(r, ones(size(r)) * 1e-5);
      r = min(r, ones(size(r)) * (1-1e-5));
      
      if (length(unique(r)) == 1)
          neg_pdf(:, i) = ones(size(x));
        continue;
      end;

      fx = ksdensity(r, x, 'kernel', 'normal', 'npoints', 100, 'width', 0.05, 'function', 'pdf');
      neg_pdf(:, i) = fx;
    end;
else
    neg_pdf = [];
    neg_avg_logl = zeros(num_bfrags, 1);
    for i = 1 : num_bfrags  
      neg_pdf(:, i) = ones(size(x))/dx/length(x);  
    end;
end

%% Compute average sum of pdf for the postive costs
loglike_pos_data = [];
for i = 1 : num_bfrags
  r = pos_data(:,i);
  r = max(r, 1e-5 * ones(size(r)));
  r = min(r, (1-1e-5) * ones(size(r)));
  
  % loglike
  prob_pos = interp1(x, pos_pdf(:, i), r, 'linear', 'extrap');
  prob_neg = interp1(x, neg_pdf(:, i), r, 'linear', 'extrap');
  
  % compute cost for the pos data
  loglike_r = log(prob_pos) - log(prob_neg);
  loglike_pos_data(:, i) = loglike_r;
end;




%% Plot fitted distribution for positive data

h1 = figure(1);
for i = 1 : num_bfrags
  subplot(ceil(num_bfrags / 4), 4, i, 'replace');
  
  % plot the histogram
  hist(pos_data(:, i), 0.05 : 0.05 : 0.95);

  % plot the fitted Weibull distribution
  y = pos_pdf(:, i);
  
  % need to scale the distribution to fit the histogram
  scale_factor = length(pos_data(:, i)) * 0.05;
  y = y * scale_factor;
  hold on;
  plot(x, y, 'r-', 'linewidth', 2);
  hold off;
  
  grid off;
  title([labels(i), 'FG loglike= ', num2str(pos_avg_logl(i))]);

  axis([0, 1, 0, 1000]); %max max_count]);
  h = findobj(gca,'Type','patch');
  set(h,'FaceColor','b','EdgeColor','w')
end;



%% Plot fitted distribution to negative data

h2 = figure(2);
for i = 1 : num_bfrags
  subplot(ceil(num_bfrags / 4), 4, i, 'replace');
  
  % plot the histogram
  hist(neg_data(:, i), 0.05 : 0.05 : 0.95);

  % plot the fitted Weibull distribution
  y = neg_pdf(:, i)';

  % need to scale the distribution to fit the histogram
  scale_factor = length(neg_data(:, i)) * 0.05;
  y = y * scale_factor;
  hold on;
  plot(x, y, 'r-', 'linewidth', 2);
  hold off;
  
  grid off;
  title([labels(i), 'BG loglike= ', num2str(neg_avg_logl(i))]);

  cur_axis = axis;
  axis([0, 1, cur_axis(3), cur_axis(4)]); %max max_count]);
  h = findobj(gca,'Type','patch');
  set(h,'FaceColor','b','EdgeColor','w')
end;


%% Combine positive and negative

h3 = figure(3);
num_plots = num_bfrags + 1;
for i = 1 : num_bfrags
  subplot(ceil(num_plots / 4), 4, i, 'replace');
  
  % plot the log-likelihood of the fitted Weibull distributions
  pos_y = log(pos_pdf(:, i));
  neg_y = log(neg_pdf(:, i));
  
  hold on;
  plot(x, pos_y, 'r-', 'linewidth', 2);
  plot(x, neg_y, 'b-', 'linewidth', 2);
  plot(x, pos_y - neg_y, 'g-', 'linewidth', 2);
  hold off;
  
  axis([0, 1, -10, 10]);
  grid off;
  title([labels(i), 'Log like ratio']);
  h = findobj(gca,'Type','patch');
  set(h,'FaceColor','b','EdgeColor','w')
end;

% plot the legend
subplot(ceil(num_plots / 4), 4, num_bfrags+1, 'replace');
hold on;
plot(0, 0, 'r-', 'linewidth', 2);
plot(0, 0, 'b-', 'linewidth', 2);
plot(0, 0, 'g-', 'linewidth', 2);
hold off;
legend('pos', 'neg', 'pos-neg');


%return;


%ignored_bnd_frags={'7-L','7-R','10-R','13-R','19-L','19-R','24-L'};
labels = pos_tree.header.bnd_frag_labels.label;
bnd_frag_mask = zeros(1, length(labels));
for i = 1 : length(bnd_frag_mask)
  label = labels{i};
  ignored = 0;
  for j = 1 : length(bfrags_to_ignore)
    if (strcmp(label, bfrags_to_ignore{j}))
      ignored = 1;
      break;
    end;
  end;
  bnd_frag_mask(i) = 1 - ignored;
end;

bnd_frag_mask
sum_loglike = loglike_pos_data * bnd_frag_mask';
figure (4); hist(sum_loglike);
title('Histogram of sum log likelihood');
fprintf(1, 'mean loglike = %g\n', mean(sum_loglike));




%% Print fitting results to an XML file ........................................

min_prob = 0.00001;

% sort the data
edge_id = zeros(length(labels), 1);
side_id = zeros(length(labels), 1);

for i = 1 : num_bfrags
  label = char(labels(i));
  % parse the label
  A = sscanf(label, '%d-%s');
  edge_id(i) = A(1);
  side_id(i) = char(A(2));
end;

%[edge_id, idx] = sortrows(edge_id);
%side_id = side_id(idx);

% Make sure the two cost files contain the same header info
ph = pos_tree.header;
nh = neg_tree.header;
header_equal = (ph.distance_threshold == nh.distance_threshold) && ...
             (ph.distance_tol_near_zero == nh.distance_tol_near_zero) && ...
             (ph.orient_threshold == nh.orient_threshold) && ...
             (ph.orient_tol_near_zero == nh.orient_tol_near_zero) && ...
             (ph.weight_chamfer == nh.weight_chamfer) && ...
             (ph.weight_edge_orient == nh.weight_edge_orient) && ...
             (ph.weight_contour_orient == nh.weight_contour_orient) && ...
             (ph.local_window_width == nh.local_window_width) && ...
             (ph.nbins_0topi == nh.nbins_0topi);

if (header_equal)
  fprintf(1, '\nHeader info the same? YES.\n');
else
  fprintf(1, '\nHeader info the same? NO.\n');
end;

header = pos_tree.header;

% form filename of ccm model
xml_file = fullfile(output_folder, output_fname);

tree = [];
tree.ATTRIBUTE.version = 2;

% data source
data_source.pos_ccm_cost_filename = pos_ccm_cost_fname;
data_source.neg_ccm_cost_filename = neg_ccm_cost_fname;
data_source.pos_training_images = header.list_pos_image;
data_source.neg_training_images = header.list_neg_image;
tree.data_source = data_source;

        

% ccm_params
fprintf(1, 'Output ccm follows header in positive cost file.\n');

ccm_params.distance_threshold = header.distance_threshold;
ccm_params.distance_tol_near_zero = header.distance_tol_near_zero;
ccm_params.orient_threshold = header.orient_threshold;
ccm_params.orient_tol_near_zero = header.orient_tol_near_zero;
ccm_params.weight_chamfer = header.weight_chamfer;
ccm_params.weight_edge_orient = header.weight_edge_orient;
ccm_params.weight_contour_orient = header.weight_contour_orient;
ccm_params.local_window_width = header.local_window_width;
ccm_params.nbins_0topi = header.nbins_0topi;
tree.ccm_params = ccm_params

% xgraph_info
xgraph_info.root_vid = header.root_vid;
xgraph_info.base_xgraph_size = header.base_xgraph_size;
xgraph_info.num_bfrags = header.num_bnd_frags;
tree.xgraph_info = xgraph_info;

% write the fragments model
for i = 1 : num_bfrags
  bfrag_cost_model.ATTRIBUTE.edge_id = edge_id(i);
  bfrag_cost_model.ATTRIBUTE.side_id = char(side_id(i));
  
  % foreground
  bfrag_cost_model.foreground.active_dist_type = 'NONPARAM';
  fg_hist.nbins = length(x);
  fg_hist.min = xmin;
  fg_hist.max = xmax;
  fg_hist.min_prob = min_prob; % arbitrary
  fg_hist.values = x';
  fg_hist.counts = pos_pdf(:,i)';
  bfrag_cost_model.foreground.bsta_histogram = fg_hist;
  
  % background
  bfrag_cost_model.background.active_dist_type = 'NONPARAM';
  bg_hist.nbins = length(x);
  bg_hist.min = xmin;
  bg_hist.max = xmax;
  bg_hist.min_prob = min_prob;
  bg_hist.values = x';
  bg_hist.counts = neg_pdf(:, i)';
  bfrag_cost_model.background.bsta_histogram = bg_hist;
  
  % add bfrag to overall list
  tree.list_bfrag_cost_models.bfrag_cost_model(i) = bfrag_cost_model;
end;

% user-override
% params manually inserted by user to override other model params
tree.user_override.list_bfrags_to_ignore=bfrags_to_ignore;

Pref=[]; Pref.CellItem = false;
xml_write(xml_file, tree, 'dbsks_xgraph_ccm_model', Pref);


% plot the scales
