% tst_pt_line_relor() -- test relative orientation from coplanar lines
% and points. Generates a random scene and set of camera positions, runs
% the relative orientation driver on them, and prints the ground-truth
% residuals of the solutions found. nx = #points, nl = #lines, noise =
% Gaussian noise of extracted points and lines in radians. There is no
% unique solution unless nx+np>4 and max(nx,np)>3.
% The 2 cameras are randomly rotated by ROT and translated by TRANS from
% a point DIST in front of the plane.

function tst_pt_line_relor(nx,nl,rot,trans,dist,noise)

   % Generate random camera motions, points on 3D x-y plane, 
   % and their images.
   
   [Rts,Xs,xs] = rand_images(2,nx,noise,dist,trans,rot,0);
   R1 = Rts(1:3,1:3);
   R2 = Rts(4:6,1:3);
   t1 = Rts(1:3,4);
   t2 = Rts(4:6,4);
   R21 = R2*R1';
   t21 = R1*(t2-t1);
%   Rt0 = [R21,t21/norm(t21,2)];
   H1 = R1 * [ 1,0,-t1(1); 0,1,-t1(2); 0,0,-t1(3) ];
   H2 = R2 * [ 1,0,-t2(1); 0,1,-t2(2); 0,0,-t2(3) ];
%   H = H2 * inv(H1);
%   H = sqrt(3) * H / sum(sum(H.*H))

   if (nx>0)
      X1 = xs(1:3,:); 
      X2 = xs(4:6,:);
   else
      X1 = zeros(3,0);
      X2 = zeros(3,0);
   end;
   
   % Generate random lines in 3D x-y plane, and use 3D plane to 
   % image homographies to generate image lines. The noise model is
   % bogus but this is only for testing...

   if (nl>0)
      Ls = randn(3,nl);
      L1 = inv(H1)' * Ls;
      L2 = inv(H2)' * Ls;
      L1 = L1 / diag(sqrt(sumsq(L1))) + noise*randn(3,nl);
      L2 = L2 / diag(sqrt(sumsq(L2))) + noise*randn(3,nl);   
   else
      L1 = zeros(3,0);
      L2 = zeros(3,0);
   end;

   % SIDE is used only for sign disambiguation. It can be image 1 of 
   % any finite point in the plane. If there are points we take the
   % average, if not, the image of the origin.
   if (nx > 0)
      if (nx > 1) side = sum(X1')'; else side = X1; end;
   else
      side = -R1 * t1;
   end;
   
   [H21,cond2] = pts_planes_to_proj(X2,X1,L2,L1);
   M = homog_to_Rt(H21,side);
   Rts = M(1:3,:);

%   DH = H21 * H1 * inv(H2);
%   norm(DH / abs(det(DH)^(1/3)) - eye(3))

   errs = [];
   for i = 1:size(Rts,2)/4
      R = Rts(:,4*i-3:4*i-1); t = Rts(:,4*i);
      Rerr = norm( R21'*R-eye(3), Inf);
      terr = norm( t21/norm(t21)-t, Inf);
      errs = [errs; Rerr,terr];
      fprintf(1,'solution %d: error(R)=%g, error(t)=%g\n',...
	  i,Rerr,terr);
   end;
%    if (min(sum(errs')) > 1e-2)
%      fprintf(1,'***failed***'); 
%      errs
%      H1,H2,H21,DH
%      R1,t1,R2,t2,Rt0
%    end;

%end
