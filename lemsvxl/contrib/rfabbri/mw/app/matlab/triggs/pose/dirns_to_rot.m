% [R,cond,signs] = dirns_to_rot(Y,X) -- find the rotation that best maps a
% set of unit vectors X into another set Y, with optional sign flips if
% required.

% Input: two matrices of column vectors, such that Y ~= R * X for some
% proper rotation R, modulo a sign change of each vector in Y.

% Output: An estimate of R and the numerical conditioning, and
% optionally a row vector of sign flips for Y such that Y * diag(signs)
% ~= R*X. If the vectors are all close to a codimension>=2 subspace, the
% local conditioning is poor (cond<<1).  If sign flips are permitted and
% the vectors are close to certain highly symmetrical configurations,
% there may be multiple near-global solutions and only a local solution
% is guaranteed.

% Method: Once the signs are known, we have to find R minimizing
% |Y-R*X|^2 = Tr(Y*Y') + Tr(X*X') - 2*Tr(Y*X'*R'). (Here we use the
% invariance of trace Tr() under transpose and cyclic permutations).
% The first two terms are constant and the last is 2*Tr(S*R1') where
% U*S*V' is the SVD of Y*X' and R1 = U'*R*V. Since S is diagonal
% positive, the solution is the R1 with maximal positive weight on the
% diagonal, i.e. R1 = I, R = U*V'.

% To find the signs we compare the QR decompositions of X and Y, since
% these are almost independent of any rotation R on the left. (Column
% pivoting on X is also used, for stability).  In fact, QR cancels the
% rotation only up to an overall sign for each row, so we have to search
% over row signs as well as column ones. We attack this combinatorial
% problem by relaxation, implicitly flipping the sign of each row in
% turn, choosing the best resulting sign for each column, checking
% whether the overall match-error improves, and if so accepting the sign
% change as an update. This only gives a locally optimal solution, but
% usually a good one.

function [R, cond, signs] = dirns_to_rot(Y,X)
   d = size(X,1);
   if (nargout>2)
      % Find signs by comparing R parts of QR decompositions,
      % with relaxation loop to find best row signs. 
      % W contains cost-function weights up to sign
      [QX,RX,P] = qr(X);
      [QY,RY] = qr(Y*P);
      W = RX .* RY;
      ctot = sum( W );
      tot = sum(abs(ctot));
      for it = 1:3*d
	 flipped = 0;
	 for i = 1:d
	    if ( sum(abs( ctot - 2 * W(i,:) )) > tot )
	       % sign flip of row will reduce error -- accept it
	       W(i,:) = -W(i,:);
	       ctot = sum( W );
	       tot = sum(abs(ctot));
%	       it,i
	       flipped = 1;
	    end;
	 end;
	 if (flipped == 0) break; end;
      end;
      signs = P * sign( ctot' );
      Y = Y * diag(signs);   
   end;

   % Solve for rotation minimizing |Y-R*X|^2 from sign corrected X,Y. 
   % The solution is stable (cond close to 1) iff the last 2 singular
   % values are not too small -- otherwise a rotation mixing these
   % doesn't change the error much.

   [U,S,V] = svd(Y * X');
   R = U * V';
   S = diag(S)'/S(1,1);
   
   % If the rotation is improper, correct it by flipping sign of
   % smallest singular value. But in odd dimensions we can
   % just flip the signs of R and Y without changing the error at all.

   if (det(R)<0) 
      if (nargout>2 & rem(d,2)==1)
	 R = -R; 
	 signs = -signs;
      else
         V(:,d) = -V(:,d); 
	 R = U * V'; 
      end;
   end;
   cond = [S(d-1),S(d)/S(d-1), norm(Y-R*X,1)];
%end;
