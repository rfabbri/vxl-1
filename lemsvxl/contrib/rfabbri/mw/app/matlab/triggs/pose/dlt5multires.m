% 8x8 resultant matrix for 5 point DLT-like pose method.
% See comments in dlt5() routine.

function [u,cond] = dlt5multires(M);

   A = [M, zeros(2,3);...
	zeros(2,1), M, zeros(2,2);...
	zeros(2,2), M, zeros(2,1);...
	zeros(2,3), M ];

   [U,S,V] = svd(A);
   S = diag(S)';
   cond = [S(7)/S(1), S(8)/S(7)];
   v = V(:,8);
   % Find ratio u1/u2 from largest end of v.
   % v is [u1^7,u1^6*u2,...,u2^7] up to scale.
   if (norm(v([1,2])) >= norm(v([7,8])))
      u = v([1,2]);
   else
      u = v([7,8]);
   end;
end;
