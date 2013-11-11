% Pose of a calibrated camera from exactly 4 known 3D points. Subroutine
% based on 24x24 resultant matrix.

% If Pij = ui^2 + uj^2 - Cij*ui*uj - Dij = 0 are the cosine rule
% polynomials (where Cij = 2*cos(theta_ij), Dij = |X_i-X_j|^2), then we
% build the 24x24 resultant matrix of all multiples ui*Pjk for i,j<k =
% 1:4. This gives a singular vector proportional to
%
% [u1^3, u1^2*u2, u1^2*u3, u1^2*u4, u1*u2^2, u2^3, u2^2*u3, u2^2*u4, 
%  u1*u3^2, u2*u3^2, u3^3, u3^2*u4, u1*u4^2, u2*u4^2, u3*u4^2, u4^3, 
%  u1*u2*u3, u1*u2*u4, u1*u3*u4, u2*u3*u4, u1, u2, u3, u4]

function u = pose4_24(C,D)
   M = zeros(24);
   C12 = C(1,2); C13 = C(1,3); C14 = C(1,4);
   C23 = C(2,3); C24 = C(2,4); C34 = C(3,4);
   D12 = D(1,2); D13 = D(1,3); D14 = D(1,4);
   D23 = D(2,3); D24 = D(2,4); D34 = D(3,4);

   M(1,1) = 1;
   M(1,4) = -C14;
   M(1,13) = 1;
   M(1,21) = -D14;

   M(2,2) = 1;
   M(2,14) = 1;
   M(2,18) = -C14;
   M(2,22) = -D14;

   M(3,3) = 1;
   M(3,15) = 1;
   M(3,19) = -C14;
   M(3,23) = -D14;

   M(4,4) = 1;
   M(4,13) = -C14;
   M(4,16) = 1;
   M(4,24) = -D14;

   M(5,5) = 1;
   M(5,13) = 1;
   M(5,18) = -C24;
   M(5,21) = -D24;

   M(6,6) = 1;
   M(6,8) = -C24;
   M(6,14) = 1;
   M(6,22) = -D24;

   M(7,7) = 1;
   M(7,15) = 1;
   M(7,20) = -C24;
   M(7,23) = -D24;

   M(8,8) = 1;
   M(8,14) = -C24;
   M(8,16) = 1;
   M(8,24) = -D24;

   M(9,9) = 1;
   M(9,13) = 1;
   M(9,19) = -C34;
   M(9,21) = -D34;

   M(10,10) = 1;
   M(10,14) = 1;
   M(10,20) = -C34;
   M(10,22) = -D34;

   M(11,11) = 1;
   M(11,12) = -C34;
   M(11,15) = 1;
   M(11,23) = -D34;

   M(12,12) = 1;
   M(12,15) = -C34;
   M(12,16) = 1;
   M(12,24) = -D34;

   M(13,5) = 1;
   M(13,9) = 1;
   M(13,17) = -C23;
   M(13,21) = -D23;

   M(14,2) = 1;
   M(14,10) = 1;
   M(14,17) = -C13;
   M(14,22) = -D13;

   M(15,3) = 1;
   M(15,7) = 1;
   M(15,17) = -C12;
   M(15,23) = -D12;

   M(16,4) = 1;
   M(16,8) = 1;
   M(16,18) = -C12;
   M(16,24) = -D12;

   M(17,1) = 1;
   M(17,2) = -C12;
   M(17,5) = 1;
   M(17,21) = -D12;

   M(18,2) = 1;
   M(18,5) = -C12;
   M(18,6) = 1;
   M(18,22) = -D12;

   M(19,1) = 1;
   M(19,3) = -C13;
   M(19,9) = 1;
   M(19,21) = -D13;

   M(20,3) = 1;
   M(20,9) = -C13;
   M(20,11) = 1;
   M(20,23) = -D13;

   M(21,4) = 1;
   M(21,12) = 1;
   M(21,19) = -C13;
   M(21,24) = -D13;

   M(22,6) = 1;
   M(22,7) = -C23;
   M(22,10) = 1;
   M(22,22) = -D23;

   M(23,7) = 1;
   M(23,10) = -C23;
   M(23,11) = 1;
   M(23,23) = -D23;

   M(24,8) = 1;
   M(24,12) = 1;
   M(24,20) = -C23;
   M(24,24) = -D23;

   % SVD resultant matrix to find null vector v
   [U,S,V] = svd(M);
   % S = diag(S)'
   v = V(:,24);

   % v is proportional to :
   % [u1^3, u1^2*u2, u1^2*u3, u1^2*u4, u1*u2^2, u2^3, u2^2*u3, u2^2*u4, 
   %  u1*u3^2, u2*u3^2, u3^3, u3^2*u4, u1*u4^2, u2*u4^2, u3*u4^2, u4^3, 
   %  u1*u2*u3, u1*u2*u4, u1*u3*u4, u2*u3*u4, u1, u2, u3, u4]

   % Select monomials w = scale*[(u1..u4)*[u1^2..u4^2,1]], find largest
   % entry and divide to get u.j^2, then scale the corresponding block
   % (u1..u4) to get solution. Throw away solutions with u4^2<0.

   % [UW,SW,VW] = svd([v(1:4), v(5:8), v(9:12), v(13:16), v(21:24)]');
   % SW = diag(SW)'
   w = v([1:4, 5:8, 9:12, 13:16, 21:24])';
   m = find(abs(w)==max(abs(w)));
   m = m(1);
   i = floor((m-1)/4);
   j = m-4*i;
   uj2 = w(5*j-4)/w(16+j);
   if uj2<=0, u = zeros(0,4); return; end;
   u = w([4*i+1:4*i+4])/w(4*i+j)*sqrt(uj2);
%end;
