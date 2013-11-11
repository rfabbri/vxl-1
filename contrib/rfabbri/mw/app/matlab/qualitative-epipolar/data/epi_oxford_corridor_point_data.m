show_point_selection = true;
nf_tst= 1000; %number of figure to show the selected correspondences  on top of data
nimg0 = '000';
nimg1 = '003';
data_dir = 'data/oxford/corridor/';

if ~exist('sig')
  sig = 0;
end

% indices into all_p0
p_id = [
%558     %1
468      %2
482     %3 xx polycost: nsheets > 2
%114     %4 xx segfaults episearch
%530     %5
%264     %6
%206     %7
330     %8
%358     %9
495     %10
128     %11
%275     %12
%239      %13 xx
%615     %14
269     %15
];

% % indices into all_p0
% p_id = [
% 558     %1
% 468      %2
% 482     %3 xx polycost: nsheets > 2
% %114     %4 xx segfaults episearch
% %530     %5
% 264     %6
% 206     %7
% 330     %8
% 358     %9
% 495     %10
% 128     %11
% %275     %12
% 239      %13 xx
% %615     %14
% 269     %15
% ];


% just make sure this box encompasses the true epipole
xmin = -1000;
xmax = 1000;
ymin = -1000;
ymax = 1000;
box = [xmin ymin; 
       xmax ymax];

y_invert = false;

im0 = imread([data_dir 'bt.' nimg0 '.pgm']);
im1 = imread([data_dir 'bt.' nimg1 '.pgm']);

im0_xmax = size(im0,2);
im0_ymax = size(im0,1);
im1_xmax = size(im1,2);
im1_ymax = size(im1,1);

% all Harris corner points
all_p0 = load([data_dir 'bt.' nimg0 '.corners']);
all_p1 = load([data_dir 'bt.' nimg1 '.corners']);

corr = load([data_dir 'bt.nview-corners-Inf']);



pts0 = all_p0(p_id,:);
pts1 = zeros(size(pts0));

% search for corresponding selected points 


found_inf = false;
for ic=1:size(corr,1)
  for ip=1:max(size(p_id))
    if corr(ic,eval(nimg0)+1)+1 == p_id(ip)

%      disp(['Found correspondence for' num2str(p_id (ip)) '.']);

      if corr(ic,eval(nimg1)+1) ~= Inf

%        disp(['Found a correspondence for ' num2str(p_id (ip)) ' in image 2.']);
        pts1(ip,:) = all_p1(corr(ic,eval(nimg1)+1)+1,:);

      else

        disp(['NO correspondence for ' num2str(p_id (ip)) ' in image 2.']);
        pts1(ip,:) = [Inf Inf];
        found_inf=true;
        error('Not found a corresp');

      end

    end
  end
end


[ap0,ap1] = ox_get_corresp_pts(corr,all_p0, all_p1, eval(nimg0),eval(nimg1));

% Ground-truth epipoles from all points
% Options:
%   - use 8 point DLT using all good points
%   or
%   - use RANSAC over this set
%
% I suspect that, even though the matches are correct, RANSAC will give a better
% estimation due to noise outliers, degeneracy outliers, and numerical
% instability outliers.

% perturb
ap0 = ap0 + sig*randn(size(ap0));
ap1 = ap1 + sig*randn(size(ap1));

%fm_gt = fmatrix(ap0,ap1,'vpglransac');
fm_gt = fmatrix(ap0,ap1,'vpgl8pt');
%fm_gt = fmatrix(ap0,ap1,'mvl8pt');
fm_fail = eye(3);
fm_fail(3,3)=0;
if norm(fm_gt-fm_fail) > 0.0001
  a = null(fm_gt);
  e0 = [a(1)/a(3) a(2)/a(3)];
  a = null(fm_gt');
  e1 = [a(1)/a(3) a(2)/a(3)];
end



%p_id = [182  148   35  202    1   91  241  169];
%p_id = [182  148   35  202    1   91  241];
% This is the best tuple for the 8point algthm, over 1117 ransac runs :
p_id = [22    29   102   259   180    33   247   109];
%pts0 = ap0(p_id,:);
%pts1 = ap1(p_id,:);

pts0 =[160.34       139.45
       284.42       151.04
       111.03       262.85
       150.14       399.83
        83.88       229.44
       236.89       158.27
       232.42       235.19
       314.95       274.55];

pts1 = [ 176.92       134.32
         315.75       152.55
         116.16       277.57
         161.89       441.93
         85.138       237.88
         262.67       158.98
         257.03       239.48
         348.9        284.41];


if show_point_selection
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
  if(~found_inf) 
    cplot2(pts1,'rp');
    cplot2(pts1,'ro');
  end
end
