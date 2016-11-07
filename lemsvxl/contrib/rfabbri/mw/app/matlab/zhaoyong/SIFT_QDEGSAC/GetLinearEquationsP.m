function [eqns] = GetLinearEquationsP(m2D, m3D)
%
%  returns the linear equations eq for the projection matrix computation
%  following from the correspondences m2D, m3D in Euclidian coords
% call:
%    eq = GetLinearEquationsF(m2D, m3D)

n = size(m2D,2); 

% write down all equations
eqns = [zeros(n,4), -m3D', -ones(n,1), (m2D(2,:).*m3D(1,:))', (m2D(2,:).*m3D(2,:))', (m2D(2,:).*m3D(3,:))', m2D(2,:)';
	    m3D', ones(n,1), zeros(n,4), (-m2D(1,:).*m3D(1,:))', (-m2D(1,:).*m3D(2,:))',(-m2D(1,:).*m3D(3,:))', -m2D(1,:)'];
% 	 	(-m2D(2,:).*m3D(1,:))', (-m2D(2,:).*m3D(2,:))',(-m2D(2,:).*m3D(3,:))' -m2D(2,:)', ...
% 		(m2D(1,:).*m3D(1,:))', (m2D(1,:).*m3D(2,:))', (m2D(1,:).*m3D(3,:))', m2D(1,:)', zeros(n,4)];