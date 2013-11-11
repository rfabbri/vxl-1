% Tests according to my (*) note of Wed Nov  5 23:07:44 EST 2008

mypath='/home/rfabbri/lib/data/oxford/corridor/';

P1orig = load([mypath 'bt.000.P']);
P2orig = load([mypath 'bt.004.P']);
P1orig = P1orig/norm(P1orig);
P2orig = P2orig/norm(P2orig);

[P1M, P2M] = fabbri_world_to_1st_cam(P1orig, P2orig);
K1 = P1M(1:3,1:3);

F = vgg_F_from_P(P1M, P2M);

[P1, P2] = GetPmatrixFromFmatrix(F);

% Sanity check
F_tst = vgg_F_from_P(P1, P2);
F/norm(F), F_tst/norm(F_tst)

% Now we can find a matrix H

a2 = P2(:,4);
lambda=a2'*P2M(:,4);
lambda=lambda/(a2'*a2);
P2 = P2*lambda;

M = P2M(1:3,1:3) - P2(1:3,1:3)*K1;

v_candidate1 = [1 0 0]*M/P2(1,4)
v_candidate2 = [0 1 0]*M/P2(2,4)
v_candidate3 = [0 0 1]*M/P2(3,4)

H = [K1 zeros(3,1); v_candidate3 1];
%H = rand(4,4);

% Now we can apply H to P1 and P2 to get P1M
P1Mc = P1*H;
P2Mc = P2*H;
%P2M_computed = P2M_computed/norm(P2M_computed);

%P1M_computed = P1M_computed/norm(P1M_computed);
%P2M_computed = P2M_computed/norm(P2M_computed);

P1M, P1Mc
P2M, P2Mc


[K1c, R1c, T1c] = vgg_KR_from_P(P1Mc);
[K1, R1, T1] = vgg_KR_from_P(P1M);
K1, R1, T1
K1c, R1c, T1c

[K2c, R2c, T2c] = vgg_KR_from_P(P2Mc);
[K2, R2, T2] = vgg_KR_from_P(P2M);
K2, R2, T2/norm(T2)
K2c, R2c, T2c/norm(T2c)

% Sanity
%Fc = vgg_F_from_P(P1Mc, P2Mc);

%disp('computed F')
%Fc/norm(Fc)

%disp('original F')
%F/norm(F)
