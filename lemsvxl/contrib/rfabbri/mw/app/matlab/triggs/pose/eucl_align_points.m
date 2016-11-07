% [R,t,cond,scale] = eucl_align_points(Y,X,proper)
% Find Euclidean transform X -> scale*R*X+t that best aligns a 
% d-dimensional point set X to point set Y. 
% If proper>=0, improper rotations (det=-1) are forbidden.
%
% Method: With centred data DY,DX, if DY ~ scale*R*DX then 
% YX ~ scale*R*X2  where YX and X2 are the dxd Y-X and X-X
% scatter matrices. Hence scale*R ~ YX*inv(X2) and we can 
% find the result by SVD. 

function [R,t,cond,scale] = eucl_align_points(Y,X,proper)
   if (nargin<3) proper=0; end;
   [d,n] = size(X);
   % Centre data and find scatter matrices
   xbar = sum(X')'/n;
   ybar = sum(Y')'/n;
   DX = X - xbar * ones(1,n);
   DY = Y - ybar * ones(1,n);
   % X2 = DX * DX';
   % YX = DY * DX';
   % [U,S,V] = svd(YX*inv(X2));
   % A = DY / DX;
   [U,S,V] = svd(DY * pinv(DX,1e-10*norm(DX,1)));   
   R = U * V';
   if (nargout>3) scale = sum(diag(S))/d; else scale = 1; end;
   if (proper) 
      if (det(R)<0)
	 % flipping smallest singular value gives smallest error
	 R = U * diag([ones(1,d-1),-1]) * V';
      end; 
   end;
   t = ybar - scale*R*xbar;
   if (nargout>2) SYX = svd(DY*DX'); cond = [SYX(d)/SYX(1)]; end;
%end;
