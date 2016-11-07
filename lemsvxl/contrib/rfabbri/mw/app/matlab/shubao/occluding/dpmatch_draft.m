function match = dpmatch(vec1, vec2)

N1 = length(vec1); N2 = length(vec2);
costs(1,1) = 0;
for ind1 = 2:N1
    for ind2 = 1:N2
        if (constraints(ind1,ind2,N1,N2) == 0)
            continue;
        end
        bestcost = inf;
        for k = ind2-1:-1:1
            if (constraints(ind1-1,k,N1,N2)==0)
                break;
            end
            x = costs(ind1-1,k); %%%
            if (x<bestcost)
                bestcost = x+jumpcost;
                bestpoint = [ind1-1;k];
            end
        end
        for k = ind1-1:-1:1
            if (underconstraint(k,ind2-1,N1,N2)==0)
                break;
            end
            x = costs(k,ind2-1);
            if (x<bestcost)
                bestcost = x+jumpcost;
                bestpoint = [k; ind2-1];
            end
        end
        cost_ind12 = ;
        costs(ind1,ind2) = bestcost+cost_ind12;
        backpointers(:,ind1,ind2) = bestpoint;
    end
end

bestpath(:,1) = [N1;N2];
leng = 1;
while(any(bestpath(:,leng) ~= [1;1]))
    bestpath(:,leng+1) = backPointers(:,bestpath(1,leng),bestpath(2,leng));
    leng = leng + 1;
end
idx_matched = bestpath(:,end:-1:1);
 
function YorN = underconstraint(ind1,ind2,N1,N2)
if (ind1<1 || ind1>N1 || ind2<1 || ind2>N2) 
	YorN = 0; return;
end
if ((ind1==1 || ind1==N1 || ind2==1 || ind2==N2) && (ind1~=ind2))
	YorN = 0; return;
end
YorN = 1;