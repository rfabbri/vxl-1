% generate n uniform random points in a box [-1,1]x[-1,1]x[-flatness,flatness]
% and their projections into m random cameras, which are about dist from
% the origin, spread randomly by spread, and rotated randomly by about rot degrees

function [Rts,Xs,xs] = rand_images(m,n,noise,dist,spread,rot,flatness)
   if (n>0) 
      Xs = diag([1,1,flatness])*(2*rand(3,n)-ones(3,n));
   else
      Xs = zeros(3,0);
   end;
   Rts = zeros(3*m,4); 
   xs = zeros(3*m,n);
   for i = 1:m
      R = quat_to_rot([pi*rot/180*randn(3,1);1]);
      t = spread*randn(3,1) - [0; 0; dist];
      % make sure we stay on same side of scene.
      if (t(3) > -0.1*dist) t(3) = -0.2*dist - t(3); end;
      Rts(3*i-2:3*i,:) = [R,t];
      for p = 1:n
	 px = R*(Xs(:,p)-t);
	 px = px/norm(px)+noise*rand(3,1);
	 xs(3*i-2:3*i,p) = px/norm(px);
      end;
   end;
%end;   
