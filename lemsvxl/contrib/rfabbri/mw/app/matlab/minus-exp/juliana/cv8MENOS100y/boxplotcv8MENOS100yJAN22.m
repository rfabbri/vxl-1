% GRÁFICO E BOXPLOT DO TEMPO DE EXECUCÃO DO MINUS-CHICAGO PARA OS PTS 620,
% 3389 E PONTOS OBTIDOS DA SUBTRACÃO DE 100 PIXELS DA COORDENADA Y DE PTS
% SOBRE A CURVA 8 - JANEIRO DE 22
clear all
close all

time=load('timesolcv8MENOS100yJAN22.txt');
pts=load('cv8fr42MENOS100y.txt'); %pts da cv 8 utilizados subtraídos 100 pixels da coordenada y
for a=1:35
    ptinvert(a,:)=pts(36-a,:); %inverte a posicão dos pontos para coincidir com a posicão no gráfico
end
ninvert=ptinvert;
ninvert(26,:)=[ ];
ninvert;

x620=267.83329318961659737;
y620=398.26842358907475727;
x3389=239.87635969387076784;
y3389=87.172102114226248659;
%equacao da reta r que passa pelos pontos 620 e 3389
deltax=x620-x3389;
deltay= y620-y3389;
x=x3389:0.1:x620;
y=(deltay/deltax)*(x-x3389)+y3389;

for j=35:-1:1
    t(:,j)=time(1000*(36-j)-999:1000*(36-j),2); 
    m(j,1)=mean(t(:,j)); 
    m(j,2)=abs((deltay/deltax)*(ptinvert(j,1)-x3389)+y3389 -ptinvert(j,2))/sqrt((ptinvert(j,1))^2 + (ptinvert(j,2))^2);  % distância à reta
end
Nt=t;
Nt(:,26)=[];
Nm=m;
Nm(26,:)=[];
Nstr={'518*','515*','512*','509*','506*','503*','500*','497*','494*','491*','488*','485*','482*','479*','476*','473*','470*','467*','464*','461*','458*','455*', '452*','449*','446*', '443*', '440*', '437*', '434*','431*', '428*','425*', '422*', '419*'};


str={'518*','515*','512*','509*','506*','503*','500*','497*','494*','491*','488*','485*','482*','479*','476*','473*','470*','467*','464*','461*','458*','455*', '452*','449*','446*', '445*', '443*', '440*', '437*', '434*','431*', '428*','425*', '422*', '419*'};

figure
colororder({'b','#77AC30'})
yyaxis left
boxplot(Nt, Nstr)
xlabel('Pontos')
ylabel('Tempo (ms)')
hold on
plot(Nm(:,1),'o'); %tempo médio
yyaxis right
plot(Nm(:,2),'*','Color','#77AC30') %distância do ponto à reta
ylabel('Distância do ponto à reta r')
legend('Tempo médio','Distância do ponto à reta')
title('Boxplot do tempo de execucão do minus-chicago para os pontos 620, 3389 e pontos obtidos da subtracão de 100 pixels da coordenada y de pontos sobre a curva 8')
hold off

figure
plot(ninvert(:,1),ninvert(:,2), '*','MarkerSize',10)
hold on
plot([x620 x3389],[y620 y3389],'o','MarkerFaceColor','r')
plot(x,y,'r')
title('Imagem na câmera 42 de pontos obtidos a partir da subtracão de 100 pixels da coordenada y de pontos sobre a curva 8')
text(ninvert(1:25,1)-3, ninvert(1:25,2)+8, Nstr(1,1:25))
text(ninvert(26:34,1)-1, ninvert(26:34,2)+8, Nstr(1,26:34))
% text([ptinvert(26,1)-1 x620+2 x3389+2],[ptinvert(26,2)-8 y620 y3389],{'445*','620','3389'})
text([x620+2 x3389+2],[y620 y3389],{'620','3389'})

xlabel('x')
ylabel('y')
hold off



