% Refazendo experimentos 7 ao 14 com Intelboost desabilitado -26/05/22
% Problem x Time x track NÚMERO MÁXIMO DE TRACKS=1000
% 8 Problemas, 100 experimentos de 5 testes cada
% Aqui estão sendo considerados os problemas no arquivo FrePTcorrigido.txt

clear all
close all


%%% Carrega vários arquivos
for j=1:8
    i=string(j+6);
    str1=join(['NNtracks' i 'txt'], ["-", "."]);
    str2=join(['NNtime-sol' i 'txt'], ["-", "."]);
    NNtracks(:,j)=load(str1);
    NNtimesoltrack(:,:,j)=load(str2);
end
%%%%%%%%%%%%%%%%%%%%%%%%

% Tempo médio e track média total dos 20 problemas juntos
NNtmed=sum(NNtimesoltrack(:,2,:));
NNtsum=sum(NNtmed)/4000;
NNtrsum=sum(NNtracks);
NNtrmed=sum(NNtrsum)/1248000;
NNtxtemptra=sum(NNtmed)/sum(NNtrsum);
% 0,0238 ms/track = 23,8 microsegundos/track

for a=1:8 %Problemas 1 - 8
    
    for p=1:500 %100*5 testes
        NNtimesoltrack(p,4,a)=sum(NNtracks(312*(p-1)+1:312*p,a)); %Total de tracks700 em cada teste
    end

%separando o tempo e a solucão e arrumando os dados (experimento x teste)

    for i=1:100
        for j=1:5
%             Temp(i,j,a)=timesoltrack(5*(i-1)+j,2,a);  %tempo
            Sol(i,j,a)=NNtimesoltrack(5*(i-1)+j,3,a);  %solucão
   NNtxtemptra=sum(NNtmed)/sum(NNtrsum);
                                
            if NNtimesoltrack(5*(i-1)+j,3,a)==313 % TRASOL é a matriz do número de iteracões na raiz que é solucão em CADA TESTE (CONSIDERA OS 500 TESTES EM CADA PROBLEMA)
                NNTRASOL(5*(i-1)+j,a)=NaN; % Caso em que não encontra a solucão
            else
                 NNTRASOL(5*(i-1)+j,a)=NNtracks(1560*(i-1)+312*(j-1)+NNtimesoltrack(5*(i-1)+j,3,a)+1,a); % soma 1 pq as raízes vão 0-311
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

for a=1:8
    NNtime(:,a)=NNtimesoltrack(:,2,a);
    NNsolu(:,a)=NNtimesoltrack(:,3,a);
    NNtotra(:,a)=NNtimesoltrack(:,4,a); % total de iteracões
end

% Alternativa para criar array de strings com números -+-+-+-+-+

for t=1:8
    NNat1(t)=t+6;
%     at2(t)=t+10;
end
NNstra1=string(NNat1);
% Nstra2=string(at2);

%%%%%%% -+-+-+-+-+-+-+-+-+-+-+-+

% Tempo
figure
boxplot(NNtime(:,1:8),NNstra1)
xlabel('Problema')
ylabel('Tempo (ms)')
title('Tempo de execucão minus-chicago para os Problemas-NN 7 a 14')


% Iteracões em cada raiz
figure
boxplot(NNtracks(:,1:8))
xlabel('Problema')
ylabel('Número de iteracões em cada raiz')
title('Número de iteracões em cada raiz no minus-chicago para os Problemas 7 a 14')


% Total de iteracões
figure
boxplot(NNtotra(:,1:8))
xlabel('Problema')
ylabel('Número total de iteracões')
title('Número total de iteracões no minus-chicago para os Problemas 7 a 14')


% Iteracões na raiz que é solucão
figure
boxplot(NNTRASOL(:,1:10))
xlabel('Problema')
ylabel('Número de iteracões')
title('Número de iteracões no minus-chicago para a determinacão da raiz que é solucão dos Problemas 1 a 10')

figure
boxplot(NNTRASOL(:,11:20),'label',Nstra2)
xlabel('Problema')
ylabel('Número de iteracões')
title('Número de iteracões no minus-chicago para a determinacão da raiz que é solucão dos Problemas 11 a 20')


%%% Verificando em quais testes na raiz que é solucão o número de iteracões
%%% ultrapassa o limite

for p=1000:-100:300
    NSol=NNTRASOL;
    for r=1:20
        auxN1=isnan(NNTRASOL(:,r));
        aux1=sum(auxN1(:,1)==1);
        for s=1:500
            if NNTRASOL(s,r)>p % ERRO 
               NSol(s,r)=NaN;
            end
        end
        auxN2=isnan(NSol(:,r));
        aux2=sum(auxN2==1);
        NSRT(r,(1000-p)/100+1)=aux2-aux1;
%         NSRT(r,6)=aux2; %TOTAL DE TESTES QUE FALHAM POR QQ MOTIVO para cada p
    end
end
            
       
figure
    bar(transpose(NSRT))

  

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


