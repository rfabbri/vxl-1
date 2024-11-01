converge_flags = cell(numIM,1);
for cv=1:numIM
    if(isempty(clusters{cv,1}))
        continue;
    end
    converge_flags{cv,1} = cell(1,size(clusters{cv,1},2));
    for ccrv = 1:size(clusters{cv,1},2)
        cid = clusters{cv,1}(1,ccrv);
        curve = all_recs{cv,1}{1,cid};
        curve_size = size(curve,1);
        converge_flags{cv,1}{1,ccrv} = zeros(curve_size,1);
    end
end