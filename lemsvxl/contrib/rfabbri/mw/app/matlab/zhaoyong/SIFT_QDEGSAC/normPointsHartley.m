function [KNormal,m1,m2]=normPointsHartley(tm1,tm2)
%
% Hartley point normalization
%
%

minx = min([tm1(1,:), tm2(1,:)]);
maxx = max([tm1(1,:), tm2(1,:)]);
miny = min([tm1(2,:), tm2(2,:)]);
maxy = max([tm1(2,:), tm2(2,:)]);

% compute normalization 
if maxx-minx > maxy-miny
   delta = maxx-minx;
   minn = minx;
else
   delta = maxy-miny;
   minn = miny;
end   

KNormal(1,1) = 2/delta;
KNormal(2,2) = 2/delta;
KNormal(1,3) = -2*minn/delta - 1;
KNormal(2,3) = -2*minn/delta - 1;
KNormal(3,3) = 1; 

m1 = KNormal(1:2,1:3)*[tm1; ones(1,size(tm1,2))];
m2 = KNormal(1:2,1:3)*[tm2; ones(1,size(tm2,2))];