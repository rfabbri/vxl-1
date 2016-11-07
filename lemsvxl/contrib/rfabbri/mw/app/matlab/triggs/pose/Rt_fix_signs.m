% [Rts] = Rt_fix_signs(Rts,x1,x2,Rthresh,tthresh)
% Run through a series of (R,t) pairs representing possible
% transformations between two calibrated cameras (e.g. estimated from an
% essential matrix or a plane homography), eliminating ones that
% represent twisted pairs and correcting the sign of the remaining
% translations.  
%
% Method: Corresponding visible image points x1,x2 must have positive
% depths z1,z2>0 in each camera. If P1=(I|0) and P2=R(I|-t) then z2.x2 =
% R*(z1.x1 - t), i.e. solving x1 = a*t + b*(R'*x2) should give positive
% a,b. The quadratic constraints below express this. We count the number
% of inlying point-pairs, flip the sign of t if necessary, and reject
% anything that still isn't right for most of the points.

function [Rt] = Rt_fix_signs(Rts,x1,x2,Rthresh,tthresh)
   n = size(x1,2);
   Rt = [];
   for i=1:4:size(Rts,2)
      R = Rts(1:3,i:i+2); 
      t = Rts(1:3,i+3); 
      tt = t'*t;
      rok = 0; tok = 0;
      for p=1:n
	 x = x1(:,p); y = R'*x2(:,p);
	 tx = t'*x; 
	 ty = t'*y;
	 xy = x'*y;
	 yy = y'*y; 
	 if (tt*xy>tx*ty) rok = rok+1; end;
	 if (tx*yy>ty*xy) tok = tok+1; end;
      end;
      if (tok<=n/2) Rts(1:3,i+3) = -t; tok = n-tok; end;
      if (rok>=n*Rthresh & tok>=n*tthresh) 
	 if (isempty(Rt)) Rt = Rts(:,i:i+3); else Rt = [Rt, Rts(:,i:i+3)]; end;
      end;   
   end;
%end;
