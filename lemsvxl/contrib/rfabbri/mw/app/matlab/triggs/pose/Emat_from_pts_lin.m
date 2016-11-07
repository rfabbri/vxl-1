% [E12,cond]=Emat_from_pts_lin(x1,x2,method)
% Linearly estimate the 3x3 essential matrix E12 linking two sets x1,x2
% of n>=6 homogeneous calibrated image points.
%
% Method: the `8 point' variant estimates the E matrix linearly from
% n>=8 point-pairs without trying to enforce any of the nonlinear
% constraints on it. The `6 point' one estimates the minimum-norm 3D
% subspace of the 9D linear space of all 3x3 E-matrix candidates, then
% tries to solve the Demazure constraints on this subspace. However it
% only does so quasi-linearly, so the result is still not guaranteed to
% be a valid essential matrix. If METHOD is not specified, we try to
% guess which will be best from the singular values of the F matrix
% equations, but this is pretty arbitrary. Use Ematrix_to_Rt() to
% project E onto the valid subspace and estimate the corresponding
% rotation and translation.

function [E12,cond] = Emat_from_pts_lin(x1,x2,method)
   [E12s,cond] = Fspace_from_pts(x1,x2,6);
   n= size(x1,2);
   if (n<8) 
      method = 6;
   elseif (nargin<3) 
      if ((n>8 & cond(9)<0.01*cond(8)) | cond(8)>0.1*cond(6)) 
	 method = 8;
      else 
	 method = 6;
      end;
   elseif (~(method==6)) 
      method = 8; 
   end;
   cond = cond(method:method+1);

   if (method==8)
      E12 = E12s(:,7:9);
   else	
      % 6 point method: E should be close to the 3D minimum residual
      % subspace, say E = x*Ex+y*Ey+z*Ez. It must satisfy the Demazure
      % constraints (E*E'-trace(E*E')/2)*E =0 (which enforce two equal
      % and one zero singular values). Write these 3x3=9 cubics as 9
      % quasi-linear equations in the 10 variables (x^3,x^2y,...,z^3),
      % solve linearly, and pull out strong estimates of x:y:z.
      % (For >=8 points the solution is usually close to (0:0:1) as Ez
      % corresponds to the `8 point' estimate of E). For nearly coplanar
      % points, rank(N)=4 and the solution is not trustworthy, although
      % for some reason it usually seems to coincide with one of the two
      % possible orientation-from-planar-points solutions...
      % 
      Ex = E12s(:,1:3);
      Ey = E12s(:,4:6);
      Ez = E12s(:,7:9);
      Nxx = Ex*Ex'; Nxx = Nxx - trace(Nxx)*eye(3)/2;
      Nyy = Ey*Ey'; Nyy = Nyy - trace(Nyy)*eye(3)/2;
      Nzz = Ez*Ez'; Nzz = Nzz - trace(Nzz)*eye(3)/2; 
      Nxy = Ex*Ey'; Nxy = Nxy+Nxy'-trace(Nxy)*eye(3);
      Nxz = Ex*Ez'; Nxz = Nxz+Nxz'-trace(Nxz)*eye(3);
      Nyz = Ey*Ez'; Nyz = Nyz+Nyz'-trace(Nyz)*eye(3);
      Nxxx = Nxx*Ex;
      Nyyy = Nyy*Ey;
      Nzzz = Nzz*Ez;
      Nxxy = Nxx*Ey+Nxy*Ex;
      Nxxz = Nxx*Ez+Nxz*Ex;
      Nxyy = Nyy*Ex+Nxy*Ey;
      Nxzz = Nzz*Ex+Nxz*Ez;
      Nyyz = Nyy*Ez+Nyz*Ey;
      Nyzz = Nzz*Ey+Nyz*Ez;
      Nxyz = Nxy*Ez+Nyz*Ex+Nxz*Ey;
      N = reshape([Nxxx,Nxxy,Nxxz,Nxyy,Nxyz,Nxzz,Nyyy,Nyyz,Nyzz,Nzzz],9,10);

      [U,S,V] = svd(N);
      S = diag(S)/S(1,1);
      V = V(:,10);
      x3 = V(1); y3 = V(7); z3 = V(10);
      if (abs(z3)>max(abs(x3),abs(y3)))
	 a = z3; x = V(6)/a; y = V(9)/a; z = 1;		% xzz,yzz,zzz
      elseif (abs(y3)>max(abs(x3),abs(z3)))
	 a = y3; x = V(4)/a; y = 1; z = V(8)/a;		% xyy,yyy,yyz
      else
	 a = x3; x = 1; y = V(2)/a; z = V(3)/a; 	% xxx,xxy,xxz
      end;
      E12 = x*Ex+y*Ey+z*Ez;
      v1 = [x^3; x^2*y; x^2*z; x*y^2; x*y*z; x*z^2; y^3; y^2*z; y*z^2; z^3];
      cond = [cond; norm(V/a-v1); S];   
   end;
%end;
