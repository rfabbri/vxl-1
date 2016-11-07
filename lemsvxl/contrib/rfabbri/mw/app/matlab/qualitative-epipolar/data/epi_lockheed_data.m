% Data from lockheed with horizon

nf_tst=1000; %number of figure to show the selected correspondences  on top of data
data_dir = 'data/infinity/';
if ~exist('show_point_selection')
  show_point_selection = true;
end

%1 2 3 4 6 7 9 11

% indices into ap0
p_id = [
%1    %1
%6 %2
%38 %3 
%10 %4 
%42 %5xx
%3  %6
%58%7
34    %8
%16   %9
%39   %10
%17   %11
%8   %12
%59  %13
%48  %14
%15  %15
%--------------
18
47
%41
14
7
%22
60
16
];
%18  47  41  14   7  22  60  16


% Good 8-tuplet for 8 pt alg:
%p_id = [ 9  10  25  17  37  38  22  36 ];


% just make sure this box encompasses the true epipole
%xmin = -6000;
%xmax = 9000;
%ymin = -5000;
%ymax = 5000;
xmin = -2500;
xmax = 2500;
ymin = -2500;
ymax = 2500;
box = [xmin ymin; 
       xmax ymax];

y_invert = false;

clear cpstruct;
load([data_dir 'uvs040203-002-77-frame1_63-corresp.mat']);
[ap0,ap1] = cpstruct2pairs(cpstruct);

im0=rgb2gray(imread([data_dir 'uvs040203-002-77-frame1-zoomed.jpg']));
im1=rgb2gray(imread([data_dir 'uvs040203-002-77-frame63-zoomed.jpg']));

im0_xmax = size(im0,2);
im0_ymax = size(im0,1);
im1_xmax = size(im1,2);
im1_ymax = size(im1,1);


pts0 = ap0(p_id,:);
pts1 = ap1(p_id,:);

if (show_point_selection)
  figure(nf_tst);  
  clf;
  imshow(im0,[]);
  hold on;
  cplot2(ap0,'.');
  cplot2(pts0,'rp');
  cplot2(pts0,'ro');


  figure(nf_tst+1);  
  clf;
  imshow(im1,[]);
  hold on;
  cplot2(ap1,'.');
  cplot2(pts1,'rp');
  cplot2(pts1,'ro');
end
