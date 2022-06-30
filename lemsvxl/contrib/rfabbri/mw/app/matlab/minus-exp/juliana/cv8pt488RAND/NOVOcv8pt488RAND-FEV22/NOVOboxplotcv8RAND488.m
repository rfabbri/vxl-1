%GRÁFICOS DOS PONTOS ALEATÓRIOS GERADOS A PARTIR DO PONTO 488 COM
%DISTRIBUICÃO NORMAL (SIGMA=20) E TEMPO DE EXECUCÃO DO MINUS-CHICAGO PARA
%OS ESSES PONTOS E OS PONTOS 620 E 3389 NAS CÂMERAS 42, 54 E 62

clear all
close all

M=load('novaMRAND488.txt'); % Matriz dos 10 pontos aleatórios nas imagens das 3 câmeras 
M42=M(1:3:30,:);
M54=M(2:3:30,:);
M62=M(3:3:30,:);
pt=load('input488JAN22.txt'); % matriz das coordenadas dos pts 620, 3389 e 488 nas imagens das 3 câmeras
str={'P1', 'P2','P3','P4','P5','P6','P7', 'P8', 'P9','P10'};

%câmera 42
figure
plot(M42(:,1),M42(:,2), '*','MarkerSize',8);
hold on
plot(pt(1:2,1),pt(1:2,2), 'o','MarkerFaceColor','r'); %pts 620 e 3389
text(pt(1:3,1),pt(1:3,2)+5, {'620','3389','488'})
text(M42(:,1),M42(:,2)+5,str)
plot(pt(3,1),pt(3,2),'*', 'MarkerSize',10); %pt 488
xlabel('x')
ylabel('y')
ylim([50,450])
title('Imagem dos pontos 620, 3389, 488 e pontos gerados aleatóriamente com distribuicão normal (\sigma=20) a partir do ponto 488 na câmera 42')
hold off

%câmera 54
figure
plot(M54(:,1),M54(:,2), '*','MarkerSize',8);
hold on
plot(pt(4:5,1),pt(4:5,2), 'o','MarkerFaceColor','r'); %pts 620 e 3389
text(pt(4:5,1)-4,pt(4:5,2)+5, {'620','3389'})
text(pt(6,1)-3,pt(6,2)-5, {'488'})
text(M54(:,1)-1,M54(:,2)+5,str)
plot(pt(6,1),pt(6,2),'*', 'MarkerSize',10); %pt 488
xlabel('x')
ylabel('y')
ylim([40,265])
title('Imagem dos pontos 620, 3389, 488 e pontos gerados aleatóriamente com distribuicão normal (\sigma=20) a partir do ponto 488 na câmera 54')
hold off

%câmera 62
figure
plot(M62(:,1),M62(:,2), '*','MarkerSize',8);
hold on
plot(pt(7:8,1),pt(7:8,2), 'o','MarkerFaceColor','r'); %pts 620 e 3389
text(pt(7:8,1),pt(7:8,2)+5, {'620','3389'})
text(pt(9,1)-2,pt(9,2)-5, {'488'})
text(M62(3:10,1)-2,M62(3:10,2)+5,str(:,3:10))
text(M62(1:2,1),M62(1:2,2)-5,str(:,1:2))
plot(pt(9,1),pt(9,2),'*', 'MarkerSize',10); %pt 488
xlabel('x')
ylabel('y')
title('Imagem dos pontos 620, 3389, 488 e pontos gerados aleatóriamente com distribuicão normal (\sigma=20) a partir do ponto 488 na câmera 62')
hold off

timesol=load('NOVOtimesolutionRAND488.txt');
for j=1:10
    t(:,j)=timesol(1000*j-999:1000*j,2);
    m(1:j)=mean(t(:,j)); 
end

figure
boxplot(t,str)
%ylim([550,5600])
hold on
plot(m, 'o')
legend('Tempo médio')
xlabel('Pontos aleatórios')
ylabel('Tempo de execucão do solver (ms)')
title('Tempo de execucão do minus-chicago para os pontos 620, 3389 e pontos aleatórios gerados a partir do ponto 488 com distribuicão normal (\sigma=20)')
hold off

figure
th=tiledlayout(2,5)
xlabel(th,'Tempo (ms)')
title(th,'Teste 3b: Tempo de execucão do minus-chicago para pontos aleatórios gerados a partir do ponto 488 com distribuicão normal (\sigma=20)')
for a=1:10
    nexttile
    histogram(t(:,a))
    title(['P ',num2str(a)])
end

figure   % A informacão do gráfico não sei se é útil
tp=tiledlayout(2,5)
for k=1:10
    nexttile
    pd(k) = fitdist(t(:,k),'Normal');
    xi(k)=min(t(:,k));
    xf(k)=max(t(:,k));
    plot(xi(k):xf(k),pdf(pd(a),xi(k):xf(k)),'r')
end

%Funcão normal que se ajusta aos dados
pd = fitdist(t(:,1),'Normal')
xi=min(t(:,1));
    xf=max(t(:,1));
    x = xi:xf;

y = pdf(pd,x);
% y = (exp(-(1/2)*((x - 968.58)/122.051).^2))/sqrt(2*pi*122.051^2);
plot(x,y)



