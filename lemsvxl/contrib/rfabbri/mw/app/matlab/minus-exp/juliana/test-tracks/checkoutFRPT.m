%Verifica se na matriz FRePt há problemas com câmeras ou pontos repetidos e
%se há problemas iguais

clear all
close all

frpt=load('FRePT.txt');
a=0;
for i=1:1000
    if (frpt(i,1)==frpt(i,2))||(frpt(i,1)==frpt(i,3))||(frpt(i,2)==frpt(i,3))
        a=a+1;
        erro(a,1)=i;
    elseif (frpt(i,4)==frpt(i,5))||(frpt(i,4)==frpt(i,6))||(frpt(i,5)==frpt(i,6))
        a=a+1;
        erro(a,1)=i;
    end
end

writematrix(erro, 'prob_com_erro.txt','Delimiter','space');

frpt1=frpt;
for j=30:-1:1
    frpt1(erro(j,1),:)=[];
end

writematrix(frpt1,'FRePTcorrigido.txt', 'Delimiter', 'space');
clear a;
a=1;
for p=1:970
    for q=1:970
        if p~=q
            for r=1:3
                l(1,r)=(frpt1(p,r)==frpt1(q,1))||(frpt1(p,r)==frpt1(q,2))||(frpt1(p,r)==frpt1(q,3));
                l(1,r+3)=(frpt1(p,r+3)==frpt1(q,4))||(frpt1(p,r+3)==frpt1(q,5))||(frpt1(p,r+3)==frpt1(q,6));
            end
            if l(1,:)==[1 1 1 1 1 1]
                erro1(a,1)=p;
                erro1(a,2)=q;
                a=a+1;
            end
        end
    end
end
% NÃO HÁ PROBLEMAS REPETIDOS!!!
                    