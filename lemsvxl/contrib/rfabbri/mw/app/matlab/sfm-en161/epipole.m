function [ep1, ep2]=epipole(F)
% [e1, e2]=epipole(F12)
% compute two epipoles e1 and e2 from the fundamental matrix F12
e1d=F(2,3)*F(1,2)-F(2,2)*F(1,3);
e2d=F(1,3)*F(2,1)-F(1,1)*F(2,3);
e3d=F(2,2)*F(1,1)-F(2,1)*F(1,2);
ep1=[e1d; e2d; e3d];

e1=F(3,2)*F(2,1)-F(2,2)*F(3,1);
e2=F(3,1)*F(1,2)-F(1,1)*F(3,2);
e3=F(2,2)*F(1,1)-F(2,1)*F(1,2);
ep2=[e1; e2; e3];
