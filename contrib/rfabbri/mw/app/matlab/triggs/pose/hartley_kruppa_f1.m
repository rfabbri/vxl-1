% try 2 sided for stability. Fix |F| vs |e|...

% [fs,cond] = hartley_kruppa_f(F,e,pp0,pp1)
% Given a fundamental matrix and one epipole derived from cameras
% with zero skew, equal aspect ratio, and known principal points 
% pp0,pp1, find the unknown focal lengths f0,f1 using Hartley's
% Kruppa equation based method.

function [fs,cond] = hartley_kruppa_f1(F10,e10,e01,pp0,pp1)
   if (nargin<4)
      if (nargin<3) pp0=[0;0;1]; end; 
      pp1=pp0; 
   end;
   me0 = mcross(e10);
   Fp0 = F10' * pp1;
   mep0 = me0 * pp0;
   FIF0 = F10' * diag([1,1,0]) * F10;
   FpF0 = Fp0 * Fp0';
   eIe0 = me0 * diag([1,1,0]) * me0';
   epe0 = mep0 * mep0';

   me1 = mcross(e01);
   Fp1 = F10 * pp0;
   mep1 = me1 * pp1;
   FIF1 = F10 * diag([1,1,0]) * F10';
   FpF1 = Fp1 * Fp1';
   eIe1 = me1 * diag([1,1,0]) * me1';
   epe1 = mep1 * mep1';

   A1 = A2 = zeros(12,2);
   for i=1:3
      for j = 1:i
%	 A1(i*(i-1)/2+j,:) = [FIF0(i,j), FpF0(i,j)];
%	 A2(i*(i-1)/2+j,:) = [eIe0(i,j), epe0(i,j)];
	 A1(6+i*(i-1)/2+j,:) = [eIe1(i,j), epe1(i,j)];
	 A2(6+i*(i-1)/2+j,:) = [FIF1(i,j), FpF1(i,j)];
      end;
   end;
   [F10,e10],pp1'*F10*pp0
   % scaling didn't improve results:
   %   A1 = A1/norm(A1,1);
   %   A2 = A2/norm(A2,1);
   [A1,A2]
   [U,S,V] = svd([A1,A2]);
   diag(S)'
   % sqrt(sumsq(A)), sqrt(sumsq(A'))
   sol = V(:,4);
   cond = [S(4,4)/S(3,3),S(3,3)/S(1,1)];
   fs = sqrt([sol(3)/sol(4), sol(1)/sol(2)]);
end;
