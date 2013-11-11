show_point_selection = true;
nf_tst= 1000; %number of figure to show the selected correspondences  on top of data
nimg0 = '000';
nimg1 = '005';
data_dir = 'data/oxford/dunster/';



%p_id=1:8;
% Best point selection for 8pt alg
p_id = [58   9   2  19  62  83  95   7];
%p_id = [58   9   2  19];

% just make sure this box encompasses the true epipole
xmin = -80000;
xmax =  80000;
ymin = -30000;
ymax =  30000;
box = [xmin ymin; 
       xmax ymax];

y_invert = false;

im0 = imread([data_dir 'house.' nimg0 '.pgm']);
im1 = imread([data_dir 'house.' nimg1 '.pgm']);


im0_xmax = size(im0,2);
im0_ymax = size(im0,1);
im1_xmax = size(im1,2);
im1_ymax = size(im1,1);


% all Harris corner points
all_p0 = load([data_dir 'house.' nimg0 '.corners']);
all_p1 = load([data_dir 'house.' nimg1 '.corners']);

corr = load([data_dir 'house.nview-corners-Inf']);



[ap0,ap1] = ox_get_corresp_pts(corr,all_p0, all_p1, eval(nimg0),eval(nimg1));

%p_id = [182  148   35  202    1   91  241  169];
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
