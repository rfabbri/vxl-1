% Input-specific -------------------------------------------------------------

% Input number of views in numIM
numIM = ...;           % OK
%data_id = 12;

% load_pattern_12       already automatically defined by matlab 
% For a different data_id / dataset you need to change load_pattern and load_pattern_*

%All the views that will be used in the clustering.
%The first view is where the initial seed/query curve is located.

%Input numIM-1
all_views = 0:numIM-1;
numViews = size(all_views,2);

% Drawing-specific -------------------------------------------------------------

all_clusters = cell(0,0);
num_shared_edges = 3;
junction_dist_threshold = 0.01;
branch_length_threshold = 0.01;
sample_merge_threshold = 0.005;
sample_break_threshold = 3*sample_merge_threshold;

% %DEFAULTS
% all_clusters = cell(0,0);
% num_shared_edges = 3;
% junction_dist_threshold = 0.03;
% branch_length_threshold = 0.1;
% sample_dist_threshold = 0.01;

branch_size_threshold = 3;