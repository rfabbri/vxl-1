function pts3dcellrs = cellresample(pts3dcell, rate)

imnum = length(pts3dcell);
pts3dcellrs = cell(imnum,1);
for i = 1:imnum
    [N, S, V] = size(pts3dcell{i});
    pts3dcellrs{i} = zeros(N,ceil(S/rate),V);
    cnt = 1;
    for s = 1:rate:S
        pts3dcellrs{i}(:,cnt,:) = pts3dcell{i}(:,s,:);
        cnt = cnt + 1;
    end
end
