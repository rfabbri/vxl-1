function [F] = GetSolutionFLinear(eq,NullSpaceDim,enforceRankTwo)
%
%
%  Computes  eight point solution for fundamental matrix for Nullspace
%  dimension NullSpaceDim. Each solution is putted into F(1:3,1:3,i)
%

if (nargin < 3)
	enforceRankTwo = 0;
end

[uu,ss,vv]=svd(eq);
index = 1;

if enforceRankTwo == 1
	for i = 9-NullSpaceDim+1 : 9
		[u,s,v] = svd(reshape(vv(:,i),3,3)');
		if rank(s) > 0
			% enforce rank 2 for each F spanning the nullspace
			F(1:3,1:3,index)=u*diag([s(1,1) s(2,2) 0])*v';
			index = index+1;
		end
	end
else
	for i = 9-NullSpaceDim+1 : 9
		% enforce rank 2 for each F spanning the nullspace
		F(1:3,1:3,index)=reshape(vv(:,i),3,3)';
		index = index+1;
	end
end
