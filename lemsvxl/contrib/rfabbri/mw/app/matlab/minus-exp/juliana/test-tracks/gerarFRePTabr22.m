% Gerar configuracões de câmeras e pontos para input do minus-chicago, a
% partir dos dados sintéticos - ABR 2022

clear all
close all
M=[81 63 74 1943 2186 865 0 1];
Mrand(1,:)=[154.82787361402444049 241.32490343742378514];
Mrand(2,:)=[183.9677038356408616 173.54827853530332504];


M(1,4:6)= M(1,4:6)+rand([0,99],1,3);
M(1:1000,7)=0;
M(1:1000,8)=1;
M(1:1000,4:6)=randi([233,5117],1000,3);
writematrix(M, 'FRePT.txt', 'Delimiter', 'space');