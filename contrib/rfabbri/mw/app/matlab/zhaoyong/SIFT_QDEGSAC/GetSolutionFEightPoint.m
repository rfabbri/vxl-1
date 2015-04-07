function [F] = GetSolutionFEightPoint(eq)
%
%
%  Computes  eight point solution for fundamental matrix
%

[u,s,v]=svd(eq);
F(1:3,1:3,1)=reshape(v(:,9),3,3)';