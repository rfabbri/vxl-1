% To be run after define_cams.m
% choose a theta and phi

define_cams

npts = 10000;

Kmax = 100;

vtheta1 = zeros(npts,1);
vtheta2 = zeros(size(vtheta1));
vk1 = zeros(size(vtheta1));
vk2 = zeros(size(vtheta1));
VT = zeros(size(vtheta1,1),3);
VN = zeros(size(vtheta1,1),3);

for i=1:npts;
    T = random_t();
    VT(i,:) = T;

    gama1 = F1;

    t1 = project_t(T,F1,gama1);

    gama2 = F2;
    t2 = project_t(T,F2,gama2);

    theta1 = t1'*t_ref1;
    theta2 = t2'*t_ref2;

    vtheta1(i) = theta1;
    vtheta2(i) = theta2;

    N = random_n(T);
    VN(i,:) = N; 

    K = rand()*Kmax;
    vk1(i) = project_k(T,t1,F1,gama1,Gama1,N,K);
    vk2(i) = project_k(T,t2,F2,gama2,Gama2,N,K);
end

vtheta1 = acos(vtheta1);
vtheta2 = acos(vtheta2);

theta_matrix;
