clear all;
%close all;

read_curve_sketch4;

[dmy,R] = size(recs);

%STRAND 1
% %10
% queryPoint = [2.134 3.335 3.579];
% queryPoint = [2.036 3.203 3.53];
% queryPoint = [2.158 3.024 3.602];

% %6
% queryPoint = [2.328 3.213 3.711];

% %7
% queryPoint = [2.196 3.021 3.628];
% queryPoint = [2.334 3.206 3.709];

% %8
% queryPoint = [2.333 3.208 3.713];

% %STRAND 2
% %6
% queryPoint = [2.65 3.213 3.935];
% 
% %7
% queryPoint = [2.661 3.421 3.95];
% 
% %8
% queryPoint = [2.648 3.388 3.946];

% %STRAND 3
% %10
% queryPoint = [0.4183 2.249 6.323];
% 
% %13
% queryPoint = [0.6455 2.25 6.367];
% 
% %6
% queryPoint = [0.2813 2.258 6.207];
% queryPoint = [0.9808 2.233 6.262];
% 
% %7
% queryPoint = [0.2384 2.261 6.171];
% queryPoint = [0.9669 2.226 6.238];
% 
% %8
% queryPoint = [0.5034 2.261 6.32];
% queryPoint = [0.9395 2.22 6.23];

% %STRAND 4
% %13
% queryPoint = [0.04589 4.01 5.46];
% 
% %6
% queryPoint = [0.03082 4.008 5.892];
% 
% %7
% queryPoint = [0.7561 4.026 5.09];
% 
% %8
% queryPoint = [0.008174 4.008 5.675];

queryPoint = [2.158 3.024 3.602];

maxDistSq = 10^10;
maxDistIND = 0;

for r=1:R
    
    curCurve = recs{1,r};
    firstPoint = curCurve(1,:);
    
    distSq = (firstPoint(1,1) - queryPoint(1,1))^2 + (firstPoint(1,2) - queryPoint(1,2))^2 + (firstPoint(1,3) - queryPoint(1,3))^2;
    
    if(maxDistSq > distSq)
        maxDistSq = distSq;
        maxDistIND = r;
    end
    
end

maxDistIND