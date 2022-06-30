
% Problem x Time x track NÚMERO MÁXIMO DE TRACKS=700
% 1000 Problemas, 100 experimentos de 5 testes cada
% Aqui estão sendo considerados os problemas no arquivo FrePTcorrigido.txt

clear all
close all


%%% Carrega vários arquivos
for j=1:20
    i=string(j);
    str1=join(['tracks700' i 'txt'], ["-", "."]);
    str2=join(['time-sol700' i 'txt'], ["-", "."]);
    tracks700(:,j)=load(str1);
    timesoltrack700(:,:,j)=load(str2);
end
%%%%%%%%%%%%%%%%%%%%%%%%

% Tempo médio e track média total dos 20 problemas juntos
tmed700=sum(timesoltrack700(:,2,:));
tsum=sum(tmed700)/10000;
trsum700=sum(tracks700);
trmed=sum(trsum700)/3120000;
txtemptra700=sum(tmed700)/sum(trsum700);
% Tméd= 1450,3 ms
% Trméd= 227,1326
% 0,020465 ms/track =20 microsegundos/track

for a=1:20 %Problemas 1 - 20
    
    for p=1:500 %100*5 testes
        timesoltrack700(p,4,a)=sum(tracks700(312*(p-1)+1:312*p,a)); %Total de tracks700 em cada teste
    end

%separando o tempo e a solucão e arrumando os dados (experimento x teste)

    for i=1:100
        for j=1:5
%             Temp(i,j,a)=timesoltrack(5*(i-1)+j,2,a);  %tempo
            Sol(i,j,a)=timesoltrack700(5*(i-1)+j,3,a);  %solucão
                                   
            if timesoltrack700(5*(i-1)+j,3,a)==313 % TRASOL é a matriz do número de iteracões na raiz que é solucão em CADA TESTE (CONSIDERA OS 500 TESTES EM CADA PROBLEMA)
                TRASOL700(5*(i-1)+j,a)=NaN; % Caso em que não encontra a solucão
            else
                 TRASOL700(5*(i-1)+j,a)=tracks700(1560*(i-1)+312*(j-1)+timesoltrack700(5*(i-1)+j,3,a)+1,a); % soma 1 pq as raízes vão 0-311
            end
            
        end
        
        Sol(i,6,a)=sum(Sol(i,1:5,a)==313); %Número de testes em cada experimento em que NÃO se encontra a solucão correta
        for m=1:5
            if Sol(i,m,a)~=313
                Sol(i,7,a)=m;  %Sol(i,7) mostra a primeira vez em cada experimento em que a solucão correta é encontrada
                break;
            end
        end
%         Sol(i,8,a)=max(TR(312*(i-1)+1:312*i,Sol(i,7),a)); %número máximo de iteracões em cada raiz no 1o teste em que a solucão correta foi encontrada em cada experimento.
    end
end


%%%%%%%%% BOXPLOT (Foram considerados os 500 testes juntos) %%%%%%%%%%
for a=1:20
    time(:,a)=timesoltrack700(:,2,a);
    solu(:,a)=timesoltrack700(:,3,a);
    totra(:,a)=timesoltrack700(:,4,a); % total de iteracões
end

% Alternativa para criar array de strings com números -+-+-+-+-+

for t=1:10
    at1(t)=t;
    at2(t)=t+10;
end
stra1=string(at1);
stra2=string(at2);

%%%%%%% -+-+-+-+-+-+-+-+-+-+-+-+

% Tempo
figure
% boxplot(time(:,1:10),{'1','2','3','4','5','6','7','8','9','10'})
boxplot(time(:,1:10),stra1)
xlabel('Problema')
ylabel('Tempo (ms)')
title('Tempo de execucão minus-chicago para os Problemas 1 a 10')

figure
boxplot(time(:,11:20),stra2)
xlabel('Problema')
ylabel('Tempo (ms)')
title('Tempo de execucão minus-chicago para os Problemas 11 a 20')


% Iteracões em cada raiz
figure
boxplot(tracks700(:,1:10))
xlabel('Problema')
ylabel('Número de iteracões em cada raiz')
title('Número de iteracões em cada raiz no minus-chicago para os Problemas 1 a 10')

figure
boxplot(tracks700(:,11:20),'label',stra2)
xlabel('Problema')
ylabel('Número de iteracões em cada raiz')
title('Número de iteracões em cada raiz no minus-chicago para os Problemas 11 a 20')

% Total de iteracões
figure
boxplot(totra(:,1:10))
xlabel('Problema')
ylabel('Número total de iteracões')
title('Número total de iteracões no minus-chicago para os Problemas 1 a 10')

figure
boxplot(totra(:,11:20),'label',stra2)
xlabel('Problema')
ylabel('Número total de iteracões')
title('Número total de iteracões no minus-chicago para os Problemas 11 a 20')


% Iteracões na raiz que é solucão
figure
boxplot(TRASOL700(:,1:10))
xlabel('Problema')
ylabel('Número de iteracões')
title('Número de iteracões no minus-chicago para a determinacão da raiz que é solucão dos Problemas 1 a 10')

figure
boxplot(TRASOL700(:,11:20),'label',stra2)
xlabel('Problema')
ylabel('Número de iteracões')
title('Número de iteracões no minus-chicago para a determinacão da raiz que é solucão dos Problemas 11 a 20')


%%% Verificando em quais testes na raiz que é solucão o número de iteracões
%%% ultrapassa o limite
clear p r s aux1 aux2 Sol700 SRT700
for p=700:-100:300
    Sol700=TRASOL700;
    for r=1:20
        auxN1=isnan(TRASOL700(:,r));
        aux1=sum(auxN1(:,1)==1);
        for s=1:500
            if TRASOL700(s,r)>p % ERRO 
               Sol700(s,r)=NaN;
            end
        end
        auxN2=isnan(Sol700(:,r));
        aux2=sum(auxN2==1);
        SRT700(r,(700-p)/100+1)=aux2-aux1;
        SRT700(r,6)=aux2; %TOTAL DE TESTES QUE FALHAM POR QQ MOTIVO para cada p
    end
end
            
       
figure
    bar(transpose(SRT700))

% RESULTADO: EM TODOS os testes em pelo menos 1 raiz o limite de iteracões é ultrapassado 
    



% $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% ---------- GRÁFICOS DE BARRAS (para vários problemas)---------
for n=1:20
        for o=1:5
            MinSol(n,o)=sum(Sol(:,7,n)==o); % (n x o):quantidade de experimentos no Problema n em que são necessários o testes para alcancar a solucão correta 
            RiSol(n,o+1)=sum(Sol(:,6,n)==o); % (n x o):quantidade de experimentos no Problema n em que o testes falham
            for q=1:100
                AcumuSol(q,o,n)=sum(Sol(q,1:o,n)~=313); % quantidade ACUMULADA de testes por experimento no Problema n em que se alcanca solucão correta
            end
        end
        for g=1:5
            TAcumuSol(n,g)=sum(AcumuSol(:,g,n)>0 & AcumuSol(:,g,n)<=g); % número de experimentos no Problema n em que em até g teste se alcanca solucão correta
        end
        MinSol(n,6)=sum(Sol(:,7,n)==0);
        RiSol(n,1)=sum(Sol(:,6,n)==0);
end
MinSol=transpose(MinSol);
RiSol=transpose(RiSol);
TAcumuSol=transpose(TAcumuSol);

% -+-+-+-+- Criar string dos P's -+-+-+-+-+-
clear t t1 t2 strPa strPb srtPb
for t=1:10
    t1=string(t);
    aux=t+10;
    t2=string(aux);
    strPa(t)=join(['P' t1],[""])
    strPb(t)=join(['P' t2],[""])
end


% ******* Número mínimo de testes para solucão correta 
C1 = categorical({'1','2','3','4','5','Mais de 5'});
figure
bar(C1,MinSol(:,1:10));
xlabel('Número de testes')
ylabel('Percentual de experimentos')
title('Percentual de experimentos por número de testes necessários para encontrar a solucão correta nos Problemas 1 a 10')
% legend('P1','P2','P3','P4','P5','P6','P7','P8','P9','P10')
legend(strPa)

figure
bar(C1,MinSol(:,11:20));
xlabel('Número de testes')
ylabel('Percentual de experimentos')
title('Percentual de experimentos por número de testes necessários para encontrar a solucão correta nos Problemas 11 a 20')
% legend('P11','P12','P13','P14','P15','P16','P17','P18','P19','P20')
legend(strPb)


% ******* Número mínimo ACUMULADO de testes para solucão correta 
C3 = categorical({'1','2','3','4','5'});
figure
bar(C3, TAcumuSol(:,1:10));
xlabel('Teste')
ylabel('Percentual de experimentos')
title('Frequência acumulada de experimentos nos quais se encontra a solucão correta em algum teste nos Problemas 1 a 10')
legend(strPa)

figure
bar(C3, TAcumuSol(:,11:20));
xlabel('Teste')
ylabel('Percentual de experimentos')
title('Frequência acumulada de experimentos nos quais se encontra a solucão correta em algum teste nos Problemas 11 a 20')
legend(strPb)


% ******** Número de testes por experimento em que NÃO se encontra a solucão correta
C2=0:5;
figure
bar(C2,RiSol(:,1:10));
xlabel('Número de testes que falham')
ylabel('Percentual de experimentos')
legend(strPa)
title('Percentual de falha na determinacão da solucão correta nos experimentos dos Problemas 1 a 10')

figure
bar(C2,RiSol(:,11:20));
xlabel('Número de testes que falham')
ylabel('Percentual de experimentos')
legend(strPb)
title('Percentual de falha na determinacão da solucão correta nos experimentos dos Problemas 11 a 20')


%   ------HISTOGRAMAS (para 1 problema) -----------
%  ************    Número mínimo de testes para encontrar solucão  correta
% TSol(1:100,n)=Sol(:,7,n);
% C1 = categorical([1 2 3 4 5 6], {'1','2','3','4','5','Mais de 5'}, TSol);
% figure
% histogram(C1);
% xlabel('Número de testes por experimento')
% ylabel('Número de experimentos')
% title('Número de testes necessárias para encontrar a solucão correta nos experimentos do Problema 1')

% ********Histograma do número de testes por experimento em que se encontra a solucão
% correta
% C2 = categorical(TSol(2,:)*20,[0 20 40 60 80 100],{'0','20','40','60','80','100'});
% figure
% histogram(C2);
% xlabel('Percentual de testes em cada experimento')
% ylabel('Número de experimentos')
% title('Percentual de testes em cada experimento do Problema 1 em que se encontra a solucão correta')

%    ********** Histograma 3D do número de experimentos por testes necessários para
%encontar a solucão correta e percentual de testes em que se acerta
% figure
% histogram2(TSol(1,:),TSol(2,:)*20,12);
% xlabel('# de testes necessários para encontar a solucão correta')
% ylabel('% de testes em que se enconta a solucão correta')
% zlabel('Número de experimentos')
% title('Número de experimentos do Problema 1 por testes necessários para encontar a solucão correta e percentual de testes em que se acerta a solucão')
% 
% *******************************************************************************

% tracks700(:,1)=load('tracks700-1.txt'); timesoltrack700(:,:,1)=load('time-sol700-1.txt');
% tracks700(:,2)=load('tracks700-2.txt'); timesoltrack700(:,:,2)=load('time-sol700-2.txt');
% tracks700(:,3)=load('tracks700-3.txt'); timesoltrack700(:,:,3)=load('time-sol700-3.txt');
% tracks700(:,4)=load('tracks700-4.txt'); timesoltrack700(:,:,4)=load('time-sol700-4.txt');
% tracks700(:,5)=load('tracks700-5.txt'); timesoltrack700(:,:,5)=load('time-sol700-5.txt');
% tracks700(:,6)=load('tracks700-6.txt'); timesoltrack700(:,:,6)=load('time-sol700-6.txt');
% tracks700(:,7)=load('tracks700-7.txt'); timesoltrack700(:,:,7)=load('time-sol700-7.txt');
% tracks700(:,8)=load('tracks700-8.txt'); timesoltrack700(:,:,8)=load('time-sol700-8.txt');
% tracks700(:,9)=load('tracks700-9.txt'); timesoltrack700(:,:,9)=load('time-sol700-9.txt');
% tracks700(:,10)=load('tracks700-10.txt'); timesoltrack700(:,:,10)=load('time-sol700-10.txt');
% tracks700(:,11)=load('tracks700-11.txt'); timesoltrack700(:,:,11)=load('time-sol700-11.txt');
% tracks700(:,12)=load('tracks700-12.txt'); timesoltrack700(:,:,12)=load('time-sol700-12.txt');
% tracks700(:,13)=load('tracks700-13.txt'); timesoltrack700(:,:,13)=load('time-sol700-13.txt');
% tracks700(:,14)=load('tracks700-14.txt'); timesoltrack700(:,:,14)=load('time-sol700-14.txt');
% tracks700(:,15)=load('tracks700-15.txt'); timesoltrack700(:,:,15)=load('time-sol700-15.txt');
% tracks700(:,16)=load('tracks700-16.txt'); timesoltrack700(:,:,16)=load('time-sol700-16.txt');
% tracks700(:,17)=load('tracks700-17.txt'); timesoltrack700(:,:,17)=load('time-sol700-17.txt');
% tracks700(:,18)=load('tracks700-18.txt'); timesoltrack700(:,:,18)=load('time-sol700-18.txt');
% tracks700(:,19)=load('tracks700-19.txt'); timesoltrack700(:,:,19)=load('time-sol700-19.txt');
% tracks700(:,20)=load('tracks700-20.txt'); timesoltrack700(:,:,20)=load('time-sol700-20.txt');

