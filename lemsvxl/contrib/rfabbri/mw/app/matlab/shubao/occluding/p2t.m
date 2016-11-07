function [un,vn] = p2t(H,u,v);   
% P2T apply a Projective 2D Transform

% H is an homography of the projective plane 

dime = size(u,1);
 
c3d = [u v  ones(dime,1)]';
h2d = H * c3d;
c2d = h2d(1:2,:)./ [h2d(3,:)' h2d(3,:)']';

un = (c2d(1,:))';
vn = (c2d(2,:))'; 


