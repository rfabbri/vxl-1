% Finds polygon sheet, within a polygon list structure, containing a given point.

% Output are -1 if not found.
function [ip_gt,is_gt] = find_poly(polys,p_in);


% For each polygon and each sheet
  % if contains epipole
    % set id
    % break;

ip_gt = -1;
is_gt = -1;
for ip=1:max(size(polys))
  for isheet = 1:max(size(polys{ip}))
    x = polys{ip}{isheet}(:,1);
    y = polys{ip}{isheet}(:,2);

    if inpolygon(p_in(1),p_in(2),x,y)
      ip_gt = ip; 
      is_gt = isheet;
      break;
    end
  end
end

