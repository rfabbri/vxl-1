
if(num_attachments_last==1 && ~no_init_junction)
    rb_id = arr_branch_id_last(1,1);
    target_curve = curve_graph{rb_id,1};
    target_size = size(target_curve,1);
    target_lock_init = lock_map(rb_id,1);
    target_lock_final = lock_map(rb_id,2);
    size_beginning_branch = arr_junction_indices_last(rb_id,1);
    size_ending_branch = target_size - arr_junction_indices_last(rb_id,1) + 1;
     
    %If we're close to the
    %beginning on this branch and
    %If our native branch is not long 
    %enough, and the new branch attaches to 
    %only 1 existing branch, then elongate
    
    if(size_beginning_branch<size_ending_branch && ~target_lock_init && (size_beginning_branch<3 || get_length(target_curve(1:arr_junction_indices_last(rb_id,1),:)) < branch_length_threshold))
        target_curve = [flipud(queryCurve(new_branch_start:ms-1,:)); target_curve(arr_junction_indices_last(rb_id,1):target_size,:)];
        curve_graph{rb_id,1} = target_curve; 
        is_elongated = 1;
    
        %Set the locks
        lock_map(rb_id,1) = 0;
       
        %TODO: Adjust the content of the merged branch
        curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
        
        %If we're close to the ending and      
        %If our native branch is not long 
        %enough, and the new branch attaches to 
        %only 1 existing branch, then elongate
    
    elseif(size_ending_branch<size_beginning_branch && ~target_lock_final && (size_ending_branch<3 || get_length(target_curve(arr_junction_indices_last(rb_id,1):target_size,:)) < branch_length_threshold))
   
        target_curve = [target_curve(1:arr_junction_indices_last(rb_id,1),:); queryCurve(new_branch_start:ms-1,:)];
        curve_graph{rb_id,1} = target_curve; 
        is_elongated = 1;
       
        %Set the locks
        lock_map(rb_id,2) = 0;
       
        %TODO: Adjust the content of the merged branch
        curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
    end
end