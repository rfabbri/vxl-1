% To be called after running epi.m

% for each polygon 
%   - get cost
%         - min of cost for two sheet combination
%   - store cost in a vector
%   - sort vector
%   - display highest ranking polygon
%   - display other polygons with color proportional to cost


plot_costs = true;
n_fig_cost = 9;
n_fig = n_fig_cost;
show_centroids = false;

epi_polycost_f;

% -- Display results --
 plot_polys;
% compare_errors;
