%GRÁFICOS DOS PONTOS ALEATÓRIOS GERADOS A PARTIR DO PONTO 488 COM
%DISTRIBUICÃO NORMAL (SIGMA=0.1 e 0.5) E TEMPO DE EXECUCÃO DO MINUS-CHICAGO PARA
%ESSES PONTOS E OS PONTOS 620 E 3389 NAS CÂMERAS 42, 54 E 62

clear all
close all

pt=load('pts488E620E3389.txt'); % matriz das coordenadas dos pts 620, 3389 e 488 nas imagens das 3 câmeras
str={'P1', 'P2','P3','P4','P5','P6','P7', 'P8', 'P9','P10'};

%%%%%%%%%%%    SIGMA=0.1   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
M1=load('MRAND1pt488mar22.txt'); % Matriz dos 10 pontos aleatórios (sigma=0.1) nas imagens das 3 câmeras 
M142=M1(1:3:30,:);
M154=M1(2:3:30,:);
M162=M1(3:3:30,:);

%câmera 42
figure
plot(M142(:,1),M142(:,2), '*','MarkerSize',8);
hold on
%plot(pt(1:2,1),pt(1:2,2), 'o','MarkerFaceColor','r'); %pts 620 e 3389
%text(pt(1:3,1),pt(1:3,2)+5, {'620','3389','488'})
text(pt(3,1),pt(3,2)+0.01, {'488'})
text(M142(:,1),M142(:,2)+0.01,str)
plot(pt(3,1),pt(3,2),'*', 'MarkerSize',10); %pt 488
xlabel('x')
ylabel('y')
title('Imagem do ponto 488 e pontos gerados aleatóriamente a partir dele com distribuicão normal (\sigma=0,1) na câmera 42')
hold off

%câmera 54
figure
plot(M154(:,1),M154(:,2), '*','MarkerSize',8);
hold on
% plot(pt(4:5,1),pt(4:5,2), 'o','MarkerFaceColor','r'); %pts 620 e 3389
% text(pt(4:5,1)-4,pt(4:5,2)+5, {'620','3389'})
text(pt(6,1),pt(6,2)+0.01, {'488'})
text(M154(:,1),M154(:,2)+0.01,str)
plot(pt(6,1),pt(6,2),'*', 'MarkerSize',10); %pt 488
xlabel('x')
ylabel('y')
title('Imagem do ponto 488 e pontos gerados aleatóriamente a partir dele com distribuicão normal (\sigma=0,1), na câmera 54')
hold off

%câmera 62
figure
plot(M162(:,1),M162(:,2), '*','MarkerSize',8);
hold on
% plot(pt(7:8,1),pt(7:8,2), 'o','MarkerFaceColor','r'); %pts 620 e 3389
% text(pt(7:8,1),pt(7:8,2)+5, {'620','3389'})
text(pt(9,1),pt(9,2)+0.01, {'488'})
text(M162(1:4,1),M162(1:4,2)+0.01,str(:,1:4)) %P1 até P4
text(M162(8:10,1),M162(8:10,2)+0.01,str(:,8:10)) %P8 até P10
text(M162(6,1),M162(6,2)+0.01,str(:,6)) %P6
text(M162(5,1)-0.01,M162(5,2)-0.01,str(:,5)) %P5
text(M162(7,1)+0.005,M162(7,2)-0.01,str(:,7)) %P7
plot(pt(9,1),pt(9,2),'*', 'MarkerSize',10); %pt 488
xlabel('x')
ylabel('y')
title('Imagem do ponto 488 e pontos gerados aleatóriamente a partir dele com distribuicão normal (\sigma=0,1), na câmera 62')
hold off


%%%%%%%%%%%    SIGMA=0.5   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
M5=load('MRAND5pt488mar22.txt'); % Matriz dos 10 pontos aleatórios (sigma=0.1) nas imagens das 3 câmeras 
M542=M5(1:3:30,:);
M554=M5(2:3:30,:);
M562=M5(3:3:30,:);

%câmera 42
figure
plot(M542(:,1),M542(:,2), '*','MarkerSize',8);
hold on
% plot(pt(1:2,1),pt(1:2,2), 'o','MarkerFaceColor','r'); %pts 620 e 3389
% text(pt(1:3,1),pt(1:3,2)+5, {'620','3389','488'})
text(pt(3,1),pt(3,2)+0.05, {'488'})
text(M542(:,1),M542(:,2)+0.05,str)
plot(pt(3,1),pt(3,2),'*', 'MarkerSize',10); %pt 488
xlabel('x')
ylabel('y')
% axis equal
title('Imagem do ponto 488 e pontos gerados aleatóriamente a partir dele com distribuicão normal (\sigma=0,5), na câmera 42')
hold off

%câmera 54
figure
plot(M554(:,1),M554(:,2), '*','MarkerSize',8);
hold on
% plot(pt(4:5,1),pt(4:5,2), 'o','MarkerFaceColor','r'); %pts 620 e 3389
% text(pt(4:5,1)-4,pt(4:5,2)+5, {'620','3389'})
text(pt(6,1),pt(6,2)-0.05, {'488'})
text(M554(:,1),M554(:,2)+0.05,str)
plot(pt(6,1),pt(6,2),'*', 'MarkerSize',10); %pt 488
xlabel('x')
ylabel('y')
title('Imagem do ponto 488 e pontos gerados aleatóriamente a partir dele com distribuicão normal (\sigma=0,5), na câmera 54')
hold off

%câmera 62
figure
plot(M562(:,1),M562(:,2), '*','MarkerSize',8);
hold on
% plot(pt(7:8,1),pt(7:8,2), 'o','MarkerFaceColor','r'); %pts 620 e 3389
% text(pt(7:8,1),pt(7:8,2)+5, {'620','3389'})
text(pt(9,1),pt(9,2)+0.05, {'488'})
text(M562(1:10,1),M562(1:10,2)+0.03,str(:,1:10))
% text(M562(1:2,1),M562(1:2,2)+0.05,str(:,1:2))
plot(pt(9,1),pt(9,2),'*', 'MarkerSize',10); %pt 488
xlabel('x')
ylabel('y')
title('Imagem do ponto 488 e pontos gerados aleatóriamente a partir dele com distribuicão normal (\sigma=0,5), na câmera 62')
hold off


% ------- boxplot --------

str488={'P1', 'P2','P3','P4','P5','P6','P7', 'P8', 'P9','P10','488'};
timesol1=load('timesolutionRAND1pt488mar22.txt');
timesol5=load('timesolutionRAND5pt488mar22.txt');
timesol488=load('timesolution488mar22.txt');



%%%%%%   sigma=0,1  %%%%%%%%%%%%%%%%
for j=1:10
    t1(:,j)=timesol1(1000*j-999:1000*j,2);
    m1(j)=mean(t1(:,j)); 
end
t1(:,11)=timesol488(:,2);
m1(11)=mean(t1(:,11));

figure
boxplot(t1,str488)
hold on
plot(m1, 'o')
legend('Tempo médio')
xlabel('Pontos')
ylabel('Tempo de execucão do solver (ms)')
title('Tempo de execucão do minus-chicago para os pontos 620, 3389 (fixos) e o ponto 488 e pontos aleatórios gerados a partir dele com distribuicão normal (\sigma=0,1)')
hold off

%%%%%% sigma=0,5  %%%%%%%%%%%%%%%%%%%

for k=1:10
    t5(:,k)=timesol5(1000*k-999:1000*k,2);
    m5(k)=mean(t5(:,k)); 
end
t5(:,11)=timesol488(:,2);
m5(11)=mean(t5(:,11));

figure
boxplot(t5,str488)
hold on
plot(m5, 'o')
legend('Tempo médio')
xlabel('Pontos')
ylabel('Tempo de execucão do solver (ms)')
title('Tempo de execucão do minus-chicago para os pontos 620, 3389 (fixos) e o ponto 488 e pontos aleatórios gerados a partir dele com distribuicão normal (\sigma=0,5)')
hold off

