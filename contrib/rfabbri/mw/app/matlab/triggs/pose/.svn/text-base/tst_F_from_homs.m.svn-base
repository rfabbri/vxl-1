function terr = tst_F_from_homs(camrot,camtrans,noise)
   
   % camera 1 translates by t1 then rotates by R1 w.r.t. camera 2

   R1 = quat_to_rot([pi*camrot/180/sqrt(3) * randn(3,1); 1]);
   t1 = camtrans * randn(3,1);
   P1 = [ R1, -R1 * t1 ];
   P2 = [ eye(3,4) ];
   % F = R1 * mcross(t1)
   
   p1 = [0,0,1,-10]; X1 = [-1,0,10,1; 1,0,10,1; -1,1,10,1; 1,1,10,1]';
%   p2 = [0,1,1,-5]; X2 = [0,0,5,1; 1,0,5,1; 0,1,4,1; 1,1,4,1]';
   p2 = [0,0,1,-8]; X2 = [0,0,8,1; 1,0,8,1; 0,1,8,1; -1,1,8,1]';
   % [p1*X1, p2*X2]
   x1 = P1 * [X1,X2];
   x2 = P2 * [X1,X2];
   x1 = x1 * diag(sumsq(x1).^(-1/2)) + noise*randn(3,8);
   x2 = x2 * diag(sumsq(x2).^(-1/2)) + noise*randn(3,8);
   % for i = 1:8; x1(:,i)'*F*x2(:,i) end;

   _H1 = R1 * (eye(3) + t1*p1(1:3)/p1(4));
   _H2 = R1 * (eye(3) + t1*p2(1:3)/p2(4));
   % [H1'*F+F'*H1, H2'*F+F'*H2]

   [H1,cond] = proj_from_pts_lin(x1(:,1:4),x2(:,1:4));
   [H2,cond] = proj_from_pts_lin(x1(:,5:8),x2(:,5:8));
   % [norm(_H1/norm(_H1) - H1/norm(H1)), norm(_H2/norm(_H2) - H2/norm(H2))]
   
%   x1 = []; x2 = [];
   H12 = [H1,H2];
   H12 = [];


   [F12,e12,e21,cond] = Fmat_pts_homs_lin(x1,x2,H12);
   % [norm(t1/norm(t1)+e12),norm(t1/norm(t1)-e12)]
   
   terr = min(norm(t1/norm(t1)+e12),norm(t1/norm(t1)-e12));
   
   
%end;