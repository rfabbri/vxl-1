% Evaluation function for camera calibration & pose refinement from
% known 3D points.

function [fpars,e,dedy] = calib_pose_eval(fpars,fdata,dpars)
   % function [K,R,t] = calib_pose_eval(K,R,t,x,X,yfree)
   pfree = fdata(1:6);
   n = fdata(7);
   Xx = reshape(fdata(8:7+7*n),7,n);
   X = Xx(1:4,:);
   x = Xx(5:7,:);
   q = fpars(1:4);
   t = fpars(5:7);
   kv = fpars(8:12);

   if nargin>=3		% state update
      i = 1;
      if pfree(1)
	 q = quat_mult(q,[dpars(i:i+2);1]); 
	 q = q/norm(q);
	 i = i+3;
      end
      if pfree(2), t = t + dpars(i:i+2); i=i+3; end
      if pfree(3), kv(1) = kv(1) + dpars(i); i=i+1; end
      if pfree(4), kv(2) = kv(2) + dpars(i); i=i+1; end
      if pfree(5), kv(3) = kv(3) + dpars(i); i=i+1; end
      if pfree(6), kv(4:5) = kv(4:5) + dpars(i:i+1); i=i+2; end
      fpars = [q; t; kv];
   end
   if nargout>=2

      K = [1,kv(3),kv(4); 0,kv(2),kv(5); 0,0,kv(1)];
      R = quat_to_rot(q);
      Rt = [R,t];
      P = K*Rt;

      % Find projection error and its Jacobian w.r.t. P for each point.
      e = zeros(2*n,1);
      dedP = zeros(2*n,12);
      for i = 1:n
	 Xi = X(:,i);
	 PX = P*Xi;
	 e(2*i-1:2*i) = PX(1:2)/PX(3) - x(1:2,i)/x(3,i);
	 if nargout>=3
	    dedP(2*i-1:2*i,:) = [ ...
		   Xi'/PX(3), zeros(1,4), (-PX(1)/PX(3)^2) * Xi'; ...
		   zeros(1,4), Xi'/PX(3), (-PX(2)/PX(3)^2) * Xi' ];
	 end
      end
      if nargout>=3

	 Rx = [0,0,0; 0,0,-1; 0,1,0];
	 Ry = [0,0,1; 0,0,0; -1,0,0];
	 Rz = [0,-1,0; 1,0,0; 0,0,0];
	 
	 KR = K*R;

	 % Find Jacobian of projection matrix w.r.t. free
	 % rotation/translation/calibration parameters
	 
	 dPdy = zeros(12,0);
	 if pfree(1)		% include rotation columns dPdR 
	    dPdy = [dPdy, ...
		   reshape([KR*Rx,zeros(3,1)]',12,1),...
		   reshape([KR*Ry,zeros(3,1)]',12,1),...
		   reshape([KR*Rz,zeros(3,1)]',12,1) ];
	 end
	 if pfree(2)		% include translation columns dPdt
	    dPdy = [dPdy, ...
		   reshape([zeros(3),K(:,1)]',12,1),...
		   reshape([zeros(3),K(:,2)]',12,1),...
		   reshape([zeros(3),K(:,3)]',12,1) ];
	 end
	 if pfree(3)		 % include 1/(focal length) column dPdf
	    dPdy = [dPdy, [zeros(8,1); Rt(3,:)']]; 
	 end
	 if pfree(4)		% include aspect ratio column dPda
	    dPdy = [dPdy, [zeros(4,1); Rt(2,:)'; zeros(4,1)]]; 
	 end
	 if pfree(5)		 % include skew column dPds
	    dPdy = [dPdy, [Rt(2,:)'; zeros(8,1)]]; 
	 end
	 if pfree(6)		% include principal point columns dPdu,dPdv
	    dPdy = [dPdy, ...
		   [Rt(3,:)'; zeros(8,1)], ...
		   [zeros(4,1); Rt(3,:)'; zeros(4,1)]];
	 end
	 dedy = dedP * dPdy;
	 % [U,S,V] = svd(dedy);
	 % diag(S)',V
      end
   end
end
