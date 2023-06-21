%Análise dos experimentos realizados com 140 problemos gerados de forma aleatória
%DEZ22

clear all
close all

addpath(genpath('NOVOtests-tracks'));

aux=0;
for np=1:140 %número de problemas
    i=string(np);
    str1=join(['sol-P' i 'txt'], ["", "."]); %Quando não encontra a solucão escreve 313
    str2=join(['realsol-P' i 'txt'],["", "."]);
    str3=join(['tracks-P' i 'txt'],["", "."]); %O arquivo tracks-i.txt tem 1 coluna com o número de iteracões em cada raiz
    str4=join(['gammas-P' i 'txt'],["", "."]);
    sol(500*(np-1)+1:500*np,:)=load(str1);
    real=load(str2);
    raizrealN(aux+1:aux+length(real),:)=real; %raízes reais que não são solucão
    aux=aux+length(real);
    tracks(312*500*(np-1)+1:312*500*np,:)=load(str3);
    randoms(248*500*(np-1)+1:248*500*np,:)=load(str4);  % 248=(56+12+56)*2) - parametros+gammas+parametros*gammas dos sistemas inicial e objetivo
end

trackNsol=tracks; % trackNsol tem o número de iteracões nas raízes que Não são solucão
 for i=1:500*np
      sol(i,2)=sol(i,1)==313; 
      if sol(i,1)~=313
          trackNsol((i-1)*312+sol(i,1)+1,1)=NaN;
          tracksol(i,1)=tracks((i-1)*312+sol(i,1)+1,1); %# de iteracoes na raiz que é solucao no teste i
      else
          tracksol(i,1)=NaN;
      end
 end

 % Boxplot do número de iteracões nas raízes que SÃO e nas que NÃO SÃO solucão 
 
 for i=1:np
        tracksolP(:,i)=tracksol((i-1)*500+1:(i-1)*500+500,1);
        trackNsolP(:,i)=trackNsol((i-1)*312*500+1:312*500*i,1);
 end
 
 [a,b]=min(tracksolP);team
 [c,d]=max(tracksolP);
 
 for i=1:np
     s=string(i);
     j=join(['P' s],[""]);
     strp(i)=j;
 end
     
 for i=1:np
     xt(i)=i+0.03;
     yt(i)=a(i)+7;
     strt(i)=string(a(i));
 end
 
figure
boxplot(tracksolP(:,1:70),strp(1:70));
hold on
plot(a, 'o','MarkerSize', 7, 'MarkerFaceColor', 'r');
hold off
text(xt(1:70),yt(1:70),strt(1:70))
ylabel('\fontsize{18}Number of iterations per root')
xlabel('\fontsize{18}Problem')
title('\fontsize{20}Time spent in paths leading to ground-truth solution')

figure
boxplot(tracksolP(:,71:140),strp(71:140));
hold on
plot(a, 'o','MarkerSize', 7, 'MarkerFaceColor', 'r');
hold off
text(xt(71:140),yt(71:140),strt(71:140))
ylabel('\fontsize{18}Number of iterations per root')
xlabel('\fontsize{18}Problem')
title('\fontsize{20}Time spent in paths leading to ground-truth solution')

figure
boxplot(trackNsolP(1:70),strp(1:70));
ylabel('\fontsize{18}Number of iterations per root')
xlabel('\fontsize{18}Problem')
title('\fontsize{20}Time spent in paths leading to undesired roots')

figure
boxplot(trackNsolP(71:140),strp(71:140));
ylabel('\fontsize{18}Number of iterations per root')
xlabel('\fontsize{18}Problem')
title('\fontsize{20}Time spent in paths leading to undesired roots')






aux1=1;
aux2=1;
for i=1:length(raizrealN)
    if raizrealN(i,1)==313
        idreal(aux1,1)=i-1; %posicao da última raiz real em cd teste
        idreal(aux1,2)=aux1; % id do teste, considerando todos os problemas juntos
        idreal(aux1,3)=i-aux2; % # de raízes reais que nao sao solucao em cada teste
        aux1=aux1+1;
        aux2=i+1;
    end
end

% SEGUIR AJUSTE DAQUI
aux=1;
for r=1:500*np % # do teste, considerando todos os problemas juntos
    for s=aux:idreal(r,1) %intervalo das raízes reais do mesmo teste
        raizrealN(s,3)=tracks(raizrealN(s,1)+1+312*(idreal(r,2)-1)); % # iteracões na raiz
    end
    aux=idreal(r,1)+2;
end

%Matriz de todas as raízes reais por problema
aux1=idreal(500,1);
aux2=1;
for i=1:nt-1
   raizreal1000P(1:500,3*i-2)=Sol1000((i-1)*500+1:500*i,2);
   raizreal1000P(1:500,3*i-1)=10*ones(500,1);
   raizreal1000P(1:500,3*i)=tracksolP(:,i);
   raizreal1000P(501:aux1+500,(i-1)*3+1:3*i)=raizrealN(aux2:idreal(500*i,1),:);
   aux1=idreal(500*(i+1),1)-idreal(500*i);
   aux2=idreal(500*i,1)+1;
end
raizreal1000P(1:500,3*nt-2)=Sol1000((nt-1)*500+1:500*nt,2);
raizreal1000P(1:500,3*nt-1)=10*ones(500,1);
raizreal1000P(1:500,3*nt)=tracksolP(:,nt);
raizreal1000P(501:idreal(500*nt,1)-idreal(500*(nt-1),1)+500,(nt-1)*3+1:3*nt)=raizrealN(idreal(500*(nt-1),1)+1:idreal(500*nt,1),:);

for i=2:3:420
    a=find(raizreal1000P(:,i));
    b=a(length(a),1)+1;
    for j=b:length(raizreal1000P)
    raizreal1000P(j,i-1:i+1)=[NaN NaN NaN];
    end
end
for i=1:500
    for j=1:3:420
        if raizreal1000P(i,j)==313
            raizreal1000P(i,j)=NaN;
        end
    end
end


%matriz de todas as raízes reais juntas
for i=1:nt
    raizreal1000((i-1)*length(raizreal1000P)+1:i*length(raizreal1000P),1:3)=raizreal1000P(1:end,3*i-2:3*i);
end
