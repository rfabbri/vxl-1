function X=Triangulation_Linear(P1, P2, leftpoint, rightpoint)
% Projective triangulation from two camera matrices P1 and P2
% given its image points by linear solution.

% [leftpoint, T1]=normalize_points(leftpoint);
% [rightpoint, T2]=normalize_points(rightpoint);

% 이미지 점의 개수
[m,n]=size(leftpoint);

% normP1=norm(P1,'fro');
% normP2=norm(P2,'fro');
% P1=P1/normP1;
% P2=P2/normP2;
% [leftpoint, T1]=normalize_points(leftpoint);
% [rightpoint, T2]=normalize_points(rightpoint);

% rows of P1 and P2
p1=P1(1,:)';
p2=P1(2,:)';
p3=P1(3,:)';
p1d=P2(1,:)';
p2d=P2(2,:)';
p3d=P2(3,:)';

% Linear solution
A=[]; X=[];

for i=1:1:n
    x=leftpoint(1,i);
    y=leftpoint(2,i);
    xd=rightpoint(1,i);
    yd=rightpoint(2,i);
%     A=[  x*p3'-p1';
%          y*p3'-p2';
%          xd*p3d'-p1d';
%          yd*p3d'-p2d'
%      ];
    A=[  x*p3'-p1';
         y*p3'-p2';
%          x*p2'-y*p1';
         xd*p3d'-p1d';
         yd*p3d'-p2d'
%          xd*p2d'-yd*p1d';
     ];
    [U,D,V]=svd(A);
    [rowV,colV]=size(V);
    lastColumnV=V(:,colV);
%      lastColumnV=lastColumnV/lastColumnV(4);
    X=cat(2,X,lastColumnV);
%     disp('out--------');
%     A*lastColumnV;
%     norm(lastColumnV)
end;

[row_X, col_X]=size(X);
for i=1:col_X
    X(:,i)=X(:,i)/X(4,i);
end
