function [eqns] = GetLinearEquationsF(m1, m2)
%
%  returns the linear equations eq for the fundamental matrix computation
%  following from the correspondences m1, m2
% call:
%    eq = GetLinearEquationsF(m1, m2)

n = size(m1,2); % number of potential matches

% write down all equations
eqns=[    m1(1,:).*m2(1,:); m1(2,:).*m2(1,:); m2(1,:); 
	m1(1,:).*m2(2,:); m1(2,:).*m2(2,:); m2(2,:);
	m1(1,:);          m1(2,:);         ones(1,n)]';
