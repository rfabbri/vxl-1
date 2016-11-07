function Amatrix = cell2matrix(Acell)

celllength = length(Acell);
N = 0;
dim = 0;
for i = 1:celllength
    [N0, J, V] = size(Acell{i});
    N = N + J*V;
    dim = N0;
end

Amatrix = zeros(dim,N);
cnt = 0;
for i = 1:celllength
    [nouse, J, V] = size(Acell{i});
    for j = 1:J
        for v = 1:V
            cnt = cnt+1;
            Amatrix(:,cnt) = Acell{i}(:,j,v);
        end
    end
end