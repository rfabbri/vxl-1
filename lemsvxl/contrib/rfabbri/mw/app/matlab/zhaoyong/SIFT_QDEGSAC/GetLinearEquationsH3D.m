function [eqns] = GetLinearEquationsH3D(m1, m2)
%
%  returns the linear equations eq for the projection matrix computation
%  following from the correspondences m2D, m3D in Euclidian coords
% call:
%    eq = GetLinearEquationsF(m2D, m3D)

n = size(m1,2); 

% write down all equations
eqns = [m1', ones(size(m1,2),1) ,  zeros(size(m1,2),size(m1,1)+1), zeros(size(m1,2),size(m1,1)+1), -(m2(1,:).*m1(1,:))', -(m2(1,:).*m1(2,:))', -(m2(1,:).*m1(3,:))', -(m2(1,:).*ones(1,size(m1,2)))';
	zeros(size(m1,2),size(m1,1)+1), m1',ones(size(m1,2),1) , zeros(size(m1,2),size(m1,1)+1), -(m2(2,:).*m1(1,:))', -(m2(2,:).*m1(2,:))', -(m2(2,:).*m1(3,:))', -(m2(2,:).*ones(1,size(m1,2)))';
	zeros(size(m1,2),size(m1,1)+1),zeros(size(m1,2),size(m1,1)+1), m1', ones(size(m1,2),1) , -(m2(3,:).*m1(1,:))', -(m2(3,:).*m1(2,:))', -(m2(3,:).*m1(3,:))', -(m2(3,:).*ones(1,size(m1,2)))'];
	