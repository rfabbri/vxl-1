function [pt3d, normal, conf] = recover3d(PT, PN, dPTdv, dPNdv)

A = [PT PN dPTdv dPNdv]';
[U,D,V] = svd(A);
pt3d_homo = V(:,4);
pt3d = pt3d_homo(1:3)/pt3d_homo(4);
conf = diag(D);
normal = PT(1:3);
normal = normal/norm(normal);
