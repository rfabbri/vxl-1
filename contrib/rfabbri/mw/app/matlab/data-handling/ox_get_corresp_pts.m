% returns corresponding points in two views, given the matrix with
% correspondence as specified in oxford data
% view0 and view 1 are 0-index into the desired views.
% all_p0, all_p1: all corner points  in the dataset, irrespective if they have a
% match
%
% ap0, ap1: all points in the dataset having match
% idx: index of the matching point onto the corr structure (ie, its row)
%
function [ap0,ap1, idx] = ox_get_corresp_pts(corr,all_p0, all_p1, view0,view1)

npts0 = size(all_p0,1);
npts1 = size(all_p1,1);
min_npts = min(npts0,npts1);
pts0 = zeros(min_npts,2);
pts1 = zeros(min_npts,2);
idx  = zeros(1,min_npts);

% search for corresponding selected points 


n_valid = 0;
found_inf = false;
for ic=1:size(corr,1)
  if corr(ic,view0+1) ~= Inf
    if corr(ic,view1+1) ~= Inf
      n_valid = n_valid + 1;
      pts0(n_valid,:) = all_p0(corr(ic,view0+1)+1,:);
      pts1(n_valid,:) = all_p1(corr(ic,view1+1)+1,:);
      idx(n_valid) = ic;
    end
  end
end

ap0 = pts0(1:n_valid,:);
ap1 = pts1(1:n_valid,:);
idx = idx(1:n_valid);
