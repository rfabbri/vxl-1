clear all
close all

format long

for i=1:9
    theta(i)=2*pi*rand(1);
    coordtg(i,1)=cos(theta(i));
    coordtg(i,2)=sin(theta(i));
end
coordtg
