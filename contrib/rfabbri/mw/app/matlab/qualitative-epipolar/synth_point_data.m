show_synth_point_selection = true;
nf_tst=1000; %number of figure to show the selected correspondences  on top of data

if ~exist('sig')
  sig = 0;
end

if ~exist('angles_s')
  angles_s = '30,60'
end

if ~exist('rotate90')
  rotate90 = false;
end

% calls my c++ routines to provide synthetic data
% yaw_correction = 13*pi/180;
%yaw_correction = -14*pi/180;
yaw_correction = 0;
[ap0,ap1,sfm]=synthetic_data(angles_s,yaw_correction);

if ~exist('dont_set_p_id')
  p_id = [
  967     % 1
  1053    % 2
  371     % 3
  27      % 4
  755     % 5 x
  341     % 6
  656     % 7
  111     % 8
  768     % 9
  828     % 10
  85      % 11
  906     % 12
  1085    % 13
  151     % 14
  708     % 15
  ];

  % Crashes gpc
  %p_id = [315   907   827    74   476    84   206];
  %p_id = randperm(size(ap0,1));
  %p_id = p_id(1:8);

  %p_id = [62         369         233         436         498         137 1035 478];
  %p_id = [62         369         233         436         498         137 1035];

  % Generates a polygon that is correctly ranked first by centroids, but the wrong sheet is
  % being picked.
  %p_id = [460        1106         106         553         505         511         962];

  % Polygon ranking totally screws up for these - best polygon not in top 15!
  % This is _without_ 8 points.
  %  p_id = [ 801 332 748 540   269 951 824 27];
  p_id = [
  801
  332
  748
  540   
  269
  951
  824
  27
  115
  656     
  111     
  85      
  1085    
  906     
  768     
  ];
end

% perturb
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

im0_xmax = 500;
im0_ymax = 400;
im1_xmax = 500;
im1_ymax = 400;

% Ground-truth epipoles from camera matrices

a = null(sfm);
e0 = [a(1)/a(3) a(2)/a(3)];
a = null(sfm');
e1 = [a(1)/a(3) a(2)/a(3)];

% Rotate

%myang = 3.2064;
%myrotm = [cos(myang), - sin(myang)
%          sin(myang), cos(myang)];
%pts0 = pts0*myrotm';
%pts1 = pts1*myrotm';
%e0 = e0*myrotm';
%e1 = e1*myrotm';

if rotate90
  rotate_data;
end


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


[cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles_35(e0',e1',pts0,pts1);
cost1=mean(vv);
[cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles(e0',e1',pts0,pts1);
cost2=mean(vv);
disp([cost1 cost2])
