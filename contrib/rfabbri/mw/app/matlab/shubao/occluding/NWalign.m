function [align1, align2] = NWalign(vec1, vec2)

penalty = 0.4;

num1 = length(vec1);
num2 = length(vec2);
F = zeros(num1+1,num2+1);

% computing the F matrix
% The Fij will be assigned to be the optimal score for the alignment fo the first i elements in vec1 and the first j elemnts in vec2.
F(1,:) = 0:-1:-num2;
F(:,1) = (0:-1:-num1)';
for i = 2:num1+1
    for j = 2:num2+1
        choice1 = F(i-1,j-1) + similarity(vec1(i-1),vec2(j-1));
        choice2 = F(i-1,j) + penalty;
        choice3 = F(i,j-1) + penalty;
        F(i,j) = max([choice1 choice2 choice3]);
    end
end

% Once the F matrix is computed, note that the bottom right hand corner of the matrix is the maximum score for any alignments. To compute which alignment actually gives this score, you can start from the bottom right, and compare the value which the three possible sources (choice1, choice2, and choice3 above) to see which it came from.

align1 = [];
align2 = [];
i = num1+1;
j = num2+1;
while (i>=2 && j>=2)
    score = F(i,j);
    scorediag = F(i-1,j-1);
    scoreup = F(i,j-1);
    scoreleft = F(i-1,j);
    if (score == scoreleft + penalty)
        align1 = [i-1 align1];
        align2 = [0 align2];
        i = i-1;
    elseif (score == scoreup + penalty)
        align1 = [0 align1];
        align2 = [j-1 align2];
        j = j-1;
    else
        align1 = [i-1 align1];
        align2 = [j-1 align2];
        i = i-1;
        j = j-1;
    end
end
while (i>=2)
    align1 = [i-1 align1];
    align2 = [0 align2];
    i = i-1;
end
while (j>=2)
    align1 = [0 align1];
    align2 = [j-1 align2];
    j = j-1;
end

function simval = similarity(a1,a2)
p = a1.relpos-a2.relpos;
g = a1.relgrad-a2.relgrad;
diff = mod(a1.or-a2.or,pi);
o = min(pi-diff,diff)/(pi/2);
simval = 1-sqrt(.5*p^2+.3*g^2+.2*o^2);
% fprintf(1,'%f\t%f\t%f\t%f\n',p,g,o,simval);
