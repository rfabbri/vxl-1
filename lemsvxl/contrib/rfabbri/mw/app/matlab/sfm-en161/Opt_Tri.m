function [X, P1, P2, hat_leftpoint, hat_rightpoint]=OptimalTriangulation(lpoint, rpoint, F_input)
% [X, P1, P2, hat_leftpoint, hat_rightpoint]=...
%                            OptimalTriangulation(lpoint, rpoint, F_input,w,h)
% Given a measured point correspondence x to xd, and a fundamental
% matrix F_input, compute the corrected correspondences hat_x to hat_xd that
% minimize the geometric error (11.1) subject to the epipolar constraint
% hat_xd'*F*hat_x = 0.
%
% Image Dimensions [w h] used for normalizing points and F matrix.
% Return values X : 3D projective points
%                     P1, P2 : two camera projection matrices
%                     hat_leftpoint, hat_rightpoint : estimated image points

leftpoint  = lpoint;
rightpoint = rpoint;
F          = F_input;

DEBUG = 0;
[m,n]=size(leftpoint);

[P1, P2]=GetPmatrixFromFmatrix(F); % P1, P2
hat_leftpoint=[]; % hat_x a��
hat_rightpoint=[]; % hat_xd a��

% �˰?��
for i=1:n
    
x=leftpoint(1,i);
y=leftpoint(2,i);
xd=rightpoint(1,i);
yd=rightpoint(2,i);

% �̹��� a��; ��a8�� �̵�
T=[1 0 -x;
       0 1 -y;
       0 0 1];
Td=[1 0 -xd;
       0 1 -yd;
       0 0 1];

% T�� �̿��� ��ο� F�� ��ȯ
% F=Td^{-T}*F*T^{-1}
F0=inv(Td)'*F*inv(T);

% �� ������ ���.
[ep1, ep2]=epipole(F0);
%ed=null(F0');
ed=ep2/ep2(3);
%e=null(F0);
e=ep1/ep1(3);

% ������ normalize : e(1)*e(1)+e(2)*e(2)=1 �� �ǵ���.
ed_scale=sqrt(ed(1)*ed(1)+ed(2)*ed(2));
ed=ed/ed_scale;
e_scale=sqrt(e(1)*e(1)+e(2)*e(2));
e=e/e_scale;

% ������; x��8�� ȸ���ϴ� R����
R=[e(1) e(2) 0;
   -e(2) e(1) 0;
     0     0     1];
 Rd=[ed(1) ed(2) 0;
      -ed(2) ed(1) 0;
        0       0       1];

% R*e=(1, 0, e3)^T, Rd*ed=(1, 0, ed3)^T ���
tmp_e=R*e;
e3=tmp_e(3);
tmp_ed=Rd*ed;
ed3=tmp_ed(3);

% F�� R; �̿��� ��ȯ   
% F=Rd*F*R^{T}
F0=Rd*F0*R'
x
y

inv(T)*R'

fprintf('The transformed F-matrix for Point pair %d',i);
F0

% f=e3, fd=e3d, a=F22, b=F23, c=F32, d=F33
f=e3;
fd=ed3;

a=F0(2,2)
b=F0(2,3)
c=F0(3,2)
d=F0(3,3)
f
fd

pause

% g(t)�Ŀ���, t�� ��; ����.

% g(t)=t((at+b)^2+fd^2(ct+d)^2)^2-(ad-bc)(1+fd^2t^2)^2(at+b)(ct+d)
% g(t)=t*((a*t+b)^2+fd^2*(c*t+d)^2)^2-(a*d-b*c)*(1+f^2*t^2)^2*(a*t+b)*(c*t+d)
% ��; t�� ���� ���׽�8�� Ǯ�.
% g(t)=
% -(a*d-b*c)*f^4*a*c*t^6
% +((a^2+fd^2*c^2)^2-(a*d-b*c)*f^4*b*c-(a*d-b*c)*f^4*a*d)*t^5
% +(2*(2*b*a+2*fd^2*d*c)*(a^2+fd^2*c^2)-2*(a*d-b*c)*f^2*a*c-(a*d-b*c)*f^4*b*d)*t^4
% +(-2*(a*d-b*c)*f^2*b*c-2*(a*d-b*c)*f^2*a*d+2*(b^2+fd^2*d^2)*(a^2+fd^2*c^2)+(2*b*a+2*fd^2*d*c)^2)*t^3
% +(-(a*d-b*c)*a*c-2*(a*d-b*c)*f^2*b*d+2*(b^2+fd^2*d^2)*(2*b*a+2*fd^2*d*c))*t^2
% +((b^2+fd^2*d^2)^2-(a*d-b*c)*b*c-(a*d-b*c)*a*d)*t
% -(a*d-b*c)*b*d

% (b*c^2*f^4*a-a^2*d*f^4*c)*t^6
% +(b^2*c^2*f^4+a^4+2*a^2*fd^2*c^2+fd^4*c^4-a^2*d^2*f^4)*t^5
% +(4*fd^4*c^3*d+4*a*b*fd^2*c^2+4*a^2*fd^2*c*d+4*a^3*b-a*d^2*f^4*b-2*a^2*d*f^2*c+2*b*c^2*f^2*a+b^2*c*f^4*d)*t^4
% +(2*a^2*fd^2*d^2+6*fd^4*c^2*d^2+6*a^2*b^2+8*a*b*fd^2*c*d-2*a^2*d^2*f^2+2*b^2*c^2*f^2+2*b^2*fd^2*c^2)*t^3
% +(-2*a*d^2*f^2*b+4*fd^4*c*d^3+4*a*b*fd^2*d^2-a^2*d*c+4*b^2*fd^2*c*d+4*a*b^3+b*c^2*a+2*b^2*c*f^2*d)*t^2
% +(b^2*c^2+b^4-a^2*d^2+fd^4*d^4+2*b^2*fd^2*d^2)*t
% -a*d^2*b+b^2*c*d


% gPoly=[-(a*d-b*c)*f^4*a*c,
%  +((a^2+fd^2*c^2)^2-(a*d-b*c)*f^4*b*c-(a*d-b*c)*f^4*a*d),
%  +(2*(2*b*a+2*fd^2*d*c)*(a^2+fd^2*c^2)-2*(a*d-b*c)*f^2*a*c-(a*d-b*c)*f^4*b*d),
%  +(-2*(a*d-b*c)*f^2*b*c-2*(a*d-b*c)*f^2*a*d+2*(b^2+fd^2*d^2)*(a^2+fd^2*c^2)+(2*b*a+2*fd^2*d*c)^2),
%  +(-(a*d-b*c)*a*c-2*(a*d-b*c)*f^2*b*d+2*(b^2+fd^2*d^2)*(2*b*a+2*fd^2*d*c)),
%  +((b^2+fd^2*d^2)^2-(a*d-b*c)*b*c-(a*d-b*c)*a*d),
%  -(a*d-b*c)*b*d];

gPoly=[(b*c^2*f^4*a-a^2*d*f^4*c),
+(b^2*c^2*f^4+a^4+2*a^2*fd^2*c^2+fd^4*c^4-a^2*d^2*f^4),
+(4*fd^4*c^3*d+4*a*b*fd^2*c^2+4*a^2*fd^2*c*d+4*a^3*b-a*d^2*f^4*b-2*a^2*d*f^2*c+2*b*c^2*f^2*a+b^2*c*f^4*d),
+(2*a^2*fd^2*d^2+6*fd^4*c^2*d^2+6*a^2*b^2+8*a*b*fd^2*c*d-2*a^2*d^2*f^2+2*b^2*c^2*f^2+2*b^2*fd^2*c^2),
+(-2*a*d^2*f^2*b+4*fd^4*c*d^3+4*a*b*fd^2*d^2-a^2*d*c+4*b^2*fd^2*c*d+4*a*b^3+b*c^2*a+2*b^2*c*f^2*d),
+(b^2*c^2+b^4-a^2*d^2+fd^4*d^4+2*b^2*fd^2*d^2),
-a*d^2*b+b^2*c*d];
 
disp('Polynomial Coefficients');
for i=1:length(gPoly)
    disp(gPoly(i))
end;

disp(' ');

sol_t=roots(gPoly');
%for i=1:length(sol_t)
%    disp(sol_t(i))
%end
sol_t

% s(t)=2t/(1+f^2t^2)^2 + (ct+d)^2/((at+b)+fd^2(ct+d)^2)
% s(t)�� �ؼҰ�; ����� t �� ã�´�.
min_st=1e+100;
for i=1:length(sol_t)
    t=real(sol_t(i));
    st=(t^2)/(1+f^2*t^2)^2 + (c*t+d)^2/((a*t+b)^2+fd^2*(c*t+d)^2);
    if st<min_st 
        min_st=st;
        min_t=t;
    end;
end;
% min_st : s(t)�� �ؼҰ�
% min_t : �ؼҰ�; ����� t

% t->���Ѵ�� �Ͽ�;�� s(t)�� �ؼҰ����� Ȯ��
% if t->���Ѵ�, s(t)=1/f^2+c^2/(a^2+fd^2*c^2)
if (1/f^2+c^2/(a^2+fd^2*c^2))<min_st
% DEBUG
%     disp('s(t)���� t�� ���Ѵ��� ��� ����.')
    min_st=1/f^2+c^2/(a^2+fd^2*c^2);
    min_t=1e+10;
end;

min_t

% DEBUG
% disp('s(t)�� �ؼҰ�=');
% disp(min_st);
% disp('at t=');
% disp(min_t);

%���׿� �׽�Ʈ
%Xplot=[];
%for t=-1000000:100:1000000
%    st=(2*t)/(1+f^2*t^2)^2 + (c*t+d)^2/((a*t+b)^2+fd^2*(c*t+d)^2);
%    Xplot=cat(1,Xplot,st);
%end;
%plot(-1000000:100:1000000,Xplot)

% �� ���� �� ���
% l=(t*f,1,-t), ld=(-fd*(c*t+d),a*t+b,c*t+d)
t=min_t;

l=[t*f,1,-t]; ld=[-fd*(c*t+d),a*t+b,c*t+d];
% ���� ����� a�� ���, �� hat{x}, hat{x'} ���
hat_x=find_ClosePointOnLine_FromOrigin(l);
hat_x=hat_x/hat_x(3);
hat_xd=find_ClosePointOnLine_FromOrigin(ld);
hat_xd=hat_xd/hat_xd(3);

disp('epipolar line 1');
l


pause

% ��ǥ�� ����
% T^{-1}*R^{T}*hat_x, Td^{-1}*Rd^{T}*hat_xd
hat_x=inv(T)*R'*hat_x;
hat_x=hat_x/hat_x(3);
hat_xd=inv(Td)*Rd'*hat_xd;
hat_xd=hat_xd/hat_xd(3);

% hat_x �� hat_xd �� ����.
hat_leftpoint=cat(2,hat_leftpoint,hat_x);
hat_rightpoint=cat(2,hat_rightpoint,hat_xd);

end; % end of for each points

% Linear Triangulation ���8�� 3��� a hat{X} ���
X=Triangulation_Linear(P1, P2, hat_leftpoint, hat_rightpoint);

% X�� ���� ��� 1�� ����
[row_X, col_X]=size(X);
for i=1:col_X
    X(:,i)=X(:,i)/X(4,i);
end

% ��� ����
% Minimization ���� Ȯ��
sum=0;
for i=1:n
    x1=hat_leftpoint(1,i)-leftpoint(1,i);
    x2=hat_leftpoint(2,i)-leftpoint(2,i);
    xd1=hat_rightpoint(1,i)-rightpoint(1,i);
    xd2=hat_rightpoint(2,i)-rightpoint(2,i);
    sum=sum+(x1*x1+x2*x2)+(xd1*xd1+xd2*x2);
end;
sum=sum/n;
if (DEBUG == 1)
    disp('Minimization error:'); disp(sum);
end

% Triangulation ���� Ȯ��
if (DEBUG == 1)
    disp('Triangulation error with hat_x:');
    check_Triangulation(P1, P2, hat_leftpoint, hat_rightpoint, X)
    disp('Triangulation error with original x:');
    check_Triangulation(P1, P2, leftpoint, rightpoint, X)

    % Fudamental matrix ���� Ȯ��
    disp('Fundamental matrix error, x''Fx=0:');
    check2_Fmatrix(F, hat_leftpoint, hat_rightpoint);
    disp('Fundamental matrix error, epipolar line distance:');
    check3_Fmatrix(F, hat_leftpoint, hat_rightpoint);
end


function point=find_ClosePointOnLine_FromOrigin(line)
lambda=line(1); mu=line(2); v=line(3);
point=[-lambda*v; -mu*v; lambda^2+mu^2];


