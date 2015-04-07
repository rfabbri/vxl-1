Gama1 =[0,0,2]';
Tgt1 = [0 1 0]';
Gama2 = [0, 10, 2]';
Tgt2 = [1 0 0.5]';
Tgt2 = Tgt2/norm(Tgt2);

R_gt = eye(3);
T_gt = zeros(3,1);

gama1 = Gama1/Gama1(3);
gama2 = Gama2/Gama2(3);

tgt1 = Tgt1 - Tgt1(3)*gama1;
tgt1 = tgt1/norm(tgt1);

tgt2 = Tgt2 - Tgt2(3)*gama2;
tgt2 = tgt2/norm(tgt2);
