if(merged_corresp_mask_cumulative(ms,1))
    query_sample = queryCurve(ms,:);
    min_branch_dist = 10^10;
                             
    for br=1:num_branches
        %If branch has only 1 point, this
        %operation is not meaningful
        if(size(curve_graph{br,1},1)<=1)
            continue;
        end
                                 
        %If the current branch has no
        %correspondence, move onto the next one
        if(~merged_corresp_mask_all{br,1}(ms,1))
            continue;
        end
                                 
        [arr_closest_pt(br,:),arr_outside_init_range(br,1),arr_outside_final_range(br,1),arr_prev_sample(br,1),arr_next_sample(br,1)] = find_closest_point_on_curve(curve_graph{br,1},query_sample);
        arr_branch_dist(br,1) = norm(arr_closest_pt(br,:) - query_sample);
        %Only save the ID of the closest branch,
        %all other attributes are saved.
        if(arr_branch_dist(br,1) < min_branch_dist)
            closest_branch = br;
            min_branch_dist = arr_branch_dist(br,1);
        end
    end
    if(min_branch_dist > sample_merge_threshold)
        merged_corresp_mask_cumulative(ms,1) = 0;
    end
end
                         
if(dist_to_prev_sample>sample_break_threshold && ~merged_corresp_mask_cumulative(ms,1))
    no_init_flag = 1;
end