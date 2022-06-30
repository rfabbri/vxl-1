% BOXPLOT DO TEMPO DE EXECUCÃO DO SOLVER PARA PONTOS ALEATÓRIOS (uniformemente distribuídos) COM COORDENADAS a)ENTRE O E 1; b) ENTRE 0 E 1000 E idTANGENTES 0 E 1- 1000 vezes para cada ponto
% GRÁFICO DOS PONTOS ALEATÓRIOS
% EXPERIMENTOS FEITOS EM 14/01/22 - 4 CPUs

clear all
close all

%carregar tempos/pontos
timept=load('timesolutionRAND-JAN22-CORRETO.txt');
timept2=load('timesolutionRANDFEV22.txt');


ptfrA1=[0.792207329559554	0.392227019534168
0.959492426392903	0.655477890177557
0.655740699156587	0.171186687811562];

ptfrB1=[0.0357116785741896	0.706046088019609
0.849129305868777	0.0318328463774207
0.933993247757551	0.27692298496089];

ptfrC1=[0.678735154857773	0.0461713906311539
0.757740130578333	0.0971317812358475
0.743132468124916	0.823457828327293];

ptfrA1000=[118.997681558377	699.076722656686
498.364051982143	890.903252535799
959.743958516081    959.291425205444];

ptfrB1000=[340.385726666133	547.215529963803
585.267750979777	138.624442828679
223.811939491137	149.294005559057];

ptfrC1000=[751.267059305653	257.508254123736
255.095115459269	840.717255983663
505.957051665142	254.28217897153];

% format shortG
% id ponto
A(1,1)=1;
A(2,1)=1000;

% Tempo médio do solver para cada ponto e distância do ponto ao ponto 3011
for i=1:2
    A(i,2)=mean(timept(1000*i-999:1000*i,2));  % tempo médio
    A(i,3)=median(timept(1000*i-999:1000*i,2));  % tempo mediano
end
for j=1:2
    t(:,j)=timept(1000*j-999:1000*j,2);
end


str={'A', 'B'}; %caso A: coord entre 0 e 1; caso B: coord entre 0 e 1000
figure      %Boxplot com os pontos obedecendo a posicão relativa deles em relacão à reta r

boxplot(t, str)
xlabel('Caso')
ylabel('Tempo (ms)')
hold on
plot(A(:,2),'o') % marcacão das médias
hold off
title('Tempo de execucão minus-chicago com os pontos 620, 3389 e pontos aleatórios (4 CPUS)')
legend('Tempo médio')

figure
plot(ptfrA1(:,1),ptfrA1(:,2), '*', 'Color','b', 'MarkerSize', 15);
hold on
plot(ptfrB1(:,1),ptfrB1(:,2), '*', 'Color','r','MarkerSize', 15);
plot(ptfrC1(:,1),ptfrC1(:,2), '*', 'Color','m','MarkerSize', 15);
title('Imagem dos 3 pontos aleatórios com coordenadas entre 0 e 1 nas três câmeras')
legend('Câmera A', 'Câmera B', 'Câmera C')
xlabel('x')
ylabel('y')
str={'P1', 'P2', 'P3'}
text(ptfrA1(:,1)+0.015, ptfrA1(:,2), str)
text(ptfrB1(:,1)+0.015, ptfrB1(:,2), str)
text(ptfrC1(:,1)+0.015, ptfrC1(:,2), str)
hold off

figure
plot(ptfrA1000(:,1),ptfrA1000(:,2), 'o', 'MarkerFaceColor','b', 'MarkerSize', 10);
hold on
plot(ptfrB1000(:,1),ptfrB1000(:,2), 'o', 'MarkerFaceColor','y','MarkerSize', 10);
plot(ptfrC1000(:,1),ptfrC1000(:,2), 'o', 'MarkerFaceColor','m','MarkerSize', 10);
title('Imagem dos 3 pontos aleatórios com coordenadas entre 0 e 1000 nas três câmeras')
legend('Câmera A', 'Câmera B', 'Câmera C')
xlabel('x')
ylabel('y')
ylim([100, 1000])
xlim([100,1000])
text(ptfrA1000(:,1)+8, ptfrA1000(:,2), str)
text(ptfrB1000(:,1)+8, ptfrB1000(:,2), str)
text(ptfrC1000(:,1)+8, ptfrC1000(:,2), str)
hold off

%Boxplot para testes feitos em FEV 22
% format shortG
% id ponto
B(1,1)=1;
B(2,1)=1000;

% Tempo médio do solver para cada ponto e distância do ponto ao ponto 3011
for i=1:2
    B(i,2)=mean(timept2(2001-1000*i:1000*(3-i),2));  % tempo médio
    B(i,3)=median(timept2(2001-1000*i:1000*(3-i),2));  % tempo mediano
end
for m=1:2
    t2(:,m)=timept2(2001-1000*m:1000*(3-m),2);
end


str2={'A', 'B'}; %caso A: coord entre 0 e 1; caso B: coord entre 0 e 1000
figure      %Boxplot com os pontos obedecendo a posicão relativa deles em relacão à reta r

boxplot(t2, str2)
xlabel('Caso')
ylabel('Tempo (ms)')
hold on
plot(B(:,2),'o') % marcacão das médias
hold off
title('Tempo de execucão minus-chicago com os pontos 620, 3389 e pontos aleatórios (4 CPUS)')
legend('Tempo médio')
