%
% pts0 and pts1 NOT in homg form.
%
function [d,vv] = epi_geometric_error_f (fm, pts0, pts1)

d = 0;
vv = zeros(size(pts0,1),1);
for i=1:size(pts0,1)
  % get epipolar line of pts0(i,:) in image 1

  p0_homg =[pts0(i,:), 1]'; 
  l1 = fm*p0_homg;

  p1_homg =[pts1(i,:), 1]'; 
  l0 = p1_homg'*fm;
  l0 = l0';

  d0 = perp_dist_squared_homg(p0_homg,l0);
  d1 = perp_dist_squared_homg(p1_homg,l1);

  d = d + d0 + d1;
  vv(i) = d0 + d1;
end

vv = sqrt(vv/2); % rms
