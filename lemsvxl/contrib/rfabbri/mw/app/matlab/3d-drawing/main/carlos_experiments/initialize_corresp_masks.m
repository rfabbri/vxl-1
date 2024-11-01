all_corresp_masks = cell(numIM,1);
for acm=1:numIM
    if(~isempty(clusters{acm,1}))
        num_clustered_curves = size(clusters{acm,1},2);
        all_corresp_masks{acm,1} = cell(1,num_clustered_curves);
    end
end