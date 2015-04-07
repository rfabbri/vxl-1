function [eqns] = GetLinearEquationsQuadric(m1,m2)
%
%  returns the linear equations eq for the projection matrix computation
%  following from the correspondences m2D, m3D in Euclidian coords
% call:
%    eq = GetLinearEquationsQuadric(m1, m2)

n = size(m1,2); 

% write down all equations
eqns = [m1(1,:)'.^2,  m1(2,:)'.^2, m1(3,:)'.^2,  (m1(1,:).*m1(2,:))', (m1(1,:).*m1(3,:))', (m1(2,:).*m1(3,:))', m1(1,:)', m1(2,:)', m1(3,:)', ones(size(m1,2),1)];
    