function [K,R,t] = cal_pose_refine(K,R,t,x,X,yfree)
   Rx = [0,0,0; 0,0,-1; 0,1,0];
   Ry = [0,0,1; 0,0,0; -1,0,0];
   Rz = [0,-1,0; 1,0,0; 0,0,0];

   Rt = [R,t];
   P = K*Rt;
   KR = K*R;

   % Find Jacobian of projection matrix w.r.t. free
   % rotation/translation/calibration parameters

   dPdy = zeros(12,0);
   if yfree(1)		% include rotation columns dPdR 
      dPdy = [dPdy, ...
	     reshape([KR*Rx,zeros(3,1)]',12,1),...
	     reshape([KR*Ry,zeros(3,1)]',12,1),...
	     reshape([KR*Rz,zeros(3,1)]',12,1) ];
   end;
   if yfree(2)		% include translation columns dPdt
      dPdy = [dPdy, ...
	     reshape([zeros(3),K(:,1)]',12,1),...
	     reshape([zeros(3),K(:,2)]',12,1),...
	     reshape([zeros(3),K(:,3)]',12,1) ];
   end;
   if yfree(3)		 % include 1/(focal length) column dPdf
      dPdy = [dPdy, [zeros(8,1); Rt(3,:)']]; 
   end;
   if yfree(4)		% include aspect ratio column dPda
      dPdy = [dPdy, [zeros(4,1); Rt(2,:)'; zeros(4,1)]]; 
   end; 
   if yfree(5)		 % include skew column dPds
      dPdy = [dPdy, [Rt(2,:)'; zeros(8,1)]]; 
   end;
   if yfree(6)		% include principal point columns dPdu,dPdv
      dPdy = [dPdy, ...
	     [Rt(3,:)'; zeros(8,1)], ...
	     [dPdy, [zeros(4,1); Rt(3,:)'; zeros(4,1)]]; 
   end;

   % Find projection error and its Jacobian w.r.t. P for each point.
   e = zeros(2*n,1);
   dedP = zeros(2*n,12);
   for i = 1:n
      Xi = X(:,i);
      PX = P*Xi;
      e(2*i-1:2*i) = PX(1:2)/PX(3) - x(1:2,i)/x(3,i);
      dedP(2*i-1:2*i,:) = [ ...
	     Xi'/PX(3), zeros(1,4), (-PX(1)/PX(3)^2) * Xi'; ...
	     zeros(1,4), Xi'/PX(3), (-PX(2)/PX(3)^2) * Xi' ];
   end;
   dedy = dedP * dPdy;
   

end;