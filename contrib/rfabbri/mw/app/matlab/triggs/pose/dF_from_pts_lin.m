% [S,e,dF,cond] = dF_from_pts_lin(x,v)
% Linearly estimate the infinitessimal epipolar constraint linking
% points x and image velocities v. The constraint is
%	xi'*(S*xi-cross(e,vi)) = 0 
% Here, e is the epipole and S is the 3x3 symmetric part of the 2nd
% order fundamental matrix dF. (Tensorially, dF = [P,P,dP,dP] where
% dP is the change in projection matrix P). dF can only be recovered up
% to a multiple of [e]_x:  dF = dF1 + mu * mcross(e) where 
% dF1 = (1-e*e') * S * (1+e*e')
% Depth recovery is 
% (d(lambda)/lambda + mu) * cross(e,x) = dF1 * x + cross(e,v)
% The det(F)=0 constraint becomes e'*S*e = 0. With noise, the above form
% for dF1 guarantees projection onto the constraint for dF1 and
% S1=(dF1+dF1')/2, i.e. it effectively enforces e'*S*e=0 by changing S
% for fixed e.
%

function [S,e,dF,cond] = dF_from_pts_lin(x,v)
   [Ses,cond] = dFspace_from_pts(x,v,8);
   S = Ses(:,1:3);
   e = Ses(:,4);
   cond = [e'*S*e / norm(S,1); cond];
   T = (S - e * (e'*S));
   dF = T + (T*e) * e';
   S = (dF + dF')/2;
%end;
