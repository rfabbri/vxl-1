function edgeormap = edgetangent(edgelist,colnum,rownum)

edgeormap = sparse(rownum, colnum);
for i = 1:length(edgelist)
    numpts = size(edgelist{i},1);
    xsp = csaps(1:numpts,edgelist{i}(:,2),0.9);
    ysp = csaps(1:numpts,edgelist{i}(:,1),0.9);
    dxsp = fnder(xsp); dysp = fnder(ysp);
    dx = fnval(dxsp,1:numpts); dy = fnval(dysp, 1:numpts);
    theta = atan2(dy,dx);
    for j = 1:numpts
        edgeormap(round(edgelist{i}(j,1)),round(edgelist{i}(j,2))) = theta(j);
    end
end
