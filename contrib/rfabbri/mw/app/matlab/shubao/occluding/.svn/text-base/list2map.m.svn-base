function map = list2map(list,rownum,colnum)

map = sparse(rownum,colnum);
for i = 1:length(list)
    numpts = size(list{i},1);
    y = round(list{i}(:,1));
    x = round(list{i}(:,2));
    for j = 1:numpts
        map(y(j),x(j)) = 1;
    end

end
