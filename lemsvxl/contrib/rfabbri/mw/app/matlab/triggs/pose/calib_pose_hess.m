% Full-Newton gradient and Hessian for optimizing camera pose +
% calibration from images of known 3D points. For use with my
% fullnewt() optimizer or similar.

% The camera projection is P = K*[R,t] with parameters
% [q,t,invf,u0,v0,a,s] where q is a unit quaternion giving camera
% rotation R=R(q), t is the position of the 3D origin w.r.t. the
% calibrated camera frame, and K = [1,s,u0; 0,a,v0; 0,0,invf] is the
% camera calibration with parameters invf,[u0,v0],a,s (inverse focal
% length, principal point, aspect ratio and skew).

% The formulae were calculated and optimized automatically by Maple
% which is why they're a bit of a mess. They could be tidied a bit by
% hand computation, but probably not reduced in size much -- the use of
% exact 2nd derivatives gives significantly better convergence for this
% problem, but calculating them is messy however you do it.

function [y,e,ey,eyy] = calib_pose_hess(y,data,dy)
   yfree = data(1:11);
   free_inds = find(yfree);
   
   if nargin>=3		% state update
      dy1 = zeros(11,1);
      dy1(free_inds) = dy;
      if yfree(1:3)
	 q = quat_mult(y(1:4),[dy1(1:3)/2;1]); 
	 y(1:4) = q/norm(q);
      end
      y(5:12) = y(5:12) + dy1(4:11);
   end
   if nargout>=2	% find error and maybe derivatives
      % Unpack 3D and image points, and camera parameters
      n = data(12);
      Xx = reshape(data(13:12+7*n),7,n);
      X = Xx(1:4,:);
      x = Xx(5:7,:);
      q = y(1:4);
      t = y(5:7);
      invf = y(8);
      a = y(9);
      s = y(10);
      u0 = y(11);
      v0 = y(12);
      R = quat_to_rot(q);
      K = [1,s,u0; 0,a,v0; 0,0,invf];
      P = K*[R,t];

      % Accumulate error e and derivatives ey = de/dy, 
      % eyy = d2e/dy2 from each point
      e = 0;
      if nargout>=3
	 ey = zeros(11,1); 
	 if nargout>=4, eyy = zeros(11); end
      end
      for i = 1:n
	 % cost function contribution
	 Xi = X(:,i);
	 PX = P*Xi;
	 id = 1/PX(3);
	 dx = PX(1:2)*id-x(1:2,i)/x(3,i);
	 e = e + dx'*dx;

	 if nargout>=3
	    % gradient contributions
	    id2 = id^2;
	    t18 = R(3,3)*Xi(2)-R(3,2)*Xi(3);
	    t19 = t18*invf;
	    t20 = id2*PX(2);
	    t21 = id2*PX(1);
	    t22 = P(2,3)*Xi(2)-P(2,2)*Xi(3);
	    t23 = P(1,3)*Xi(2)-P(1,2)*Xi(3);
	    t24 = t22*id-t19*t20;
	    t25 = t23*id-t19*t21;
	    ey(1) = ey(1) + dx(1)*t25+dx(2)*t24;
	    t26 = -R(3,3)*Xi(1)+R(3,1)*Xi(3);
	    t27 = t26*invf;
	    t28 = -P(2,3)*Xi(1)+P(2,1)*Xi(3);
	    t29 = -P(1,3)*Xi(1)+P(1,1)*Xi(3);
	    t30 = t28*id-t27*t20;
	    t31 = t29*id-t27*t21;
	    ey(2) = ey(2) + dx(1)*t31+dx(2)*t30;
	    t32 = R(3,2)*Xi(1)-R(3,1)*Xi(2);
	    t33 = t32*invf;
	    t34 = P(2,2)*Xi(1)-P(2,1)*Xi(2);
	    t35 = P(1,2)*Xi(1)-P(1,1)*Xi(2);
	    t36 = t34*id-t33*t20;
	    t37 = t35*id-t33*t21;
	    ey(3) = ey(3) + dx(1)*t37+dx(2)*t36;
	    t38 = Xi(4)*id;
	    t39 = dx(1)*t38;
	    ey(4) = ey(4) + t39;
	    t40 = dx(1)*s+dx(2)*a;
	    ey(5) = ey(5) + t40*t38;
	    t41 = id2*invf;
	    t42 = (v0*id-PX(2)*t41)*Xi(4);
	    t43 = (u0*id-PX(1)*t41)*Xi(4);
	    ey(6) = ey(6) + dx(1)*t43+dx(2)*t42;
	    t9 = R(3,1)*Xi(1)+R(3,2)*Xi(2)+R(3,3)*Xi(3)+t(3)*Xi(4);
	    t44 = id2*t9;
	    t45 = -dx(1)*PX(1)-dx(2)*PX(2);
	    ey(7) = ey(7) + t45*t44;
	    t46 = R(2,1)*Xi(1)+R(2,2)*Xi(2)+R(2,3)*Xi(3)+t(2)*Xi(4);
	    t47 = id*t46;
	    ey(8) = ey(8) + dx(2)*t47;
	    ey(9) = ey(9) + dx(1)*t47;
	    t48 = t9*id;
	    ey(10) = ey(10) + dx(1)*t48;
	    t49 = dx(2)*id;
	    ey(11) = ey(11) + t9*t49;
	    if nargout>=4
	       % Hessian contributions
	       t50 = id2*id;
	       t51 = 2*dx(1);
	       t52 = 2*dx(2);
	       t53 = id2*t19;
	       t54 = PX(1)*t50;
	       t55 = PX(2)*t50;
	       t56 = t19^2;
	       eyy(1,1) = eyy(1,1) + t25^2+t24^2 ...
		   +(-t23*t53+t56*t54)*t51+(-t22*t53+t56*t55)*t52;
	       t57 = 2*t50;
	       t58 = t45*t57;
	       t59 = t19*t58;
	       eyy(2,1) = eyy(2,1) + t31*t25+t24*t30-t27*t59 ...
		   +(dx(1)*(-t29*t19-t23*t27)+dx(2)*(-t28*t19-t22*t27))*id2;
	       t60 = id2*t27;
	       t61 = t27^2*t50;
	       eyy(2,2) = eyy(2,2) + t31^2+t30^2 ...
		   +(-t29*t60+PX(1)*t61)*t51+(-t28*t60+PX(2)*t61)*t52;
	       t62 = t33*t58;
	       eyy(3,1) = eyy(3,1) + t37*t25+t36*t24-t19*t62 ...
		   +(dx(1)*(-t35*t19-t23*t33)+dx(2)*(-t34*t19-t22*t33))*id2;
	       eyy(3,2) = eyy(3,2) + t37*t31+t30*t36-t27*t62 ...
		   +(dx(1)*(-t35*t27-t29*t33)+dx(2)*(-t34*t27-t28*t33))*id2;
	       t63 = id2*t33;
	       t64 = t33^2;
	       eyy(3,3) = eyy(3,3) + t37^2+t36^2 ...
		   +(-t35*t63+t64*t54)*t51+(-t34*t63+t64*t55)*t52;
	       t65 = dx(1)*id2;
	       eyy(4,1) = eyy(4,1) + (id*t25-t19*t65)*Xi(4);
	       eyy(4,2) = eyy(4,2) + (id*t31-dx(1)*t60)*Xi(4);
	       eyy(4,3) = eyy(4,3) + (id*t37-dx(1)*t63)*Xi(4);
	       t66 = Xi(4)^2;
	       t67 = t66*id2;
	       eyy(4,4) = eyy(4,4) + t67;
	       t68 = t40*id2;
	       eyy(5,1) = eyy(5,1) + ((s*t25+a*t24)*id-t19*t68)*Xi(4);
	       eyy(5,2) = eyy(5,2) + ((s*t31+a*t30)*id-t27*t68)*Xi(4);
	       eyy(5,3) = eyy(5,3) + ((s*t37+a*t36)*id-t33*t68)*Xi(4);
	       eyy(5,4) = eyy(5,4) + s*t67;
	       eyy(5,5) = eyy(5,5) + (s^2+a^2)*t67;
	       t69 = invf*t58;
	       eyy(6,1) = eyy(6,1) + t25*t43+t24*t42 ...
		   +(-t19*t69+(dx(1)*(-u0*t19-t23*invf)+dx(2)*(-v0*t19-t22*invf))*id2)*Xi(4);
	       eyy(6,2) = eyy(6,2) + t43*t31+t42*t30 ...
		   +(-t27*t69+(dx(1)*(-u0*t27-t29*invf)+dx(2)*(-v0*t27-t28*invf))*id2)*Xi(4);
	       eyy(6,3) = eyy(6,3) + t43*t37+t42*t36 ...
		   +(-t33*t69+(dx(1)*(-u0*t33-t35*invf)+dx(2)*(-v0*t33-t34*invf))*id2)*Xi(4);
	       t70 = dx(1)*t66;
	       eyy(6,4) = eyy(6,4) + t43*t38-t41*t70;
	       eyy(6,5) = eyy(6,5) + (t43*s+t42*a)*t38-t66*invf*t68;
	       t71 = invf^2;
	       eyy(6,6) = eyy(6,6) + t43^2+t42^2+2*(-u0*t41+t71*t54)*t70 ...
		   +(-v0*t41+t71*t55)*t66*t52;
	       eyy(7,1) = eyy(7,1) + -t9*t59+ ...
		   (t45*t18+(-dx(1)*t23-PX(1)*t25-PX(2)*t24-dx(2)*t22)*t9)*id2;
	       eyy(7,2) = eyy(7,2) + -t9*t27*t58 ...
		   +(t45*t26+(-dx(1)*t29-PX(1)*t31-PX(2)*t30-dx(2)*t28)*t9)*id2;
	       t72 = t9*t33*t57;
	       eyy(7,3) = eyy(7,3) + -t9*t37*t21-t9*t36*t20 ...
		   +dx(1)*(PX(1)*t72+(-t35*t9-PX(1)*t32)*id2) ...
		   +dx(2)*(PX(2)*t72+(-t34*t9-PX(2)*t32)*id2);
	       t73 = -t54-t65;
	       t74 = t9*t73;
	       eyy(7,4) = eyy(7,4) + Xi(4)*t74;
	       t75 = t9*Xi(4);
	       eyy(7,5) = eyy(7,5) + ((-PX(1)*s-PX(2)*a)*t50-t68)*t75;
	       eyy(7,6) = eyy(7,6) + (-PX(1)*t43-PX(2)*t42)*t44 ...
		   +(-t9*t69+(dx(1)*(-u0*t9-PX(1))+dx(2)*(-v0*t9-PX(2)))*id2)*Xi(4);
	       t76 = t9^2;
	       eyy(7,7) = eyy(7,7) + ((PX(1)^2+PX(2)^2)*id2^2-t58)*t76;
	       t77 = id2*t46;
	       t78 = (R(2,3)*Xi(2)-R(2,2)*Xi(3))*id-t19*t77;
	       eyy(8,1) = eyy(8,1) + t24*t47+dx(2)*t78;
	       t79 = (-R(2,3)*Xi(1)+R(2,1)*Xi(3))*id-t46*t60;
	       eyy(8,2) = eyy(8,2) + t47*t30+dx(2)*t79;
	       t80 = (R(2,2)*Xi(1)-R(2,1)*Xi(2))*id-t46*t63;
	       eyy(8,3) = eyy(8,3) + t36*t47+dx(2)*t80;
	       eyy(8,5) = eyy(8,5) + (a*t77+t49)*Xi(4);
	       t81 = Xi(4)*t41;
	       eyy(8,6) = eyy(8,6) + (id*t42-dx(2)*t81)*t46;
	       t82 = t9*t46;
	       t83 = -t55-dx(2)*id2;
	       eyy(8,7) = eyy(8,7) + t83*t82;
	       t84 = t46^2*id2;
	       eyy(8,8) = eyy(8,8) + t84;
	       eyy(9,1) = eyy(9,1) + t25*t47+dx(1)*t78;
	       eyy(9,2) = eyy(9,2) + t31*t47+dx(1)*t79;
	       eyy(9,3) = eyy(9,3) + t37*t47+dx(1)*t80;
	       t85 = Xi(4)*t77;
	       eyy(9,4) = eyy(9,4) + t85;
	       eyy(9,5) = eyy(9,5) + s*t85+t39;
	       eyy(9,6) = eyy(9,6) + (id*t43-dx(1)*t81)*t46;
	       eyy(9,7) = eyy(9,7) + t46*t74;
	       eyy(9,9) = eyy(9,9) + t84;
	       t86 = t18*id-t19*t44;
	       eyy(10,1) = eyy(10,1) + t25*t48+dx(1)*t86;
	       t87 = t26*id-t27*t44;
	       eyy(10,2) = eyy(10,2) + t31*t48+dx(1)*t87;
	       t88 = t32*id-t9*t63;
	       eyy(10,3) = eyy(10,3) + t37*t48+dx(1)*t88;
	       t89 = id2*t75;
	       eyy(10,4) = eyy(10,4) + t89;
	       eyy(10,5) = eyy(10,5) + s*t89;
	       t90 = (id-t9*t41)*Xi(4);
	       eyy(10,6) = eyy(10,6) + t43*t48+dx(1)*t90;
	       eyy(10,7) = eyy(10,7) + t73*t76;
	       t91 = id2*t82;
	       eyy(10,9) = eyy(10,9) + t91;
	       t92 = t76*id2;
	       eyy(10,10) = eyy(10,10) + t92;
	       eyy(11,1) = eyy(11,1) + t24*t48+dx(2)*t86;
	       eyy(11,2) = eyy(11,2) + t48*t30+dx(2)*t87;
	       eyy(11,3) = eyy(11,3) + t36*t48+dx(2)*t88;
	       eyy(11,5) = eyy(11,5) + a*t89;
	       eyy(11,6) = eyy(11,6) + t42*t48+dx(2)*t90;
	       eyy(11,7) = eyy(11,7) + t83*t76;
	       eyy(11,8) = eyy(11,8) + t91;
	       eyy(11,11) = eyy(11,11) + t92;
	    end
	 end
      end
      e = e/2;
      if nargout>=3
	 % Select derivative matrix elements corresponding to free
	 % variables, and fill in upper half of Hessian matrix.
	 
	 ey = ey(free_inds);
	 if nargout>=4,
	    eyy = eyy(free_inds,free_inds);
	    eyy = eyy + tril(eyy,-1)';
	 end
      end
   end
end