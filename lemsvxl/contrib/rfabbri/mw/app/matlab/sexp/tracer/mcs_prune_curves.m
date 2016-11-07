function [newcrvs,newcosts] = mcs_prune_curves(crvs, costs, tau_t)
% Typical use:
% - load the file supports by doing :
%
%   load supports
% 
% - load all curves using read_curve_sketch
% - call this function
% - rename the output suitabily
% - ploat_all_curves.

if length(costs) ~= length(crvs)
  error('lengths don''t match');
end

newcrvs  = cell(0,0);
newcosts = [];

for i=1:length(crvs)
  if costs(i) >= tau_t
    newcrvs{end+1} = crvs{i};
    newcosts(end+1) = costs(i);
  end
end
