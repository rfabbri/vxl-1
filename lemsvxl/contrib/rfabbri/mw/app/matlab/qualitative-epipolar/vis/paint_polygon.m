% paints polygon containing an epipole



% Image 0:

% For each polygon and each sheet
  % if contains epipole
    % set id
    % break;

mycolor = [0.5 0.5 0.5];

[ip_gt,is_gt] = find_poly(polys0,e0_new);

found_p = (ip_gt ~= -1);

if (found_p)
  figure(n_fig);
  fill(polys0{ip_gt}{is_gt}(:,1),polys0{ip_gt}{is_gt}(:,2),mycolor);
else
  error('Point not found');
end
