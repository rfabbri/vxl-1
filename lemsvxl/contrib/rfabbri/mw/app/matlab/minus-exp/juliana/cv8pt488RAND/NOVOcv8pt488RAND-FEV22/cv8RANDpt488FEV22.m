%Insercão de ruído gaussiano com sigma 200 nas coordenadas do ponto 488 nas câmeras 42, 54
%e 62
clear all
close all

%matriz das coordenadas do pt 488 nas câmeras 42, 54, 62
M488=[188.9968147583066127 330.99260529244764939;
239.87635969387076784 87.172102114226248659;
230.07341479310812815 354.83747944660694884] 

%matriz aleatória, distribuicão normal com média 0 e sigma 20
Norm=20*randn(30,2);  

%matriz de coordenadas de 10 pontos (em cada câmera) gerados a partir do pt
%488 com ruído da gaussiana com média 0 e sigma 20
for j=1:10
        MRAND(3*j-2:3*j,:)=M488(1:3,:)+Norm(3*j-2:3*j,:)
end

writematrix(MRAND, 'novaMRAND488.txt', 'Delimiter', 'space');