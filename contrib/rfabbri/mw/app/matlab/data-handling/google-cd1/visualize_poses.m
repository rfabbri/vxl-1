close all; clear;
dir_name = 'jiajun_matches/'; 

dir_list = dir(dir_name);
% pose between 00000_00.jpg and 00000_01.jpg
TT = readPoses([dir_name 'jana-pose.txt']);

plotting = 1;  % visualize matches 
figure(400); hold on; axis equal; box on;
xlabel('x');
ylabel('y');
zlabel('z');
scale = 10;
% draw_frame_scaled_cb([diag([1 1 1]), zeros(3,1)],scale,0);
% text(0,0,0,'0');
k = 0;
for i = 1:3:length(TT)
    % trans = -TT(i).pose(1:3,1:3,1)'*TT(i).pose(1:3,4);
    % rot = -TT(i).pose(1:3,1:3,1)';
    TT(i).name(1);
    for j = 1 % :2:8
       k = k+1;
       trans = TT(i).pose(1:3,4);
       Rot = TT(i).pose(1:3,1:3,j);
       [alpha(k), beta(k), gamma(k)] = rotMatrixEuler(Rot);
       text(trans(1)+2, trans(2)+2, trans(3)+2, num2str(i));
       colorRand = []; % take the default color
       figure(400);
       draw_frame_scaled_cb([Rot trans],scale,1, colorRand);
    end
end;

sc = (pi/20)*180/pi;
degc = 180/pi;
figure(401); 
subplot(131); hold on; plot([1:k],degc*abs(alpha)); plot([1:k],degc*abs(alpha),'.'); ...
    mx = mean(degc*abs(alpha)); axis([0 k -sc+mx sc+mx]);
title('roll-x');
subplot(132); hold on; plot([1:k],degc*abs(beta)); plot([1:k],degc*abs(beta),'.'); ...
    my = mean(degc*abs(beta)); axis([0 k -sc+my sc+my]);
title('pitch-y');
subplot(133); hold on; plot([1:k],degc*abs(gamma)); plot([1:k],degc*abs(gamma),'.'); ...
    mz = mean(degc*abs(gamma)); axis([0 k -sc+mz sc+mz]);
title('yaw-z');       

      
