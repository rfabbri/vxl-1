numIM = 25;           % OK
load_pattern_12       % OK

%All the views that will be used in the clustering.
%The first view is where the initial seed/query curve is located.
all_views = 0:24;
numViews = size(all_views,2);
num_shared_edges = 3;
