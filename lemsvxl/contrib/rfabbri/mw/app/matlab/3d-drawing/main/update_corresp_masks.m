queryAlignment = alignment_curves{all_views(1,av)+1,1}{1,cc};
for ss=1:size(queryAlignment,1)
    cur_alignment = queryAlignment{ss,1};
    if(isempty(cur_alignment))
        continue;
    end
    view_curve = unique(cur_alignment(:,1:2),'rows');
    num_corresps = size(view_curve,1);
    %List of [viewID curveID] that need to be updated
    update_map = zeros(num_corresps,2);
    for crsp=1:num_corresps
        view = view_curve(crsp,1);
        corresp_curveID = view_curve(crsp,2);
        clust_index = find(clusters{view,1}==corresp_curveID);
        if(isempty(clust_index))
            continue;
        end
        corresp_masks{view,1}(ss,clust_index) = 1;
        update_map(crsp,:) = [view corresp_curveID];
    end
end