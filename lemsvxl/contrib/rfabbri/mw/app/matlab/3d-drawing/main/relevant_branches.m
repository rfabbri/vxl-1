num_relevant_branches = size(arr_branch_id_cur,2);
for br=1:num_relevant_branches
    rel_br_id = arr_branch_id_cur(1,br);
                  
    %Store the junction index, meaning the
    %index into the branch samples that
    %indicates a junction, for each branch
                                    
    target_curve = curve_graph{rel_br_id,1};
    target_size = size(target_curve,1);
    if(arr_prev_sample(rel_br_id,1)==1 && arr_outside_init_range(rel_br_id,1))
        curve_graph{rel_br_id,1} = [query_sample; target_curve];
        arr_junction_indices(rel_br_id,1) = 1;
    elseif(arr_next_sample(rel_br_id,1)==target_size && arr_outside_final_range(rel_br_id,1))
        curve_graph{rel_br_id,1} = [target_curve; query_sample];
        arr_junction_indices(rel_br_id,1) = target_size+1;
    else
        curve_graph{rel_br_id,1} = [target_curve(1:arr_prev_sample(rel_br_id,1),:); query_sample; target_curve(arr_next_sample(rel_br_id,1):target_size,:)];
        arr_junction_indices(rel_br_id,1) = arr_prev_sample(rel_br_id,1)+1;
    end
                                    
    %Adjust the contents of the target
    %branch after this operation
    curve_graph_content{rel_br_id,1} = unique([curve_graph_content{rel_br_id,1}; [all_views(1,mrv)+1 cc]],'rows');
end