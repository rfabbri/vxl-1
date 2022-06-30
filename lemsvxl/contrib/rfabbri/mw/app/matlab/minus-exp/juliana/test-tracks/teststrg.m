clear all
close all

% i=string(4)
% b=join(['tracks' '700' i 'txt'], ["", "-", "."])

for j=1:5
    i=string(j);
    b=join(['tracks700-' i 'txt'], ["", "."]);
    tracks700(:,j)=load(b);
end