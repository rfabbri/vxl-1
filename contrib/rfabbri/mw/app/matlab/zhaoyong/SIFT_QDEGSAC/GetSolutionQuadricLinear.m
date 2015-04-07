function [Q] = GetSolutionQuadricLinear(eq,NullSpaceDim,enforce)
%
%
%  Computes solution for projection matrix for nullspace
%  dimension NullSpaceDim. Each solution is putted into P(1:3,1:3,i)
%

[uu,ss,vv]=svd(eq);
index = 1;

for i = 10-NullSpaceDim+1 : 10
	q=vv(:,i);
	Q(1:4,1:4,index)= [q(1)    q(4)/2   q(5)/2   q(7)/2
                       q(4)/2  q(2)     q(6)/2   q(8)/2
                       q(5)/2  q(6)/2   q(3)     q(9)/2
                       q(7)/2  q(8)/2   q(9)/2   q(10)];
    index = index+1;
end
	

