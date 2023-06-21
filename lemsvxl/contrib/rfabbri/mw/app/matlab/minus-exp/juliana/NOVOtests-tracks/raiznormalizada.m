clear all
close all

tracks=load('tracks-P2.txt');
sol=load('sol-P2.txt');
raiz=load('raiz.txt');

addpath(genpath('NOVOtest-tracks'));

for i=1:500
    if sol(i,1)~=313
        root(i,1:14)=transpose(raiz((i-1)*312*14+sol(i)*14+1:(i-1)*312*14+sol(i)*14+14,1));
    else
        root(i,1:14)=NaN(1,14);
    end
end

for j=1:500
    nroot(j,1:4)=root(j,1:4)/(norm(root(j,1:4)));
    nroot(j,5:8)=root(j,5:8)/(norm(root(j,5:8)));
    nroot(j,9:11)=root(j,9:11)/(norm(root(j,9:11)));
    nroot(j,12:14)=root(j,12:14)/(norm(root(j,12:14)));
end

