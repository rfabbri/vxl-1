% minus-chicago (câmeras 42, 54 e 62) para os pontos 620, 3389 e pontos obtidos a partir dos pontos da curva 8 subtraídos 100 pixel da
% coordenada y. 13/01/22
clear all
close all

cv8fr42=load("cv8fr42MENOS100y.txt");
cv8fr54=load("cv8fr54MENOS100y.txt");
cv8fr62=load("cv8fr62MENOS100y.txt");

%subtraindo 100 da coordenada y
cv8fr42(:,2)=cv8fr42(:,2)-100;
cv8fr54(:,2)=cv8fr54(:,2)-100;
cv8fr62(:,2)=cv8fr62(:,2)-100;

writematrix(cv8fr42, 'cv8fr42-menos100y.txt','Delimiter','tab');
writematrix(cv8fr54, 'cv8fr54-menos100y.txt','Delimiter','tab');
writematrix(cv8fr62, 'cv8fr62-menos100y.txt','Delimiter','tab');


