function [d,X] = d_pt_circle3(p,r,c,n)
% distance from point to 3D circle
% All vector parameters are row vectors
% r - radius
% c - center
% n - normal to circle plane
% p - 3D point


% projection of p into plane
q = p - (n*(p-c)')*n;

% closest point in circle
X = c + r*(q-c)/norm(q-c);

% distance to closest point
d = norm(X-p);
