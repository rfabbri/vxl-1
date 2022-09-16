% Gerar configuracões de câmeras e pontos para input do minus-chicago, a
% partir dos dados sintéticos - ABR 2022

clear all
close all

M(1:1000,1:3)= randi([0,99],1000,3);
M(1:1000,7)=0;
M(1:1000,8)=1;
M(1:1000,4:6)=randi([1,5117],1000,3);
writematrix(M, 'FRePT.txt', 'Delimiter', 'space');