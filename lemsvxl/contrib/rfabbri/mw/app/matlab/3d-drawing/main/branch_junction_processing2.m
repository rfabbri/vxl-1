%Go over each relevant branch to
%process the junctions of each
for rb=1:num_attachments_cur

    rb_id = arr_branch_id_cur(1,rb);
    
    target_curve = curve_graph{rb_id,1};
    target_size = size(target_curve,1);
											     
    %Break only if the
    %relevant segment remains
    %if(arr_junction_indices(rb_id,1)<init_target_min || arr_junction_indices(rb_id,1)>init_target_max)
    %continue;
    %end
    at_least_one_branch_broken = 1;
											     
    size_beginning_branch = arr_junction_indices(rb_id,1);
    size_ending_branch = target_size - arr_junction_indices(rb_id,1) + 1;
    
    %Junction breaks the existing branch it's
    %being attached to
    target_curve1 = target_curve(1:arr_junction_indices(rb_id,1),:);
    target_curve2 = target_curve(arr_junction_indices(rb_id,1):target_size,:);
    
    %Add the broken pieces
    curve_graph{rb_id,1} = target_curve1;
    curve_graph = [curve_graph; cell(1,1)];
    cg_size = size(curve_graph,1);
    curve_graph{cg_size,1} = target_curve2;
    %Set the junction locks for the new branches
    cur_locks = lock_map(rb_id,:);
    cur_lock_init = cur_locks(1,1);
    cur_lock_final = cur_locks(1,2);
    lock_map(rb_id,:) = [cur_lock_init 1];
    lock_map = [lock_map; [1 cur_lock_final]];
											     
    %Set the contents of the new branch
    curve_graph_content = [curve_graph_content; cell(1,1)];
    curve_graph_content{cg_size,1} = curve_graph_content{rb_id,1};
    %Set the correspondence flags for the new branch
    merged_corresp_mask_all = [merged_corresp_mask_all; cell(1,1)];
    merged_corresp_mask_all{cg_size,1} = merged_corresp_mask_all{rb_id,1};
    %Set the equivalence table for the new branch
    equiv_table_cur{rb_id,1} = [equiv_table_cur{rb_id,1} cg_size];
    equiv_table_cur = [equiv_table_cur; cell(1,1)];
    equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} rb_id];
end
										 
%Add the junction if it's not added already
if(~is_junction_saved && at_least_one_branch_broken)
    all_junctions = [all_junctions; queryCurve(ms,:)];
    is_junction_saved = 1;
end