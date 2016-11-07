% Pose of a calibrated camera from exactly 4 known 3D points. Subroutine
% based on a 12x12 partially eliminated partial resultant matrix.

% If Pij = ui^2 + uj^2 - Cij*ui*uj - Dij = 0 are the cosine rule
% polynomials (where Cij = 2*cos(theta_ij), Dij = |X_i-X_j|^2), then we
% build the 24x24 resultant matrix of all multiples ui*Pjk for i,j<k =
% 1:4. Choosing the first 12 of the monomials to be of the form ui^2*uj,
% i=1:3,j=1:4 and the first 12 polynomials to be Pi4*uj gives an upper
% triangular, unit diagonal leading 12x12 submatrix which we use for
% gaussian elimination to reduce the remaining 12 polynomials to a 12x12
% system in the remaining monomials [u1*u4^2,u2*u4^2,u3*u4^2,u4^3,
% u1*u2*u3,u1*u2*u4,u1*u3*u4,u2*u3*u4, u1,u2,u3,u4]. As usual, the null
% vector of this matrix gives the solution up to a scale factor.

function u = pose4_12(C,D)
   M = zeros(12);
   C12 = C(1,2); C13 = C(1,3); C14 = C(1,4);
   C23 = C(2,3); C24 = C(2,4); C34 = C(3,4);
   D12 = D(1,2); D13 = D(1,3); D14 = D(1,4);
   D23 = D(2,3); D24 = D(2,4); D34 = D(3,4);
   t2 = C14^2-2;
   t5 = -D12+D14+D24;
   t7 = C14*D14;
   t9 = C24^2-2;
   t13 = C24*D24;
   t16 = -D13+D14+D34;
   t19 = C34^2-2;
   t23 = C34*D34;
   t24 = -D23+D24+D34;
   M(1,1) = t2;
   M(1,2) = C12;
   M(1,4) = -C14;
   M(1,6) = -C12*C14+C24;
   M(1,9) = t5;
   M(1,10) = -C12*D14;
   M(1,12) = t7;
   M(2,1) = C12;
   M(2,2) = t9;
   M(2,4) = -C24;
   M(2,6) = C14-C12*C24;
   M(2,9) = -C12*D24;
   M(2,10) = t5;
   M(2,12) = t13;
   M(3,3) = -2;
   M(3,5) = -C12;
   M(3,7) = C14;
   M(3,8) = C24;
   M(3,11) = t5;
   M(4,1) = C14;
   M(4,2) = C24;
   M(4,4) = -2;
   M(4,6) = -C12;
   M(4,12) = t5;
   M(5,1) = t2;
   M(5,3) = C13;
   M(5,4) = -C14;
   M(5,7) = -C13*C14+C34;
   M(5,9) = t16;
   M(5,11) = -C13*D14;
   M(5,12) = t7;
   M(6,2) = -2;
   M(6,5) = -C13;
   M(6,6) = C14;
   M(6,8) = C34;
   M(6,10) = t16;
   M(7,1) = C13;
   M(7,3) = t19;
   M(7,4) = -C34;
   M(7,7) = C14-C13*C34;
   M(7,9) = -C13*D34;
   M(7,11) = t16;
   M(7,12) = t23;
   M(8,1) = C14;
   M(8,3) = C34;
   M(8,4) = -2;
   M(8,7) = -C13;
   M(8,12) = t16;
   M(9,1) = -2;
   M(9,5) = -C23;
   M(9,6) = C24;
   M(9,7) = C34;
   M(9,9) = t24;
   M(10,2) = t9;
   M(10,3) = C23;
   M(10,4) = -C24;
   M(10,8) = -C23*C24+C34;
   M(10,10) = t24;
   M(10,11) = -C23*D24;
   M(10,12) = t13;
   M(11,2) = C23;
   M(11,3) = t19;
   M(11,4) = -C34;
   M(11,8) = C24-C23*C34;
   M(11,10) = -C23*D34;
   M(11,11) = t24;
   M(11,12) = t23;
   M(12,2) = C24;
   M(12,3) = C34;
   M(12,4) = -2;
   M(12,8) = -C23;
   M(12,12) = t24;

   % SVD resultant matrix to find null vector v
   [U,S,V] = svd(M);
   % S = diag(S)'
   v = V(:,12);

   % Pull out monomials w = scale*[(u1..u4)*u4^2,(u1..u4)], find largest
   % entry and divide to get u4^2, then scale the corresponding block
   % (u1..u4) to get solution. Throw away solutions with u4^2<0.

   w = v([1:4,9:12])';
   m = find(abs(w)==max(abs(w)));
   m = m(1);
   if m<5	% big endian, u4>1
      u4 = w(m)/w(m+4);
      if u4<=0, u = zeros(0,4); return; end;
      u4 = sqrt(u4);
      u = (u4/w(4))*w(1:4);
   else		% little endian, u4<1
      u4 = w(m-4)/w(m);
      if u4<=0, u = zeros(0,4); return; end; 
      u4 = sqrt(u4);
      u = (u4/w(8))*w(5:8);
   end;
%end;
