function [H] = GetSolutionH3DLinear(eq,NullSpaceDim,enforce)
%
%
%  Computes solution for projection matrix for nullspace
%  dimension NullSpaceDim. Each solution is putted into P(1:3,1:3,i)
%

[uu,ss,vv]=svd(eq);
index = 1;

for i = 16-NullSpaceDim+1 : 16
	H(1:4,1:4,index)=reshape(vv(:,i),4,4)';
	index = index+1;
end
	

