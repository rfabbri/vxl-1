% Pose of a calibrated camera from exactly 4 known 3D points. Subroutine
% based on a 12x12 partially eliminated partial resultant matrix.

% If Pij = ui^2 + uj^2 - Cij*ui*uj - Dij = 0 are the cosine rule
% polynomials (where Cij = 2*cos(theta_ij), Dij = |X_i-X_j|^2), then we
% build the 24x24 resultant matrix of all multiples ui*Pjk for i,j<k =
% 1:4. Choosing the first 12 of the monomials to be of the form ui^2*uj,
% i=1:3,j=1:4 and the first 12 polynomials to be Pi4*uj gives an upper
% triangular, unit diagonal leading 12x12 submatrix which we use for
% gaussian elimination to reduce the remaining 12 polynomials to a 12x12
% system in the remaining monomials 
% [u4^3, u1*u2*u3, u1*u2*u4, u1*u3*u4, u2*u3*u4, u1, u2, u3, u4]
% As usual, the null vector of this matrix gives the solution up to a
% scale factor.

function u = pose4_9(C,D)
   M = zeros(9);
   C12 = C(1,2); C13 = C(1,3); C14 = C(1,4);
   C23 = C(2,3); C24 = C(2,4); C34 = C(3,4);
   D12 = D(1,2); D13 = D(1,3); D14 = D(1,4);
   D23 = D(2,3); D24 = D(2,4); D34 = D(3,4);

   t1 = C14*C23;
   t2 = C13*C24;
   t4 = C14*C24;
   t6 = C14*C34;
   t7 = C24*C34;
   t9 = C14*(D23-D24-D34);
   t11 = C24*(D13-D14-D34);
   t13 = C14^2;
   t16 = C23-C23*t13/2-C12*C13/2;
   t20 = -2+t13;
   t22 = C12*C34;
   t23 = t13*D23;
   t24 = t13*D24;
   t25 = t13*D34;
   t29 = C12*D34;
   t31 = C14*D14;
   t33 = C24^2;
   t35 = -C12*C23/2+C13-C13*t33/2;
   t39 = -2+t33;
   t44 = t33*D13;
   t45 = t33*D14;
   t46 = t33*D34;
   t48 = C24*D24;
   t56 = C13*D24;
   t59 = C34^2;
   t61 = -C13*C23/2+C12-C12*t59/2;
   t65 = -2+t59;
   t70 = t59*D12;
   t71 = t59*D14;
   t72 = t59*D24;
   t74 = C34*D34;
   t78 = C34*(D12-D14-D24);
   t87 = C23*D14;
   M(1,1) = -2;
   M(1,2) = -t1/2-t2/2;
   M(1,3) = -C12+t4;
   M(1,4) = t6/2;
   M(1,5) = t7/2;
   M(1,6) = -t9/2;
   M(1,7) = -t11/2;
   M(1,8) = 0;
   M(1,9) = -D12+D14+D24;
   M(2,1) = -C14;
   M(2,2) = t16;
   M(2,3) = -C12*C14/2+C24*t13/2;
   M(2,4) = t20*C34/2;
   M(2,5) = t22/2;
   M(2,6) = -D12+D14+D23-D34-t23/2+t24/2+t25/2;
   M(2,7) = -C12*D14/2-C12*D13/2+t29/2;
   M(2,8) = 0;
   M(2,9) = t31;
   M(3,1) = -C24;
   M(3,2) = t35;
   M(3,3) = -C12*C24/2+C14*t33/2;
   M(3,4) = t22/2;
   M(3,5) = t39*C34/2;
   M(3,6) = -C12*D24/2-C12*D23/2+t29/2;
   M(3,7) = -D12+D24+D13-D34-t44/2+t45/2+t46/2;
   M(3,8) = 0;
   M(3,9) = t48;
   M(4,1) = -C14;
   M(4,2) = t16;
   M(4,3) = t20*C24/2;
   M(4,4) = -C13*C14/2+C34*t13/2;
   M(4,5) = t2/2;
   M(4,6) = -D13+D14+D23-D24-t23/2+t24/2+t25/2;
   M(4,7) = 0;
   M(4,8) = -C13*D14/2-C13*D12/2+t56/2;
   M(4,9) = t31;
   M(5,1) = -C34;
   M(5,2) = t61;
   M(5,3) = t2/2;
   M(5,4) = -C13*C34/2+C14*t59/2;
   M(5,5) = t65*C24/2;
   M(5,6) = -C13*D34/2-C13*D23/2+t56/2;
   M(5,7) = 0;
   M(5,8) = -D13+D34+D12-D24-t70/2+t71/2+t72/2;
   M(5,9) = t74;
   M(6,1) = -2;
   M(6,2) = -t1/2-t22/2;
   M(6,3) = t4/2;
   M(6,4) = -C13+t6;
   M(6,5) = t7/2;
   M(6,6) = -t9/2;
   M(6,7) = 0;
   M(6,8) = -t78/2;
   M(6,9) = -D13+D14+D34;
   M(7,1) = -C24;
   M(7,2) = t35;
   M(7,3) = t39*C14/2;
   M(7,4) = t1/2;
   M(7,5) = -C23*C24/2+C34*t33/2;
   M(7,6) = 0;
   M(7,7) = -D23+D24+D13-D14-t44/2+t45/2+t46/2;
   M(7,8) = -C23*D24/2-C23*D12/2+t87/2;
   M(7,9) = t48;
   M(8,1) = -C34;
   M(8,2) = t61;
   M(8,3) = t1/2;
   M(8,4) = t65*C14/2;
   M(8,5) = -C23*C34/2+C24*t59/2;
   M(8,6) = 0;
   M(8,7) = -C23*D34/2-C23*D13/2+t87/2;
   M(8,8) = -D23+D34+D12-D14-t70/2+t71/2+t72/2;
   M(8,9) = t74;
   M(9,1) = -2;
   M(9,2) = -t2/2-t22/2;
   M(9,3) = t4/2;
   M(9,4) = t6/2;
   M(9,5) = -C23+t7;
   M(9,6) = 0;
   M(9,7) = -t11/2;
   M(9,8) = -t78/2;
   M(9,9) = -D23+D24+D34;

   % SVD resultant matrix to find null vector v
   [U,S,V] = svd(M);
   % S = diag(S)'
   v = V(:,9);

   % v = [u4^3, u1*u2*u3, u1*u2*u4, u1*u3*u4, u2*u3*u4, u1, u2, u3, u4]
   % Pull out monomials w = scale*[(u1..u4)*u4^2,(u1..u4)], find largest
   % entry and divide to get u4^2, then scale the corresponding block
   % (u1..u4) to get solution. Throw away solutions with u4^2<0.

   if v(1)/v(9)<=0, u = zeros(0,4); return; end;
   u4 = sqrt(v(1)/v(9));
   if sum(abs(v([2:5]))) > sum(abs(v([6:9]))) % big endian
      u = [v(2)/v(5),v(2)/v(4),v(2)/v(3),1]*u4;
   else		% little endian
      u = v([6:9])'/v(9)*u4;
   end;
%end;
