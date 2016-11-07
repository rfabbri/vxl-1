function [mtots,merrors] = run_dlt(trials,n,noise,flat,refine,methods)
   methnames = ['lin4';'eig4';'mlin4';'---';'lin5';'lin6';'plan4'];
   methns = [4,4,4,0,5,6,4];
   refinename = [' ';'r'];
   im_width = 512;
   if (nargin<1) trials = 10; end;
   if (nargin<2) n = 4; end;
   if (nargin<3) noise = 0; end;
   if (nargin<4) flat = 1e0; end;
   if (nargin<5) refine = 1; end;
   if (nargin<6) methods = [1,2,5,6,7]; end;
   % noise = 1e-30;
   % flat = 1e-0;
   % n = 4;
   % trials = 3;
   % methods = [1,2,3,5,6,7];
   % refine = 0;
   % bias=[0,0];
   rescale = 1;
   inoise = noise/im_width * rescale; % noise in rescaled coords
   fref = 2 * rescale;	% f0 (N*im_width) in rescaled coords
   Roff = quat_to_rot([0.25; 0.25; 0; 1]);
   drot = 0.2;
   t0 = [0;0;-10];
   merrors = zeros(trials,7*20);
   for trial = 1:trials
      f0 = fref*exp(0.3*randn(1));
      K0 = diag([f0,f0,1]);
      R0 = quat_to_rot([drot*randn(3,1);1]) * Roff;
      P0 = K0*[R0, -t0];
      X = [[-1,-1,1,1;-1,1,-1,1;1,-1,-1,1],0.7*randn(3,n-4); ones(1,n)];
      % X = [randn(3,n); ones(1,n)];
      % X = [eye(3),ones(3,1),randn(3,n-4); ones(1,n)];
      X = diag([1,1,flat,1]) * X;
      x = P0*X;
      x = x ./ (ones(3,1)*x(3,:)) + inoise*[randn(2,n);zeros(1,n)];
      % [max(x'),min(x')]
      for meth = methods
	 if ((meth<=4 | meth==49 | meth==84) & n>=4)		% 4 pt lin and mmat methods
	    [Ps,Rts,fs,cond] = dlt4(x,X,meth,(refine>0))
	 elseif (meth==5 & n>=5)	% 5 pt lin method
	    [Ps,K,Rts,cond] = dlt5(x,X);
	    fs = [K(1,1)];
	 elseif (meth==6 & n>=6)	% 6 pt DLT
	    [Ps,cond] = proj_from_pts_lin(x,X);
	    % RQ factorize P(:,1:3)=K*R
	    D = fliplr(eye(3));
	    [R,K] = qr(D*Ps(:,1:3)'*D);
	    R = D*R'*D;
	    K = D*K'*D;
	    D1 = diag(sign(diag(K)));
	    K = K*D1;
	    R = D1*R;
	    Rts = inv(K)*Ps;
	    K = K/K(3,3);
	    fs = sqrt(abs(K(1,1)*K(2,2)));
	 elseif (meth==7 & n>=4)	% 4 pt planar f method (1 plane only)
	    Xplanar = X([1,2,4],:);
	    Kfree = [1,0,0,0,0];
	    Kref = [fref,0,1,0,0];
	    [K,Hs,Rts,cond] = dltplanar(x,Xplanar,[1,n],[Kref;Kfree]);
	    Ps = K*Rts;
	    fs = K(1,1);
	 else
	    if (trial==1)
	       if (n<methns(meth))
		  warning('too few points for method %s',methnames(meth,:));
	       else
		  warning('unknown method %d',meth);
	       end;
	    end;
	    continue;
	 end;

	 errs = [size(fs,2),1e10*ones(1,6)];
	 for i = 1:size(fs,2)
	    P = Ps(:,4*i-3:4*i);
	    Rt = Rts(:,4*i-3:4*i);
	    f = fs(i);
	    dP = P/norm(P,1) - P0/norm(P0,1);
	    dR = Rt(:,1:3)'*R0 - eye(3);
	    % dt = (Rt(:,1:3)'*Rt(:,4) + t0)/norm(t0);
	    dt = (-Rt(:,4) - t0)/norm(t0);
	    df = (f-f0)/f0;
	    ndR = norm(dR);
	    if (ndR < errs(3))
	       errs(2:7) = [norm(dP),ndR,norm(dt),abs(df), t0'*dt/norm(t0),df];
	       % bias = bias + [t0'*dt/norm(t0),df];
	       % i,errs
	    end;
	 end;
	 merrors(trial,7*meth-6:7*meth) = errs;
      end;
   end;
   mtots = zeros(size(methods,2),12);
   mtots(:,1) = methods';
   for m = 1:size(methods,2)
      meth = methods(m);
      #if (n<methns(meth)) continue; end;
      merr = merrors(:,7*meth-6:7*meth);
      ns = merr(:,1);
      mmed = 1e10*ones(4,1);
      for i=2:7
	 if (trials>1) 
	    me = merr(find(merr(:,i)<1e10),i);
	    if (size(me,1)>0) mmed(i-1) = median(me); end;
	 else
	    me = merr(i);
	    mmed(i-1) = me;
	 end;
      end;
      if (trials>1)
	 se = sum([(merr(:,1)*ones(1,5))==(ones(trials,1)*[0:4])])/trials;
      else
	 se = (merr(1)==[0:4]);
      end;
      mtots(m,:) = [meth,mmed',se];
      fprintf(1,'n=%d trials=%d noise=%g flat=%g %s%s dP=%g dR=%g dt=%g df=%g bdt=%g bdf=%g nsol=%g,%g,%g,%g,%g\n',...
              n,trials,noise,flat, ...
'','49',...#	      refinename(refine+1,:),methnames(meth,:), ...
	      100*mmed,100*se);
   end;
   % 100*bias/trials
#end;
