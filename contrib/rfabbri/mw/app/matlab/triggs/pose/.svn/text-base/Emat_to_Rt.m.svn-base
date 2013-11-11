% [E21,Rts,cond] = Emat_to_Rt(E21)
% Project an estimated essential matrix E21 onto the space of valid E
% matrices and decompose it into a rotation and unit translation vector.
%
% Given projections P1=(I|0) and P2=R(I|-t) the
% essential matrix for the coplanarity constraint x2'*E21*x1=0 is 
% E21 = R.[t]_x where [t]_x = [0,-t_z,t_y; t_z,0,-t_x; -t_y,t_x,0].
% The two corresponding epipoles (optical centre projections) are
% e21 = +t and e12 = -R.t.
%
% Method: SVD E12 and project it onto the space of valid essential
% matrices by enforcing two equal and one zero singular
% values. For any valid E there is a `twisted pair' of solutions
% for R, and t can only be recovered up to scale and sign. We
% return both possibilities and leave Rt_fix_signs() to figure out
% which is right.

function [E21,Rts,cond] = Emat_to_Rt(E21)
   [U,S,V] = svd(E21);
   E21 = U*diag([1,1,0])*V';
   Ra = U*[0,1,0; -1,0,0; 0,0,1]*V';
   Rb = U*[0,-1,0; 1,0,0; 0,0,1]*V';
   if (det(Ra)<0) Ra = -Ra; end;
   if (det(Rb)<0) Rb = -Rb; end;
   t = V(:,3);
   Rts = [Ra,t, Rb,t];
   cond = [S(2,2)/S(1,1); S(3,3)/S(1,1)];
%end;
