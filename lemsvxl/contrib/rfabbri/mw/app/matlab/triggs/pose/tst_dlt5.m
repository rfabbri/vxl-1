function tst_dlt5()
   noise = 1e-4;
   flat = 1e0;
   n = 5;
   f0 = exp(0.5*randn(1));
   pp0 = 0.2*randn(2,1);
   K0 = [f0,0,pp0(1); 0,f0,pp0(2); 0,0,1];
   [R0,junk] = qr(randn(3));
   t0 = [0;0;3];
   P0 = K0*R0*[eye(3), -t0];
   X = diag([1,1,flat,1]) * [randn(3,n); ones(1,n)];
   x = P0*X;
   x = x ./ (ones(3,1)*x(3,:)) + noise*[randn(2,n);zeros(1,n)];

   [P,K,Rt,cond] = dlt5(x,X);
   DP = P/norm(P,1) - P0/norm(P0,1);
   DK = K-K0;
   DR = Rt(:,1:3)'*R0 - eye(3);
   DT = Rt(:,1:3)'*Rt(:,4) + t0;
   [norm(DP),norm(DK),norm(DR),norm(DT)]
   cond
end;
