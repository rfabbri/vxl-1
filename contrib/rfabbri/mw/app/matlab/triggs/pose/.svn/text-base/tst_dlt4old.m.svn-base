function tst_dlt4()
   noise = 1e-30;
   flat = 1e-100;
   n = 4;
   f0 = exp(0.5*randn(1));
   K0 = diag([f0,f0,1]);
   R0 = quat_to_rot([0.3*randn(3,1);1]);
   % [R0,junk] = qr(randn(3));
   % if (det(R0)<0) R0 = -R0; end;
   t0 = [0;0;-3];
   P0 = K0*R0*[eye(3), -t0];
   % X = diag([1,1,flat,1]) * [randn(3,n); ones(1,n)];
   X = diag([1,1,flat,1]) * [eye(3),ones(3,1),randn(3,n-4); ones(1,n)];
   x = P0*X;
   x = x ./ (ones(3,1)*x(3,:)) + noise*[randn(2,n);zeros(1,n)];

   % [Ps,cond] = proj_subs_from_pts(x,X,4);
   % S = svd(reshape([P0,Ps],12,5))

   % [Ps,Rts,fs,cond] = dlt4(x,X);
   [Ps,Rts,fs,cond] = dlt4init(x,X,[0,0,0]);
   dxs = [];
   for i = 1:size(fs,2)
      P = Ps(:,4*i-3:4*i);
      Rt = Rts(:,4*i-3:4*i);
      f = fs(i);
      DP = P/norm(P,1) - P0/norm(P0,1);
      DR = Rt(:,1:3)'*R0 - eye(3);
      DT = (Rt(:,1:3)'*Rt(:,4) + t0)/norm(t0);
      DF = (f-f0)/f0;
      dx = [norm(DP),norm(DR),norm(DT),abs(DF)];
      dxs = [dxs; dx];
%      [cond,det(X)]
%      if (norm(dx)>1e-1) P,P0,Rt(:,1:3),R0,-Rt(:,1:3)'*Rt(:,4),t0,f,f0, end;
%      if (norm(DR)>1) R0,Rt,DR,Rt(:,4),DT,f,f0,DF end;
   end;
   dxs
%   [P,Rt,f,cond] = dlt4(x,X);
%   DP = P/norm(P,1) - P0/norm(P0,1);
%   DR = Rt(:,1:3)'*R0 - eye(3);
%   DT = (Rt(:,1:3)'*Rt(:,4) + t0)/norm(t0);
%   DF = (f-f0)/f0;
%   dx = [norm(DP),norm(DR),norm(DT),abs(DF)]
%   [cond,det(X)]
%   if (norm(dx)>1e-1) P,P0,Rt(:,1:3),R0,-Rt(:,1:3)'*Rt(:,4),t0,f,f0, end;
end;
