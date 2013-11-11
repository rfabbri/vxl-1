function [pts3dcell, normalcell] = removeoutlier(pts3drawcell, normalrawcell, maskcell)

imnum = length(maskcell);
pts3dcell = cell(imnum,1);
normalcell = cell(imnum,1);
for i = 1:imnum
    [S, V] = size(maskcell{i});
    validS = sum(sum(maskcell{i},2)==V);
    pts3dcell{i} = zeros(3,validS,V);
    normalcell{i} = zeros(3,validS,V);
    cnt = 1;
    for s = 1:S
        if all(maskcell{i}(s,:) ~= 0)
            pts3dcell{i}(:,cnt,:) = pts3drawcell{i}(:,s,:);
            normalcell{i}(:,cnt,:) = normalrawcell{i}(:,s,:);
            cnt = cnt+1;
        end
    end
end