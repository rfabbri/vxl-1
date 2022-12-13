

% Problem x Time x track 
% 1000 Problemas, 100 experimentos de 5 testes cada
% Aqui estão sendo considerados os problemas no arquivo FrePTcorrigido.txt

clear all
close all

addpath(genpath('FRPT1000'));

for nt=1:138 %número de testes
    i=string(nt);
    str1=join(['tracks' i 'txt'], ["-", "."]);
    str2=join(['time-sol' i 'txt'], ["-", "."]);
    tracks(:,nt)=load(str1);
    timesoltrack(:,:,nt)=load(str2);
end

for a=1:nt %Problemas 1 - nt    
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
                TIMESOL(5*(i-1)+j,a)=NaN; % TIMESOL é a matriz dos tempos dos testes em que se encontra a solucão
            else
                 TRASOL(5*(i-1)+j,a)=tracks(1560*(i-1)+312*(j-1)+timesoltrack(5*(i-1)+j,3,a)+1,a); % soma 1 pq as raízes vão 0-311
                 TIMESOL(5*(i-1)+j,a)=timesoltrack(5*(i-1)+j,2,a);
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
for a=1:nt
    time(:,a)=timesoltrack(:,2,a);
    solu(:,a)=timesoltrack(:,3,a);
    totra(:,a)=timesoltrack(:,4,a); % total de iteracões
end

for t=1:10
    at1(t)=t;
    at2(t)=t+10;
end
stra1=string(at1);
stra2=string(at2);

% Tempo
figure
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
boxplot(tracks(:,1:10), stra1)
xlabel('Problema')
ylabel('Número de iteracões em cada raiz')
title('Número de iteracões em cada raiz no minus-chicago para os Problemas 1 a 10')

figure
boxplot(tracks(:,11:20),'label',stra2)
xlabel('Problema')
ylabel('Número de iteracões em cada raiz')
title('Número de iteracões em cada raiz no minus-chicago para os Problemas 11 a 20')

% Total de iteracões
figure
boxplot(totra(:,1:10), stra1)
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
boxplot(TRASOL(:,1:10), stra1)
xlabel('Problema')
ylabel('Número de iteracões')
title('Número de iteracões no minus-chicago para a determinacão da raiz que é solucão dos Problemas 1 a 10')

figure
boxplot(TRASOL(:,11:20),'label',stra2)
xlabel('Problema')
ylabel('Número de iteracões')
title('Número de iteracões no minus-chicago para a determinacão da raiz que é solucão dos Problemas 11 a 20')

%%% Juntando o número de iteracões em cada raiz de todos os problemas. O
%%% mesmo para o # de iteracões na raiz que é solucão e o tempo de execucão
%%% nos testes em que acha a solucão

for m=1:nt
    for n=1:312*500 %(#raízes*#testes)
        jointracks(n+156000*(m-1),1)=tracks(n,m);  % Junta as iteracões em cada raiz
    end
    
    for o=1:500
        joinTRASOL(o+500*(m-1),1)=TRASOL(o,m);   % Junta as iteracões na raiz que é solucão
        joinTIMESOL(o+500*(m-1),1)=TIMESOL(o,m); % Junta os tempos dos testes em que se encontra solucão
    end
end

join=[jointracks; joinTRASOL];
g1=repmat({'Todas as raízes'},312*500*nt,1);
g2=repmat({'Raiz que é solucão'},500*nt,1);
g=[g1; g2];   %junta os vetores de dimensões diferentes em uma matriz

figure
boxplot(join,g);
ylabel('Número de iteracões em cada raiz')
xlabel('Tipo de raiz')
title('Número de iteracões em cada raiz e apenas na raiz que é solucão no minus-chicago - Problemas 1 a 138')

%Reduzindo o limite de iteracões para 200
Sol200=joinTRASOL;
AuxN1=isnan(joinTRASOL);
Aux1=sum(AuxN1==1);  % número de testes em que a solucão falha (track=NaN)
for k=1:500*nt
    if joinTRASOL(k)>200
        Sol200(k)=NaN;  % "exclui" as raízes nas quais o número de iteracões é >200
    end
end
AuxN2=isnan(Sol200);
Aux2=sum(AuxN2==1);
Aux3=Aux2-Aux1; % número de raízes com mais de 200 iteracões

% Mantém apenas os 2 primeiros testes em cada experimento
Sol200A=Sol200; %Sol200A mantem os dois primeiros testes em cada experimento
for b=68998:-5:3  %(68998=138*500-2)
    Sol200A(b)=[];
    Sol200A(b)=[];
    Sol200A(b)=[];
end

falhaA(2)=sum(isnan(Sol200A(1:2:27599))==1); % # falhas no primeiro teste/ 27599=138*200-1
falhaA(3)=sum(isnan(Sol200A(2:2:27600))==1); % # falhas no segundo teste
Aux4A=0;
for b=1:2:27599
    if isnan(Sol200A(b))==1 && isnan(Sol200A(b+1))==1 % # falhas no primeiro e segundo testes
        Aux4A=Aux4A+1;
    end
end
falhaA(4)=Aux4A;
falhaA(1)=27600-falhaA(2)-falhaA(3)+falhaA(4); % nenhuma falha

figure
C10=categorical({'Nenhum', '1o', '2o', '1o e 2o'});
C10=reordercats(C10,{'Nenhum', '1o', '2o', '1o e 2o'});
bar(C10, falhaA*100/27600)
xlabel('Teste(s) que falha(m) em cada experimento')
ylabel('Percentual de experimentos (%)')
title('Percentual de falha em cada experimento')

%Vetor falha considera experimentos de 2 testes(69000/2=34500 experimentos)
falha(2)=sum(isnan(Sol200(1:2:68999))==1); % # falhas no primeiro teste
falha(3)=sum(isnan(Sol200(2:2:69000))==1); % # falhas no segundo teste
Aux4=0;
for b=1:2:68999
    if isnan(Sol200(b))==1 && isnan(Sol200(b+1))==1 % # falhas no primeiro e segundo testes
        Aux4=Aux4+1;
    end
end
falha(4)=Aux4;
falha(1)=69000-falha(2)-falha(3)+falha(4); % nenhuma falha

figure
bar(C10, falha*100/69000)
xlabel('Teste(s) que falha(m) em cada experimento')
ylabel('Percentual de experimentos (%)')
title('Percentual de falha em cada experimento -69000 testes')
ylim([0,100])
%%%%%%%%%%%%%%%%%%%%%%

%%% Tratando apenas as solucões reais  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% raizrealN: Matriz com as RAÍZES REAIS que NÃO são solucão em cada teste em cada um dos 138 problemas e o código isvalid
% 313 10: aparece abaixo das raízes de cada teste. Para cada problema há  500 linhas 313 10

raizrealN=load('realsol1000-juntos.txt');
ind=find(raizrealN(:,2)==10);  %procura em raizrealN onde encerra as raízes reais de um teste (término indicado pelo 10)

nind(1,1)=ind(1);
for a=2:length(ind)
    nind(a,1)=ind(a)-ind(a-1); %número de raízes reais em cada teste (as que são solucão e as que não são)
end
mean(nind)
median(nind)

figure
boxplot(nind,{'Raízes reais'})
xlabel('Tipo de raiz')
ylabel('Número de raízes')
title('Número de raízes reais em cada teste no minus-chicago - Problemas 1 a 138') 

aux5=1;
for r=1:length(ind) % # do teste, considerando todos os problemas juntos
    for s=aux5:ind(r)-1 %intervalo das raízes reais do mesmo teste
        raizrealN(s,3)=jointracks(raizrealN(s,1)+1+312*(r-1));
    end
    aux5=ind(r)+1;
end

trackraizrealN=raizrealN(:,3);
trackraizrealN(trackraizrealN==0)=NaN;
join1=[trackraizrealN; joinTRASOL];
g3=repmat({'Raízes reais que NÃO são solucão'},length(trackraizrealN),1);
g4=repmat({'Raiz real que é solucão'},500*nt,1);
G=[g3; g4];   %junta os vetores de dimensões diferentes em uma matriz

figure
boxplot(join1,G);
ylabel('Número de iteracões em cada raiz')
xlabel('Tipo de raiz')
title('Número de iteracões em cada raiz real no minus-chicago - Problemas 1 a 138 - 69000 testes')

mean(trackraizrealN, 'omitnan') %151,4618
mean(joinTRASOL, 'omitnan') %150,8616
median(trackraizrealN, 'omitnan') %128
median(joinTRASOL, 'omitnan') %128
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%% Verificando em quais testes o número de iteracões na raiz que é solucão
%%% ultrapassa o limite (1000)
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
for n=1:138
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

clear joinFalha
%%%% Juntar taxa de falha
joinFalha=sum(RiSol,2);
figure
bar(C2,joinFalha*100/(100*20));
xlabel('Número de testes que falham em cada experimento')
ylabel('Percentual de experimentos')
title('Percentual de falha na determinacão da solucão correta nos experimentos dos Problemas 1 a 20')


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
