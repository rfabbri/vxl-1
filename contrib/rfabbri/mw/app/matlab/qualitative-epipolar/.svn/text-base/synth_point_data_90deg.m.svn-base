show_synth_point_selection = true;
nf_tst=1000; %number of figure to show the selected correspondences  on top of data

if ~exist('angles_s')
  angles_s = '30,60'
end

% calls my c++ routines to provide synthetic data
[ap0,ap1,sfm]=synthetic_data(angles_s);


p_id = [
967     % 1
1053    % 2
371     % 3
27      % 4
755     % 5
341     % 6
656     % 7
111     % 8
%768     % 9
%828     % 10
%85      % 11
%906     % 12
%1085    % 13
%151     % 14
%708     % 15
];

% perturb
sig = 1;
ap0 = ap0 + sig*randn(size(ap0));
ap1 = ap1 + sig*randn(size(ap1));

%p_id = 1:size(ap0,1);

pts0 = ap0(p_id,:);
pts1 = ap1(p_id,:);

% just make sure this box encompasses the true epipole
xmin = -20000;
xmax = 20000;
ymin = -4000;
ymax = 4000;
box  = [xmin ymin; 
       xmax ymax];

% rotate 90 degree
pts0_r = zeros(size(pts0));
pts0_r(:,1) = -pts0(:,2);
pts0_r(:,2) = pts0(:,1);
pts0 = pts0_r;

pts1_r = zeros(size(pts1));
pts1_r(:,1) = -pts1(:,2);
pts1_r(:,2) = pts1(:,1);
pts1 = pts1_r;

ap0_r = zeros(size(ap0));
ap0_r(:,1) = -ap0(:,2);
ap0_r(:,2) = ap0(:,1);
ap0 = ap0_r;

ap1_r = zeros(size(ap1));
ap1_r(:,1) = -ap1(:,2);
ap1_r(:,2) = ap1(:,1);
ap1 = ap1_r;

box_r = box;
box_r(:,1) = -box(:,2);
box_r(:,2) = box(:,1);
box = box_r;

% perturb

%pts0 = pts0 + 0.5*randn(size(pts0));
%pts1 = pts1 + 0.5*randn(size(pts1));

% Show two views with the selected points superposed to the original points

if (show_synth_point_selection)
  figure(nf_tst);
  clf;
  cplot2(ap0,'.');
  axis equal;
  hold on;
  cplot2(pts0,'rp');
  cplot2(pts0,'ro');

  figure(nf_tst+1);
  clf;
  cplot2(ap1,'.');
  axis equal;
  hold on;
  cplot2(pts1,'rp');
  cplot2(pts1,'ro');
end

y_invert = false;


% Ground-truth epipoles from camera matrices

a = null(sfm);
e0 = [a(1)/a(3) a(2)/a(3)];
a = null(sfm');
e1 = [a(1)/a(3) a(2)/a(3)];

% Rotate epipoles
e0_r = zeros(size(e0));
e0_r(:,1) = -e0(:,2);
e0_r(:,2) = e0(:,1);
e0 = e0_r;

% Rotate epipoles
e1_r = zeros(size(e1));
e1_r(:,1) = -e1(:,2);
e1_r(:,2) = e1(:,1);
e1 = e1_r;
