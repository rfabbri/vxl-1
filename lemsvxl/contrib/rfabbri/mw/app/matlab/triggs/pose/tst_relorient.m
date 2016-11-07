% tst_relorient() -- test relative orientation routines generates a
% random scene and set of camera positions, runs the relative
% orientation driver on them, and prints the ground-truth residuals of
% the solutions found. n = # points, noise = Gaussian noise of extracted
% image points in radians, flatness = thickness/width ratio of scene.

function tst_relorient(n,noise,flatness)
%   global badseeds; seeds = [rand('seed'); randn('seed')];
   dist = 4; 
   spread = 2; 
   rot = 45;
   [Rts,Xs,xs] = rand_images(2,n,noise,dist,spread,rot,flatness);
   x1 = xs(1:3,:); x2 = xs(4:6,:);
   R1 = Rts(1:3,1:3); t1 = Rts(1:3,4);
   R2 = Rts(4:6,1:3); t2 = Rts(4:6,4);
   R21 = R2*R1'; t21 = R1*(t2-t1);
   % correct answer
   % Rt0=[R21,t21/norm(t21,2)], err0=sqrt(epipolar_err_proj(R21*mcross(t21),x2,x1)/n)

   [Rt1,cond] = relorient2(x1,x2);

   ok=0;
   for i = 1:size(Rt1,2)/4
      R = Rt1(:,4*i-3:4*i-1); t = Rt1(:,4*i);
      Rerr = norm(R21'*R-eye(3),Inf);
      terr = norm(t21/norm(t21)-t,Inf);
      fprintf(1,'solution %d: residual=%g, error(R)=%g, error(t)=%g\n',...
             i,cond(i),Rerr,terr);
	     % [R,t]
      if (Rerr<0.1 & terr<0.1) ok=1; end;
   end;
   if (~ok)
      fprintf(1,'No valid solutions!\n');
%      badseeds = [badseeds,seeds];
   end;
end

%for i=1:10, rand('seed',badseeds(1,i)); randn('seed',badseeds(2,i)); tst_relorient(6,1e-4,1e-1); end