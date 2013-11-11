% tst_relorient() -- test relative orientation routines generates a
% random scene and set of camera positions, runs the relative
% orientation driver on them, and prints the ground-truth residuals of
% the solutions found. n = # points, noise = Gaussian noise of extracted
% image points in radians, flatness = thickness/width ratio of scene.

function tst_relor5(n,noise,flatness)
   dist = 4; 
   spread = 2e-1; 
   rot = 45;
   [Rts,Xs,xs] = rand_images(2,n,noise,dist,spread,rot,flatness);
   x1 = xs(1:3,:); x2 = xs(4:6,:);
   R1 = Rts(1:3,1:3); t1 = Rts(1:3,4);
   R2 = Rts(4:6,1:3); t2 = Rts(4:6,4);
   R21 = R2*R1'; t21 = -R2*(t2-t1);
   % correct answer
   % Rt0=[R21,t21/norm(t21,2)], err0=sqrt(epipolar_err_proj(R21*mcross(t21),x2,x1)/n)

   if 1
      [Rts,cond] = relor_align(x2,x1);
   else
      [qs,Rts,ds] = relor5(x2,x1);
      ds
   end;
   ok=0;
   for i = 1:(size(Rts,2)/4)
      R = Rts(:,4*i-3:4*i-1); 
      t = Rts(:,4*i);
      % t21/norm(t21,2)
      Rerr = norm(R21'*R-eye(3),Inf);
      terr = min(norm(t21-norm(t21)*t,Inf),norm(-t21-norm(t21)*t,Inf))/dist;
      te1 = min(norm(t21/norm(t21)-t,Inf),norm(-t21/norm(t21)-t,Inf));
      fprintf(1,'solution %d: residual=%g, error(R)=%g, error(t)=%g (%g)\n',
             i,cond(i),Rerr,terr,te1);
	     % [R,t]
      if (Rerr<0.1 & terr<0.1) ok=1; end;
   end;
   if (~ok)
      fprintf(1,'No valid solutions!\n');
   end;
end
