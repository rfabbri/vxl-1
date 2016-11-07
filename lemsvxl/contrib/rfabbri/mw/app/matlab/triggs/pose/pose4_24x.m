% Pose of a calibrated camera from exactly 4 known 3D points. Subroutine
% based on 24x24 resultant matrix, with first order correction for noise.

% If Pij = ui^2 + uj^2 - Cij*ui*uj - Dij = 0 are the cosine rule
% polynomials (where Cij = 2*cos(theta_ij), Dij = |X_i-X_j|^2), then we
% build the 24x24 resultant matrix of all multiples ui*Pjk for i,j<k =
% 1:4. This gives a singular vector proportional to
%
% [u1^3, u1^2*u2, u1^2*u3, u1^2*u4, u1*u2^2, u2^3, u2^2*u3, u2^2*u4, 
%  u1*u3^2, u2*u3^2, u3^3, u3^2*u4, u1*u4^2, u2*u4^2, u3*u4^2, u4^3, 
%  u1*u2*u3, u1*u2*u4, u1*u3*u4, u2*u3*u4, u1, u2, u3, u4]

function u = pose4_24x(C,D,weight)
   M = zeros(24);
   C12 = C(1,2); C13 = C(1,3); C14 = C(1,4);
   C23 = C(2,3); C24 = C(2,4); C34 = C(3,4);
   D12 = D(1,2); D13 = D(1,3); D14 = D(1,4);
   D23 = D(2,3); D24 = D(2,4); D34 = D(3,4);

   M(1,1) = 1;	 M(1,5) = 1;	M(1, 2) = -C12;	 M(1,21) = -D12;
   M(2,2) = 1;	 M(2,6) = 1;	M(2, 5) = -C12;	 M(2,22) = -D12;
   M(3,3) = 1;	 M(3,7) = 1;	M(3,17) = -C12;	 M(3,23) = -D12;
   M(4,4) = 1;	 M(4,8) = 1;	M(4,18) = -C12;	 M(4,24) = -D12;

   M(5,1) = 1;	 M(5, 9) = 1;	M(5, 3) = -C13;	 M(5,21) = -D13;
   M(6,2) = 1;	 M(6,10) = 1;	M(6,17) = -C13;	 M(6,22) = -D13;
   M(7,3) = 1;	 M(7,11) = 1;	M(7, 9) = -C13;	 M(7,23) = -D13;
   M(8,4) = 1;	 M(8,12) = 1;	M(8,19) = -C13;	 M(8,24) = -D13;

   M( 9,1) = 1;	 M( 9,13) = 1;	M( 9, 4) = -C14; M( 9,21) = -D14;
   M(10,2) = 1;	 M(10,14) = 1;	M(10,18) = -C14; M(10,22) = -D14;
   M(11,3) = 1;	 M(11,15) = 1;	M(11,19) = -C14; M(11,23) = -D14;
   M(12,4) = 1;	 M(12,16) = 1;	M(12,13) = -C14; M(12,24) = -D14;

   M(13,5) = 1;	 M(13, 9) = 1;	M(13,17) = -C23; M(13,21) = -D23;
   M(14,6) = 1;	 M(14,10) = 1;	M(14, 7) = -C23; M(14,22) = -D23;
   M(15,7) = 1;	 M(15,11) = 1;	M(15,10) = -C23; M(15,23) = -D23;
   M(16,8) = 1;	 M(16,12) = 1;	M(16,20) = -C23; M(16,24) = -D23;

   M(17,5) = 1;	 M(17,13) = 1;	M(17,18) = -C24; M(17,21) = -D24;
   M(18,6) = 1;	 M(18,14) = 1;	M(18, 8) = -C24; M(18,22) = -D24;
   M(19,7) = 1;	 M(19,15) = 1;	M(19,20) = -C24; M(19,23) = -D24;
   M(20,8) = 1;	 M(20,16) = 1;	M(20,14) = -C24; M(20,24) = -D24;

   M(21, 9) = 1; M(21,13) = 1;	M(21,19) = -C34; M(21,21) = -D34;
   M(22,10) = 1; M(22,14) = 1;	M(22,20) = -C34; M(22,22) = -D34;
   M(23,11) = 1; M(23,15) = 1;	M(23,12) = -C34; M(23,23) = -D34;
   M(24,12) = 1; M(24,16) = 1;	M(24,15) = -C34; M(24,24) = -D34;

   if 1
      % ***FIXME*** this makes the weighting of the dij errors
      % wrong below
      M(1:4,:)   = M(1:4,:)   / sin(acos(C12/2));
      M(5:8,:)   = M(5:8,:)   / sin(acos(C13/2));
      M(9:12,:)  = M(9:12,:)  / sin(acos(C14/2));
      M(13:16,:) = M(13:16,:) / sin(acos(C23/2));
      M(17:20,:) = M(17:20,:) / sin(acos(C24/2));
      M(21:24,:) = M(21:24,:) / sin(acos(C34/2));
   end

   % SVD resultant matrix to find null vector v
   [U,S,V] = svd(M);
   S = diag(S)';
   u = U(:,24)';
   v = V(:,24);

   if weight(1) > 0
      DC12 = -v([2;5;17;18])*weight(1);
      DC13 = -v([3;17;9;19])*weight(1);
      DC14 = -v([4;18;19;13])*weight(1);
      DC23 = -v([17;7;10;20])*weight(1);
      DC24 = -v([18;8;20;14])*weight(1);
      DC34 = -v([19;20;12;15])*weight(1);
      step = 1;
   else
      DC12 = zeros(4,0); DC13 = DC12; DC14 = DC12; 
      DC23 = DC12; DC24 = DC12; DC34 = DC12;
      step = 0;
   end
   if weight(2) > 0
      DD = -v([21;22;23;24])*weight(2);
      step = step+1;
   else
      DD = zeros(4,0);
   end
   B = [u(1:4)*[DC12,DD], u(5:8)*[DC13,DD], u(9:12)*[DC14,DD], ...
	  u(13:16)*[DC23,DD], u(17:20)*[DC24,DD], u(21:24)*[DC34,DD]];
   b = -B'/(B*B')*S(24);

   % B*b, B*b+S(24)

   DM = zeros(24);
   doff = 1;
   if weight(1)>0
      DC12 = b(step*0+1);
      DC13 = b(step*1+1);
      DC14 = b(step*2+1);
      DC23 = b(step*3+1);
      DC24 = b(step*4+1);
      DC34 = b(step*5+1);
      doff = 2;
      DM( 1, 2) = -DC12;
      DM( 2, 5) = -DC12;
      DM( 3,17) = -DC12;
      DM( 4,18) = -DC12;
      DM( 5, 3) = -DC13;
      DM( 6,17) = -DC13;
      DM( 7, 9) = -DC13;
      DM( 8,19) = -DC13;
      DM( 9, 4) = -DC14;
      DM(10,18) = -DC14;
      DM(11,19) = -DC14;
      DM(12,13) = -DC14;
      DM(13,17) = -DC23;
      DM(14, 7) = -DC23;
      DM(15,10) = -DC23;
      DM(16,20) = -DC23;
      DM(17,18) = -DC24;
      DM(18, 8) = -DC24;
      DM(19,20) = -DC24;
      DM(20,14) = -DC24;
      DM(21,19) = -DC34;
      DM(22,20) = -DC34;
      DM(23,12) = -DC34;
      DM(24,15) = -DC34;
   end

   if weight(2)>0
      DD12 = b(step*0+doff);
      DD13 = b(step*1+doff);
      DD14 = b(step*2+doff);
      DD23 = b(step*3+doff);
      DD24 = b(step*4+doff);
      DD34 = b(step*5+doff);

      DM( 1,21) = -DD12;
      DM( 2,22) = -DD12;
      DM( 3,23) = -DD12;
      DM( 4,24) = -DD12;
      DM( 5,21) = -DD13;
      DM( 6,22) = -DD13;
      DM( 7,23) = -DD13;
      DM( 8,24) = -DD13;
      DM( 9,21) = -DD14;
      DM(10,22) = -DD14;
      DM(11,23) = -DD14;
      DM(12,24) = -DD14;
      DM(13,21) = -DD23;
      DM(14,22) = -DD23;
      DM(15,23) = -DD23;
      DM(16,24) = -DD23;
      DM(17,21) = -DD24;
      DM(18,22) = -DD24;
      DM(19,23) = -DD24;
      DM(20,24) = -DD24;
      DM(21,21) = -DD34;
      DM(22,22) = -DD34;
      DM(23,23) = -DD34;
      DM(24,24) = -DD34;
   end

   DA = U'*DM*V;
   
   %   v = V * [((S(1:23) .* DA(24,1:23))' + S(24) * DA(1:23,24)) ...
   %	    ./ (S(24)^2 - S(1:23)'.^2); ...
   %	    1];
   v = V * [(S(24) * DA(24,1:23)' + S(1:23)' .* DA(1:23,24)) ...
	  ./ (S(24)^2 - S(1:23)'.^2); ...
	  1];

   if 1
      [U1,S1,V1] = svd(M+DM);
      S1 = diag(S1)';
      v1 = V1(:,24);
      v = v/norm(v,2);
      S_conds = [S([23,24])/S(1); S1([23,24])/S1(1)]
      dv_norms = min([ norm(v1-v),norm(V(:,24)+v),norm(V(:,24)-v1); ...
	     norm(v1+v),norm(V(:,24)-v),norm(V(:,24)+v1)])
   end

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
