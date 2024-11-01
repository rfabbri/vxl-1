initialize_corresp_masks

for av=1:numViews
    cur_clusters = clusters{all_views(1,av)+1,1};
    if(isempty(cur_clusters))
        continue;
    end
    num_curves_in_cluster = size(cur_clusters,2);
    for cc=1:num_curves_in_cluster

        create_corresp_masks

        update_corresp_masks

        post_process_corresp_masks_fill_gaps

        post_process_corresp_masks_remove_outliers

        all_corresp_masks{all_views(1,av)+1,1}{1,cc} = corresp_masks;

        fuse_corresp_masks
    
    end
end