function edgegradmap = edgegrad(edgelist, gradmap)

[rownum, colnum] = size(gradmap);
edgegradmap = sparse(rownum, colnum);
for i = 1:length(edgelist)
    numpts = size(edgelist{i},1);
    for j = 1:numpts
        y = round(edgelist{i}(j,1));
        x = round(edgelist{i}(j,2));
        edgegradmap(y,x) = gradmap(y,x);
    end
end