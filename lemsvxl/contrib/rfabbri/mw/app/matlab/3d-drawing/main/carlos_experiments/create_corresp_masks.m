curveID = cur_clusters(1,cc);
queryCurve = all_recs_iter{all_views(1,av)+1,1}{1,curveID};
numSamples = size(queryCurve,1);
corresp_masks = cell(numIM,1);
for vcrsp=1:numIM
    if(~isempty(clusters{vcrsp,1}))
        corresp_masks{vcrsp,1} = zeros(numSamples,size(clusters{vcrsp,1},2));
    end
end