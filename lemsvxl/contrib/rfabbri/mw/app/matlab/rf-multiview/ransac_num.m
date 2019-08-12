e = .50; % outlier probability              
p = .99; % probability we hit all inliers sample
s = 3;   % sample size
% number of runs:
N = log(1 - p)/log(1 - (1 - e)^s)
