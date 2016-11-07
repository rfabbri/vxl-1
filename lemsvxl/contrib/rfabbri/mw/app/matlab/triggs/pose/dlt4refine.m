function [u,e] = dlt4refine(M,u)
   lambda = 0;
   u = u/norm(u,2);
   u2 = [u(1)^2; u(2)*u(1:2); u(3)*u(1:3); u(4)*u];
   r = M * u2;
   e = r' * r;
   for iter = 1:100
      % Jacobians
      du2du = [ 2*u(1),u(2), 0,    u(3), 0,   0,    u(4), 0,   0,   0;...
	         0,    u(1),2*u(2), 0,  u(3), 0,     0,  u(4), 0,   0;...
		 0,     0,   0,    u(1),u(2),2*u(3), 0,   0,  u(4), 0;...
      		 0,     0,   0,     0,   0,   0,    u(1),u(2),u(3),2*u(4)]';
      [Q,R] = qr(u);
      dudv = Q(:,2:4);
      dMdv = M * du2du * dudv;
      % find lambda giving an acceptable step
      while (1)
	 u0 = u; e0 = e; r0 = r;
	 %  Gauss-Newton is unreliable here owing to nonlinearity 
	 %  and occasional large residuals -- use full Newton
	 % if (lambda==0) dv = -dMdv \ r;
	 % else dv = -[dMdv; lambda^2 * eye(3)] \ [r; zeros(3,1)];	 
	 % end;
	 rm = r' * M;
	 rd2Mdu2 = [ rm(1),rm(2),rm(4),rm(7); ...
	               0,  rm(3),rm(5),rm(8); ...
		       0,    0,  rm(6),rm(9); ...
		       0,    0.    0,  rm(10)];
	 J = dMdv' * dMdv + dudv' * (rd2Mdu2 + rd2Mdu2') * dudv;
	 dv = -(J + lambda^2 * eye(3)) \ (dMdv' * r);

	 ndv = norm(dv);
	 if (ndv<1e-9 | lambda>1e3) return; 
	 elseif (ndv>0.2) 
	    % fprintf(1,'\t\t\t\t\t\t\t%6g\n',ndv/0.2);	    
	    dv = 0.2/ndv * dv;
	    lambda = max(lambda,1e-1);
	 end;
	 u = u + dudv * dv;
	 u = u/norm(u,2);
	 u2 = [u(1)^2; u(2)*u(1:2); u(3)*u(1:3); u(4)*u];
	 r = M * u2;
	 e = r' * r;
	 % fprintf(1,'%d: %6g\t%6g\t%6g\t%6g\n',iter,lambda,ndv,e,e-e0);
	 if (e < e0*(1+1e-14)+1e-14)
	    % step OK, accept
	    lambda = lambda/4;
	    if (lambda<1e-4) lambda=0; end;
	    break;
	 end;
	 % step failed, back out & retry with new lambda
	 u = u0; e = e0; r = r0;
	 lambda = max(4 * lambda, 1e-3 +(iter==1)*1e-1);
      end;
   end;
end;
