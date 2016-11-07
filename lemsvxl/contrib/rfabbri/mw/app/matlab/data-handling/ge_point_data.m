%npts = 11;
%pts0 = zeros(npts,2);
%pts1 = zeros(npts,2);

% GE data

ap0 = [
647,627         %1
96,268          %2
947,516         %3
605,275         %4
776,779         %5
989,155         %6
1244,837        %7
417,792         %8
755,443         %9
72,20           %10
108,22          %11
1077,774        %12
550,902         %13
1249,590        %14
535,574         %15
];

ap1 = [
651,562         %1
741,346         %2
955,534         %3
1023,378        %4
544,638         %5
1103,279        %6
744,699         %7
351,618         %8
850,466         %9
739,155         %10
1134,209        %11
761,649         %12
240,650         %13
1088,604        %14
599,521         %15
];

%p_id = 1:8;
% Good 8-tuplet for 8 pt alg:
p_id = [4     5     6     8    10    11    13    15];

pts0 = ap0(p_id,:);
pts1 = ap1(p_id,:);


% invert y

y_invert  = false;
img_max_y = 959;
%xxx pts0(:,2) = img_max_y-pts0(:,2);
%pts1(:,2) = img_max_y-pts1(:,2);

%xmin = -15000;
%xmax = 5000;
%ymin = -3000;
%ymax = 3000;
xmin = -20000;
xmax = 5000;
ymin = -4000;
ymax = 4000;
%xmin = -80000;
%xmax = 80000;
%ymin = -20000;
%ymax = 20000;
box = [xmin ymin; 
       xmax ymax];
im0_xmax = 1280;
im0_ymax = 960;
im1_xmax = 1280;
im1_ymax = 960;

% Epipoles from 8-point alg vpgl using y-coord inversion

%e0 = [2054 722.62];
%e1 = [2413.6 544.07];

% Epipoles from 8-point alg vpgl NOT using y-coord inversion

%e0 = [2066.5       234.07]
%e1 = [2370.8       417.72]

% Epipoles from normalized 7 point alg vpgl NOT using y-coord inversion

%e0 = [2193.6       316.01]
%e1 = [2524.3       370.99]

% Epipoles from normalized 7 point alg mvl NOT using y-coord inversion

%e0 = [2025.8       548.56];
%e1 = [2325.8       390.15];

% Epipoles from NON-normalized 7 point alg mvl NOT using y-coord inversion.
% Totally wrong!
%e0 = [1.2542e+05          432];
%e1 = [ 3.1759e+06   5.7132e+05];

% Since the x-coordinates are wrong, then the following might be happening:
% - arbitrary truncation to rank 2 by SVD is not invariant to inversion in y
% coordinates. This should not desirably affect the x coordinates, but it is
% possible.
% - vpgl is not preconditioning the points correctly, or this preconditioning is
% also arbitrary  and affects the x-coordinates of the points.

% Epipoles from 8-point alg mvl with preconditioning and rank 2 truncation
%e0 = [1996.6       208.54];
%e1 = [2340       437.44];

% Epipoles from normalized 8-point algthm from MVL PLUS Non-linear optimization

%e0 = [1996.6       208.54];
%e1 = [   2340       437.44];

% Epipoles from 8-point alg mvl without preconditioning and with rank 2
% truncation. THIS IS EXACTLY LIKE VPGL!! THUS VPGL IS NOT PRECONDITIINING
%
%e0 = [2066.5       234.07];
%e1 = [2370.8       417.72];


% Epipoles from camera matrices:

e0 = [-11628.8,-542.608];
e1 = [-3402.71,-465.874];


%xxx e0(1,2) = img_max_y - e0(1,2);
%e1(1,2) = img_max_y - e1(1,2);
