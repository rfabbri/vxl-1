% TABELA COM TEMPO MÉDIO E MEDIANO DO SOLVER PARA OS PONTOS 620, 3389 e PONTOS SOBRE
% A CURVA 8, EXPERIMENTOS FEITOS EM
% 03/22. 1000 testes para cada ponto
% TAXA DE ERRO NA DETERMINACÃO DA SOLUCÃO

clear all
close all

%carregar tempos/pontos
timept=load('timesolutionERROmar22.txt');

%Deterimnacão dos quartis
for k=1:35
    Ntimept(k,:)=timept(1000*k-999:1000*k,2);
end
timeptT=transpose(Ntimept);
Y = prctile(timeptT,[25 50 75],1); %percentis dos tempos para cada ponto

for y=1:35
    Yinv(:,y)=Y(:,36-y); %Para manter a mesma disposicão dos pontos no gráfico
end

writematrix(Yinv, 'Mpercentis.txt', 'Delimiter', 'tab');

%id do ponto

for n=1:9
    nerro(n,1)=419+(n-1)*3;
end
nerro(10,1)=445;
for m=11:35
    nerro(m,1)=419+(m-2)*3;
end

for a=1:35
    nerro(a,2)=sum(timept(1000*a-999:1000*a,3)==313);  %número de solucões não compatíveis com configuracão de câmeras (TAXA DE ERRO.)
    nerro(a,3)=1000-nerro(a,2);
    nerro(a,4)=mean(timept(1000*a-999:1000*a,2));  %média geral
    vtemp1=timept(1000*a-999:1000*a,2:3);  %variável temporária: todos os tempos para o ponto
    vtemp2=vtemp1(vtemp1(:,2)~=313);   % variável temporária: tempos excluídos os das solucões não compatíveis para o ponto
    nerro(a,5)=mean(vtemp2);  % tempo médio sem erro
    nerro(a,6)=median(timept(1000*a-999:1000*a,2)); % tempo mediano geral
    nerro(a,7)=median(vtemp2); % tempo mediano sem erro
    tmax(a,1)=max(timept(1000*a-999:1000*a,2));
    tmax(a,2)=median(timept(1000*a-999:1000*a,2));
end
nerro;

format shortG

for j=1:35
    t(:,j)=timept(1000*j-999:1000*j,2);
end

for b=1:35     % inverte a posicão na tabela, para que fique em ordem decrescente das ids dos pontos
    ti(:,b)=t(:,36-b);
end

for a=1:35    % inverte a posicão na tabela, para que fique em ordem decrescente das ids dos pontos
    B(a,:)=nerro(36-a,:);
end 


figure
boxplot(ti,'Labels',{B(:,1)})
hold on
plot(B(:,4),'o') % marcacão das médias
plot(B(:,6),'r') %Reta unindo as medianas
xlabel('Pontos sobre a curva 8')
ylabel('Tempo (ms)')
title('Tempo de execucão minus-chicago com os pontos 620, 3389 e pontos sobre a curva 8. (Experimentos realizados em marco de 2022.)')
legend('Tempo médio','Tempo mediano')

figure
B1 = int2str(B(:,1));  %converte em string
S=cellstr(B1);   %converte em vetor de caracter
X = categorical(S);
X = reordercats(X,S);  %operacões necessárias para que o gráfico de barras saia na ordem desjada - decrescente
bar(X,B(:,2)*0.1) %gráfico do número de erros na determinacão da solucão
xlabel('Pontos sobre a curva 8')
ylabel('% de erro')
%axis tight
title('Percentual de erro do minus-chicago na determinacão da solucão para os pontos 620, 3389 e pontos sobre a curva 8')

figure
dif=abs(B(:,6)-B(:,7));
plot(X,dif, 'm') %gráfico da diferenca entre tempo mediano geral e mediano sem erro
xlabel('Pontos sobre a curva 8')
ylabel('Tempo(ms)')
title('Diferenca absoluta entre o tempo mediano geral e o tempo mediano sem erro')
