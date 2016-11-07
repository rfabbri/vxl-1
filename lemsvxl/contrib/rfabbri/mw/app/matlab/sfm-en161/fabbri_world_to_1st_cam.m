function [P1_out, P2_out] = fabbri_world_to_1st_cam(P1,P2)
% Outputs two camera matrices such that the world coordinate system is placed in
% the first one, P1_out. Therefore, P1_out = K_1 [I | 0].

% Decompose cameras

[K1,R1, T1] = vgg_KR_from_P(P1);
P1_out = K1*[eye(3,4)];

R1 = R1';
E = [R1  -R1*T1; 0 0 0 1];
P2_out = P2*E;
