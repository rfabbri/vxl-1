function [cs, d] = closest_points_conic_arcs(conics, end_pts,  sample_pts)
%
% Returns the closest points to the conic c.
%
% c: nx6 array, c(i,:) = coeffs of (x^2, xy, y^2, xw, yw, w^2) where w is the
% homogeneous factor
% end_pts: 2nx2: end points modeled by the conics, where 
%          end_pts(i:i+1,:) == (x_ini,y_ini ; x_end, y_end) 
% 
% cs: nx2 closest points
% d: 1-vector of distances of pts to c


cs = -ones(size(sample_pts));
d = zeros(size(sample_pts,1),1);
current_i = 1;
nsegs = size(conics,1);
for i=1:nsegs
  % find where endpoints are within sample_pts

  i_start = find(   sample_pts(:,1) == end_pts(2*i-1,1)...
                 & sample_pts(:,2) == end_pts(2*i-1,2));
  if isempty(i_start)
    error('endpoints not within sample points');
  end
  
  i_end = find(   sample_pts(:,1) == end_pts(2*i,1)...
                & sample_pts(:,2) == end_pts(2*i,2));
  if isempty(i_end)
    error('endpoints not within sample points');
  end
%  if (i_start > i_end)
%    error('endpoints reversed');
%  end

  i_start, i_end
  [d_i, cs_i] = p2conic_dist(sample_pts(i_start:i_end,:), conics(i,:));

  next_i = current_i + size(cs_i,1)-1;
  % append
  cs(current_i:next_i,:) = cs_i;
  d(current_i:next_i) = d_i;

  current_i = next_i+1;
end

% extrapolate from last conic fit
if (current_i < size(sample_pts,1))
  [d_i, cs_i] = p2conic_dist(sample_pts(current_i:end,:), conics(end,:));
  cs(current_i:end,:) = cs_i;
  d(current_i:end) = d_i;
end

