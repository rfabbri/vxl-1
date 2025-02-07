for b=1:num_branches

    if(size(curve_graph{b,1})<=1)
        continue;
    end
											     
    target_curve = curve_graph{b,1};
    target_size = size(target_curve,1);
    
    ind_last = find(arr_branch_id_last==b);
    ind_cur = find(arr_branch_id_cur==b);
    
    %If found in both arrays, this
    %branch will be broken in two
    %places
    if((~isempty(ind_last)) && (~isempty(ind_cur)) && (~no_init_junction))
    
        initialize_target_curves
																									          
        cur_locks = lock_map(b,:);
        cur_lock_init = cur_locks(1,1);
        cur_lock_final = cur_locks(1,2);
												         
        %Add the broken pieces
        curve_graph{b,1} = target_curve2;
        %Set the junction locks for the new branches
        lock_map(rb_id,:) = [1 1];
        
        curve_graph = [curve_graph; cell(1,1)];
        cg_size = size(curve_graph,1);
        curve_graph{cg_size,1} = target_curve1;
        %Set the junction locks for the new branches
        lock_map = [lock_map; [cur_lock_init 1]];
        set_new_branch
        
        curve_graph = [curve_graph; cell(1,1)];
        cg_size = size(curve_graph,1);
        curve_graph{cg_size,1} = target_curve3;
        %Set the junction locks for the new branches
        lock_map = [lock_map; [1 cur_lock_final]];
        set_new_branch
        equiv_table_cur{cg_size,1} = [equiv_table_cur{cg_size,1} cg_size-1];
        equiv_table_cur{cg_size-1,1} = [equiv_table_cur{cg_size-1,1} cg_size];
                                                         
        %Otherwise, break at the only
        %existing connection
    
    elseif(~isempty(ind_last) && (~no_init_junction))
        
        %Junction breaks the existing branch it's
        %being attached to
        target_curve1 = target_curve(1:arr_junction_indices_last(b,1),:);
        target_curve2 = target_curve(arr_junction_indices_last(b,1):target_size,:);
        
        %Add the broken pieces
        curve_graph{b,1} = target_curve1;
        curve_graph = [curve_graph; cell(1,1)];
        cg_size = size(curve_graph,1);
        curve_graph{cg_size,1} = target_curve2;
        %Set the junction locks for the new branches
        cur_locks = lock_map(b,:);
        cur_lock_init = cur_locks(1,1);
        cur_lock_final = cur_locks(1,2);
        lock_map(b,:) = [cur_lock_init 1];
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
        
    elseif(~isempty(ind_cur))
        
        %Junction breaks the existing branch it's
        %being attached to
        target_curve1 = target_curve(1:arr_junction_indices(b,1),:);
        target_curve2 = target_curve(arr_junction_indices(b,1):target_size,:);
        
        %Add the broken pieces
        curve_graph{b,1} = target_curve1;
        curve_graph = [curve_graph; cell(1,1)];
        cg_size = size(curve_graph,1);
        curve_graph{cg_size,1} = target_curve2;
        %Set the junction locks for the new branches
        cur_locks = lock_map(b,:);
        cur_lock_init = cur_locks(1,1);
        cur_lock_final = cur_locks(1,2);
        lock_map(b,:) = [cur_lock_init 1];
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
end