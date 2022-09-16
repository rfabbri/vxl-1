%Insercão de ruído gaussiano com sigma 0.1 e 0.5 nas coordenadas do ponto 488 nas câmeras 42, 54
%e 62
clear all
close all

%matriz das coordenadas do pt 488 nas câmeras 42, 54, 62
M488=[188.9968147583066127 330.99260529244764939;
239.87635969387076784 87.172102114226248659;
230.07341479310812815 354.83747944660694884] 

%matriz aleatória, distribuicão normal com média 0 e sigma 0.1 e 0.5
Norm1=0.1*randn(30,2);  
Norm5=0.5*randn(30,2); 

%matriz de coordenadas de 10 pontos (em cada câmera) gerados a partir do pt
%488 com ruído da gaussiana com média 0 e sigma 0.1 e 0.5
for j=1:10
        MRAND1(3*j-2:3*j,:)=M488(1:3,:)+Norm1(3*j-2:3*j,:);
        MRAND5(3*j-2:3*j,:)=M488(1:3,:)+Norm5(3*j-2:3*j,:);
end

writematrix(MRAND1, 'MRAND1pt488mar22.txt', 'Delimiter', 'space');
writematrix(MRAND5, 'MRAND5pt488mar22.txt', 'Delimiter', 'space');
