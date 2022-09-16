
% Problem x Time x track 
% 1000 Problemas, 100 experimentos de 5 testes cada
% Aqui estão sendo considerados os problemas no arquivo FrePTcorrigido.txt

clear all
close all

tracks(:,1)=load('tracks-1.txt'); timesoltrack(:,:,1)=load('time-sol-1.txt');
tracks(:,2)=load('tracks-2.txt'); timesoltrack(:,:,2)=load('time-sol-2.txt');
tracks(:,3)=load('tracks-3.txt'); timesoltrack(:,:,3)=load('time-sol-3.txt');
tracks(:,4)=load('tracks-4.txt'); timesoltrack(:,:,4)=load('time-sol-4.txt');
tracks(:,5)=load('tracks-5.txt'); timesoltrack(:,:,5)=load('time-sol-5.txt');
tracks(:,6)=load('tracks-6.txt'); timesoltrack(:,:,6)=load('time-sol-6.txt');
tracks(:,7)=load('tracks-7.txt'); timesoltrack(:,:,7)=load('time-sol-7.txt');
tracks(:,8)=load('tracks-8.txt'); timesoltrack(:,:,8)=load('time-sol-8.txt');
tracks(:,9)=load('tracks-9.txt'); timesoltrack(:,:,9)=load('time-sol-9.txt');
tracks(:,10)=load('tracks-10.txt'); timesoltrack(:,:,10)=load('time-sol-10.txt');
tracks(:,11)=load('tracks-11.txt'); timesoltrack(:,:,11)=load('time-sol-11.txt');
tracks(:,12)=load('tracks-12.txt'); timesoltrack(:,:,12)=load('time-sol-12.txt');
tracks(:,13)=load('tracks-13.txt'); timesoltrack(:,:,13)=load('time-sol-13.txt');
tracks(:,14)=load('tracks-14.txt'); timesoltrack(:,:,14)=load('time-sol-14.txt');
tracks(:,15)=load('tracks-15.txt'); timesoltrack(:,:,15)=load('time-sol-15.txt');
tracks(:,16)=load('tracks-16.txt'); timesoltrack(:,:,16)=load('time-sol-16.txt');
tracks(:,17)=load('tracks-17.txt'); timesoltrack(:,:,17)=load('time-sol-17.txt');
tracks(:,18)=load('tracks-18.txt'); timesoltrack(:,:,18)=load('time-sol-18.txt');
tracks(:,19)=load('tracks-19.txt'); timesoltrack(:,:,19)=load('time-sol-19.txt');
tracks(:,20)=load('tracks-20.txt'); timesoltrack(:,:,20)=load('time-sol-20.txt');

% Tempo médio e track médio total dos 20 problemas juntos
tmed0=sum(timesoltrack(:,2,:));
tsum0=sum(tmed0)/10000;   %tempo medio do solver em cada teste
trsum0=sum(tracks);
trmed0=sum(trsum0)/3120000;  %numero medio de tracks por raiz
txtemptra=sum(tmed0)/sum(trsum0);
% Tméd=1349,1 ms = 13,49 microsegundos por teste
% Trméd=322,0525 tracks por raiz
% tempo/track=0,0134 ms/track
newtracks=tracks;
newtracks(newtracks>600)=600;
newtrasum=sum(newtracks);
newtrasum1=sum(newtrasum);
newtemptotal=newtrasum1*0.013491; % Tempo total para nmax=600: 10.881.569,93ms
%reducão no tempo: 2.609.583,07 ~ 260,96 ms/teste


for a=1:20 %Problemas 1 - 20    
    for p=1:500 %100*5 testes
        timesoltrack(p,4,a)=sum(tracks(312*(p-1)+1:312*p,a)); %Total de tracks em cada teste
    end

%separando o tempo e a solucão e arrumando os dados (experimento x teste)

    for i=1:100
        for j=1:5
%             Temp(i,j,a)=timesoltrack(5*(i-1)+j,2,a);  %tempo
            Sol(i,j,a)=timesoltrack(5*(i-1)+j,3,a);  %solucão
                                   
            if timesoltrack(5*(i-1)+j,3,a)==313 % TRASOL é a matriz do número de iteracões na raiz que é solucão em CADA TESTE (CONSIDERA OS 500 TESTES EM CADA PROBLEMA)
                TRASOL(5*(i-1)+j,a)=NaN; % Caso em que não encontra a solucão
            else
                 TRASOL(5*(i-1)+j,a)=tracks(1560*(i-1)+312*(j-1)+timesoltrack(5*(i-1)+j,3,a)+1,a); % soma 1 pq as raízes vão 0-311
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
    time(:,a)=timesoltrack(:,2,a);
    solu(:,a)=timesoltrack(:,3,a);
    totra(:,a)=timesoltrack(:,4,a); % total de iteracões
end


% Tempo
figure
boxplot(time(:,1:10),{'1','2','3','4','5','6','7','8','9','10'})
xlabel('Problema')
ylabel('Tempo (ms)')
title('Tempo de execucão minus-chicago para os Problemas 1 a 10')

figure
boxplot(time(:,11:20),{'11','12','13','14','15','16','17','18','19','20'})
xlabel('Problema')
ylabel('Tempo (ms)')
title('Tempo de execucão minus-chicago para os Problemas 11 a 20')


% Iteracões em cada raiz
figure
boxplot(tracks(:,1:10))
xlabel('Problema')
ylabel('Número de iteracões em cada raiz')
title('Número de iteracões em cada raiz no minus-chicago para os Problemas 1 a 10')

figure
boxplot(tracks(:,11:20),'label',{'11','12','13','14','15','16','17','18','19','20'})
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
boxplot(totra(:,11:20),'label',{'11','12','13','14','15','16','17','18','19','20'})
xlabel('Problema')
ylabel('Número total de iteracões')
title('Número total de iteracões no minus-chicago para os Problemas 11 a 20')


% Iteracões na raiz que é solucão
figure
boxplot(TRASOL(:,1:10))
xlabel('Problema')
ylabel('Número de iteracões')
title('Número de iteracões no minus-chicago para a determinacão da raiz que é solucão dos Problemas 1 a 10')

figure
boxplot(TRASOL(:,11:20),'label',{'11','12','13','14','15','16','17','18','19','20'})
xlabel('Problema')
ylabel('Número de iteracões')
title('Número de iteracões no minus-chicago para a determinacão da raiz que é solucão dos Problemas 11 a 20')


%%%%%%%%%%%%%%%%%%%%%%

%%% Verificando em quais testes o número de iteracões na raiz que é solucão
%%% ultrapassa o limite (1000)
clear SRT1000 p r 
for p=1000:-100:300
    Sol1000=TRASOL;
    for r=1:20
        AuxN1=isnan(TRASOL(:,r));
        Aux1=sum(AuxN1(:,1)==1);
        for s=1:500
            if TRASOL(s,r)>p % ERRO 
               Sol1000(s,r)=NaN;
            end
        end
        AuxN2=isnan(Sol1000(:,r));
        Aux2=sum(AuxN2==1);
        SRT1000(r,(1000-p)/100+1)=Aux2-Aux1;
    end
    SRT1000(21,(1000-p)/100+1)=sum(SRT1000(1:20,(1000-p)/100+1));
end

clear t at strg Strg tSTR1000
for t=1:8
    at(t)=1000-100*(t-1);
end
strg=string(at);
Strg=categorical(strg);
Strg=reordercats(Strg,strg);
tSRT1000=transpose(SRT1000);

figure
bar(Strg,tSRT1000(:,1:20)/5)
xlabel('nmax')
ylabel('Percentual de testes')
% title('Número de testes que falham por causa do número máximo de iteracões')
legend('P1','P2','P3','P4','P5','P6','P7','P8','P9','P10','P11','P12','P13','P14','P15','P16','P17','P18','P19','P20')

figure
bar(Strg,tSRT1000(1:8,21)/100)
xlabel('nmax')
ylabel('Percentual de testes')

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


% ******* Número mínimo de testes para solucão correta 
C1 = categorical({'1','2','3','4','5','Mais de 5'});
figure
bar(C1,MinSol(:,1:10));
xlabel('Número de testes')
ylabel('Percentual de experimentos')
title('Percentual de experimentos por número de testes necessários para encontrar a solucão correta nos Problemas 1 a 10')
legend('P1','P2','P3','P4','P5','P6','P7','P8','P9','P10')

figure
bar(C1,MinSol(:,11:20));
xlabel('Número de testes')
ylabel('Percentual de experimentos')
title('Percentual de experimentos por número de testes necessários para encontrar a solucão correta nos Problemas 11 a 20')
legend('P11','P12','P13','P14','P15','P16','P17','P18','P19','P20')


% ******* Número mínimo ACUMULADO de testes para solucão correta 
C3 = categorical({'1','2','3','4','5'});
figure
bar(C3, TAcumuSol(:,1:10));
xlabel('Teste')
ylabel('Percentual de experimentos')
title('Frequência acumulada de experimentos nos quais se encontra a solucão correta em algum teste nos Problemas 1 a 10')
legend('P1','P2','P3','P4','P5','P6','P7','P8','P9','P10')

figure
bar(C3, TAcumuSol(:,11:20));
xlabel('Teste')
ylabel('Percentual de experimentos')
title('Frequência acumulada de experimentos nos quais se encontra a solucão correta em algum teste nos Problemas 11 a 20')
legend('P11','P12','P13','P14','P15','P16','P17','P18','P19','P20')


% ******** Número de testes por experimento em que se encontra a solucão correta
C2=0:5;
figure
bar(C2,RiSol(:,1:10));
xlabel('Número de testes que falham')
ylabel('Percentual de experimentos')
legend('P1','P2','P3','P4','P5','P6','P7','P8','P9','P10')
title('Percentual de falha na determinacão da solucão correta nos experimentos dos Problemas 1 a 10')

figure
bar(C2,RiSol(:,11:20));
xlabel('Número de testes que falham')
ylabel('Percentual de experimentos')
legend('P11','P12','P13','P14','P15','P16','P17','P18','P19','P20')
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
