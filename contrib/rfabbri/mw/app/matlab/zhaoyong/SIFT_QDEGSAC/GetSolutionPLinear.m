function [P] = GetSolutionPLinear(eq,NullSpaceDim,enforce)
%
%
%  Computes solution for projection matrix for nullspace
%  dimension NullSpaceDim. Each solution is putted into P(1:3,1:3,i)
%

[uu,ss,vv]=svd(eq);
index = 1;

for i = 12-NullSpaceDim+1 : 12
	P(1:3,1:4,index)=reshape(vv(:,i),4,3)';
	index = index+1;
end
	

