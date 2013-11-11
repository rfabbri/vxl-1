% 
% Tests the Euclidean representation by estimating it from known epipoles but
% unknown 1D homography, and measuring epipolar error. It also tests the
% conversion between Euclidean and Fundamental Matrix representations of
% epipolar geometry.
%
% Mon Oct  8 12:35:58 EDT 2007


% epi = epi_initialize_from_f(f);

%synth_point_data;
%synth_point_data_90deg;

% Lets get the ground-truth epipoles from the synthetic data


r0 = max(norm(e0),1);
r1 = max(norm(e1),1);
epi_s = struct('e',[e0; e1],'r',[r0,r1],'h',zeros(2,2),'refp',[0 0; 0 0]);

% Estimate homography for a subset

[h,res] = epi_estimate_h_points(epi_s,pts0,pts1);
epi_s.h = h;

% Show geometric error with respect to subset used to compute

d = epi_geometric_error(epi_s,pts0,pts1);
d = sqrt(d)/size(pts0,1)

% Show geometric error with respect to all points
%d = epi_geometric_error(epi,sp0,sp1);
%d = sqrt(d)/size(pts0,1);

% TODO: rotate 90deg, shold work.


