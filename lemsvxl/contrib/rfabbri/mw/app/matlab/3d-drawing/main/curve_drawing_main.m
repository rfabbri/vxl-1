clear all;%OK
close all;%OK
definitions_12

%-------------------------------------------------------------------------------
% Input data -------------------------------------------------------------------
%-------------------------------------------------------------------------------
colors = distinguishable_colors(numIM);
all_recs = cell(numIM,1);
all_nR = zeros(numIM,1);
all_links_3d = cell(numIM,1);
all_offsets_3d = cell(numIM,1);
all_edge_support_3d = cell(numIM,1);
all_flags = cell(numIM,1);
all_num_im_contours = zeros(numIM,1);

load_curve_sketch_without_associations
% mat files are not used nor called in the rest of the code 
% load edge-curve-index_yuliang_pavilion-mixed.mat;
% load edge-curve-offset_yuliang_pavilion-mixed.mat;
load_edge_and_curve_files

% clear all;
% close all;

%save intermediate-pavilion-mixed-half_12.mat;
%load intermediate-pavilion-mixed-half_12.mat;
% Intermediate consistes of all curves and 3D curve sketch input
% So actually we can use intermediate-* data as input for the 3D drawing
% But this main file would only work for _12

%-------------------------------------------------------------------------------
% 3D Drawing itself ------------------------------------------------------------
%-------------------------------------------------------------------------------

%As curves are modified, the modified versions go here.
all_recs_iter = all_recs;

weird_edges = [];
counter = 0;

num_colors = 0;

for colv=1:numIM
    num_colors = num_colors + all_nR(colv,1);
end

colors = distinguishable_colors(5000);
colors(1,:) = [];
color_counter = 0;

complete_curve_graph = [];
complete_lock_map = [];
all_junctions = [];

%Form the links between corresponding 3D curve samples
for fa=1:numViews
    fa_view = all_views(1,fa)+1;
    for crv = 1:all_nR(fa_view,1)

        %Zero out any flag that did not reach the minimum number of votes
        %required

        for v=1:numIM
           if(isempty(all_flags{v,1}))
               continue;
           end
           num_curves = size(all_flags{v,1},1);
           for c=1:num_curves
              if(all_flags{v,1}(c,1)<num_shared_edges)
                  all_flags{v,1}(c,1)=0;
              end
           end
        end

        if(all_flags{fa_view,1}(crv,1)==num_shared_edges)
            continue;
        end
        
        color_counter = color_counter+1;

        clusters = cell(numIM,1);
        process_flags = cell(numIM,1);

        %The ID of the initial seed/query curve where clustering will begin
        clusters{fa_view,1} = crv;
        process_flags{fa_view,1} = 0;
        
        extra_junctions = [];

        alignment_curves = cell(numIM,1);
        alignment_curves{fa_view,1} = cell(1,1);

        corr_native = cell(numIM,1);
        corr_other = cell(numIM,1);
        corr_colors = cell(numIM,1);

        corr_native{fa_view,1} = cell(1,1);
        corr_other{fa_view,1} = cell(1,1);
        corr_colors{fa_view,1} = cell(1,1);

        all_flags{fa_view,1}(crv,1) = num_shared_edges;
        keep_processing = 1;

        while(keep_processing)

            for av=1:numViews

                first_anchor = all_views(1,av);

                other_views = [];
                for ov=1:numViews
                    if(ov~=av)
                        other_views = [other_views all_views(1,ov)];
                    end
                end

                edge_support_3d = all_edge_support_3d{first_anchor+1,1};
                cur_cluster = clusters{first_anchor+1,1};
                cur_flags = process_flags{first_anchor+1,1};
                c_size = size(cur_cluster,2);

                for cc=1:c_size

                    queryID = cur_cluster(1,cc); 

                    if(cur_flags(1,cc))
                        continue;
                    end

                    cur_flags(1,cc) = 1;
                    process_flags{first_anchor+1,1} = cur_flags;

                    previous_clusters = clusters;
                    previous_flags = process_flags;

                    previous_corr_native = corr_native;
                    previous_corr_other = corr_other;
                    previous_corr_colors = corr_colors;               

                    queryCurve = all_recs{first_anchor+1,1}{1,queryID}; 
                    querySupport = edge_support_3d{queryID,1};
                    numSamples = size(queryCurve,1);

                    %Alignment curve
                    %viewid-curveid-sampleid-votes
                    queryAlignment = cell(numSamples,1);

                    %Views used in the alignment curve
                    %We use this to eliminate votes from narrow baseline views
                    queryAlignment_views = cell(numSamples,1);

                    computing_associations

                    queryAlignment_dense = cell(size(queryAlignment,1),1);
                    
                    filtering_alignmentCurve

                    start_clusters
                   
                    clusters = new_clusters;
                    process_flags = new_process_flags;

                    corr_native = new_corr_native;
                    corr_other = new_corr_other;
                    corr_colors = new_corr_colors; 

                    %Now the output should be written to vrml files

                    votes
                    alignment_curves{first_anchor+1,1}{1,cc} = queryAlignment_dense;

                end
            end  

            processing

        end

        %All correspondence info has been computed, now we need to
        %do the averaging
        averaging_iterations_done = 0;

        %Flags marking converged samples
        initialize_converge_flags
        
        %Process the query alignment to create boolean correspondence
        %masks for each other curve in the cluster
        correspondence_masks

        coll_motion1 = [];
        coll_motion2 = [];
        
        bundle_adjustment

        %Now that the cluster has converged, we will merge the curves at
        %overlapping segments

        processing_longest_curve

        merging_iterations_done=0;
        cntr = 0;
        while(~merging_iterations_done)
            cntr = cntr+1;
            
%             if(cntr>1)
%                 break;
%             end
            
            new_curve_merged = 0;
            
            %Problem curve
%             if(crv==5 && cntr==2)
%                 numViews=25;
%             end

            for mrv=1:numViews
                
                cur_clusters = clusters{all_views(1,mrv)+1,1};
                if(isempty(cur_clusters))
                    continue;
                end
                num_curves_in_cluster = size(cur_clusters,2);
                
%                 if(mrv==3)
%                     num_curves_in_cluster = 4;
%                 end
                
                for cc=1:num_curves_in_cluster
                    if(merge_flags{mrv,1}(1,cc))
                        continue;
                    end
                    curveID = cur_clusters(1,cc);
                    queryCurve = all_recs_iter{all_views(1,mrv)+1,1}{1,curveID};
                    cur_size = size(queryCurve,1);

                    query_corresp_masks = all_corresp_masks{all_views(1,mrv)+1,1}{1,cc};
                  
                    create_merged_corresp_mask

                    %Go over each sample, merge overlapping samples, break branches
                    new_branch_start = 0;
                    currently_traversing_branch = 0;
                    no_init_junction = 0;
                    all_new_branches = [];
                    all_new_locks = [];
                    
                    %A list that's filled everytime a merge happens
                    %The branch ids and the closest points are stored
                    last_merged_branches = [];
                    
                    %The branch ids that were used in the last merge
                    arr_branch_id_cur = [];
                    arr_branch_id_last = [];
                    arr_junction_indices_last = [];
                    arr_prev_sample_last = [];
                    arr_next_sample_last = [];
                    
                    %Equivalence table for broken branches in current and
                    %previous sample
                    
                    equiv_table_cur = [];
                    equiv_table_last = [];
                    
                    %If there is no correspondence on any sample, move on
                    if(all(~merged_corresp_mask_cumulative))
                        continue;
                    %Otherwise, mark this curve as merged
                    else
                        merge_flags{mrv,1}(1,cc) = 1;
                        new_curve_merged = 1;
                    end
                    
                    is_merged = 0;
                    
                    for ms=1:cur_size
                        
                         num_branches = size(curve_graph,1);
                        
                         %Arrays to store all relevant info
                         arr_outside_init_range=zeros(num_branches,1);
                         arr_outside_final_range=zeros(num_branches,1);
                         arr_prev_sample = zeros(num_branches,1);
                         arr_next_sample = zeros(num_branches,1);
                         arr_branch_dist = (-1).*ones(num_branches,1);
                         arr_closest_pt = zeros(num_branches,3);
                         arr_junction_indices = zeros(num_branches,1);
                         
                         equiv_table_cur = cell(num_branches,1);
                         
                         closest_branch = 0;
                         
                         %Current sample should add at most
                         %1 junction to the graph, even if
                         %it connects to more than 1 branch
                         is_junction_saved = 0;
                         is_last_junction_saved = 0;
                         
                         num_attachments_last = size(arr_branch_id_last,2);
                         
                         dist_to_prev_sample = 0;
                         no_init_flag = 0;
                         if(ms>1) 
                            dist_to_prev_sample = norm(queryCurve(ms,:)-queryCurve(ms-1,:));
                         end
                         
                         if(dist_to_prev_sample>sample_break_threshold && currently_traversing_branch)
                             query_branch_size = ms-new_branch_start+1;

                             %Break the existing branch
                             currently_traversing_branch = 0;

                             %If the whole query curve is a new branch,
                             %then it's not really a branch but rather
                             %an erroneously grouped 3D curve, which
                             %should now be broken free
                             if(isempty(arr_branch_id_last))
                                 new_branch = queryCurve(new_branch_start:ms-1,:);
                                 all_new_branches = [all_new_branches; cell(1,1)];
                                 anb_size = size(all_new_branches,1);
                                 all_new_branches{anb_size,1} = new_branch;
                                 all_new_locks = [all_new_locks; [0 0]];
                             else

                                 is_elongated = 0;
                                 %CHECK FOR ELONGATION AT (A): YES
                                 elongation_yes

                                 %CHECK FOR ELONGATION AT (A): NO
                                 %Otherwise this is a junction     
                                 elongation_no_junctions

                             end
                            arr_branch_id_last = [];
                            arr_junction_indices_last = [];
                            arr_prev_sample_last = [];
                            arr_next_sample_last = [];
                            num_attachments_last = 0;
                         end
                        
                         %If this sample is marked to have a correspondence
                         %Record all distances to all branches.
                         %Record closest branch distance and ID
                         %If this closest distance is still too far, remove
                         %the correspondence link
                         correspondence_validation
                                           
                         %A) If the sample has no link
                         if(~merged_corresp_mask_cumulative(ms,1))
                             %Ai) If the previous sample had a link, we
                             %start a new branch
                             if(~currently_traversing_branch)
                                new_branch_start = ms;
                                currently_traversing_branch = 1;
                                no_init_junction = no_init_flag;
                                
                             %Aii) If the previous sample did not have a
                             %link, but we reached the end, we need to
                             %finish the started branch and add it
                             
                             elseif(ms==cur_size)
                                 query_branch_size = ms-new_branch_start+1;
                                 
                                 %Break the existing branch
                                 currently_traversing_branch = 0;
                                 
                                 %If the whole query curve is a new branch,
                                 %then it's not really a branch but rather
                                 %an erroneously grouped 3D curve, which
                                 %should now be broken free
                                 if(isempty(arr_branch_id_last))
                                     
                                     new_branch = queryCurve(new_branch_start:ms,:);
                                     all_new_branches = [all_new_branches; cell(1,1)];
                                     anb_size = size(all_new_branches,1);
                                     all_new_branches{anb_size,1} = new_branch;
                                     all_new_locks = [all_new_locks; [0 0]];
                                 else
                                     is_elongated = 0;
                                     %CHECK FOR ELONGATION AT (A): YES
                                     elongation_yes

                                     %CHECK FOR ELONGATION AT (A): NO
                                     %Otherwise this is a junction     
                                     elongation_no_junctions

                                 end
                             end
                             %Aiii) Otherwise (if currently traversing a
                             %new branch, and if we're not at the end yet)
                             %do nothing
                             
                         %B) If the sample has a link, it should be merged
                         %with the relevant branches
                         else
                             %Get the shortest distance
                             shortest_dist = arr_branch_dist(closest_branch,1);
                             %Define the threshold
                             %TODO: Make the multiplication factor a 
                             %system parameter
                             dist_threshold = 2*shortest_dist;
                             is_merged = 1;
                             
                             arr_branch_id_cur = [];
                             arr_branch_id_cur = closest_branch;
                             
                             merge_to_single_branch
                             
                             num_attachments_cur = size(arr_branch_id_cur,2);
                             
                             %Insert the current sample into all
                             %relevant branches computed above
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
                             
                             %DO (A) AND (B) ATTACH TO DIFFERENT BRANCHES?
                             %Is there a new current branch?
                             is_cur_different = 0;
                             is_cur_non_equivalent = 0;
                             for nac=1:num_attachments_cur
                                 curID = arr_branch_id_cur(1,nac);
                                 %Check to see if a new branch got
                                 %involved, mark the current sample as
                                 %a junction if one did
                                 if(isempty(find(arr_branch_id_last==curID,1)) && ~isempty(arr_branch_id_last))
                                    is_cur_different = 1;
                                    is_cur_non_equivalent = 1;
                                    for eq=1:size(equiv_table_cur{curID,1},2)
                                        eqID = equiv_table_cur{curID,1}(1,eq);
                                        if(~isempty(find(arr_branch_id_last==eqID,1)))
                                            is_cur_non_equivalent = 0;
                                            break;
                                        end
                                    end
                                    if(is_cur_non_equivalent)
                                        break;
                                    end
                                 end
                             end

                             %Did we lose an existing last branch?
                             is_last_different = 0;
                             is_last_non_equivalent = 0;
                             for nal=1:num_attachments_last
                                 curID = arr_branch_id_last(1,nal);
                                 %Check to see if an existing branch
                                 %lost touch, mark the previous sample
                                 %as a junction if one did
                                 if(isempty(find(arr_branch_id_cur==curID,1)) && ~isempty(arr_branch_id_cur))
                                    is_last_different =1;
                                    is_last_non_equivalent = 1;
                                    for eq=1:size(equiv_table_last{curID,1},2)
                                        eqID = equiv_table_last{curID,1}(1,eq);
                                        if(~isempty(find(arr_branch_id_cur==eqID,1)))
                                            is_last_non_equivalent = 0;
                                            break;
                                        end
                                    end
                                    if(is_last_non_equivalent)
                                        break;
                                    end
                                 end
                             end
                             
                             %Bi) If we had been building up a new branch,
                             %now is the time to create and add it
                             if(currently_traversing_branch)
                                 %Break the existing branch
                                 currently_traversing_branch = 0;
                                 query_branch_size = ms-new_branch_start+1;
                                 
								 %If we don't have a starting point for
								 %the branch, then this branch attaches
								 %only at the current sample
								 if(isempty(arr_branch_id_last))
									 is_elongated = 0;
									 %CHECK FOR ELONGATION AT (B): YES
									 elongation_B_yes
                                     
									 %CHECK FOR ELONGATION AT (B): NO
									 %Otherwise this is a junction     
									 elongation_B_no
										 
								 %(A) AND (B) ATTACH TO DIFFERENT BRANCHES 
								 elseif(is_cur_different || is_last_different)
									 is_init_elongated = 0;
									 appended_curve = [];
									 appended_graph_content = [];
									 appended_lock = [];
									 init_branch_id = 0;
									 flip_before_second_append = 0;
									 init_target_min = 0;
									 init_target_max = 0;
									 %Check for elongation at (A) if
									 %there is only 1 attaching branch
									 if(num_attachments_last==1 && ~no_init_junction)
										 rb_id = arr_branch_id_last(1,1);
										 init_branch_id = rb_id;

										 target_curve = curve_graph{rb_id,1};
										 target_size = size(target_curve,1);
										 target_lock_init = lock_map(rb_id,1);
										 target_lock_final = lock_map(rb_id,2);

										 size_beginning_branch = arr_junction_indices_last(rb_id,1);
										 size_ending_branch = target_size - arr_junction_indices_last(rb_id,1) + 1;
										 
										 is_inverted = 0;
										 is_two_junctions = 0;
										 id_ind = find(arr_branch_id_cur==rb_id,1);
										 %We need to see if this branch
										 %is attached at two points
										 if(~isempty(id_ind))
											 is_two_junctions = 1;
											 if(arr_junction_indices_last(rb_id,1) > arr_junction_indices(rb_id,1))
												 is_inverted = 1;
											 end
										 end

										 %If we're close to the
										 %beginning on this branch and
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 if(size_beginning_branch<size_ending_branch && (~is_two_junctions || ~is_inverted) && ~target_lock_init && (size_beginning_branch<3 || get_length(target_curve(1:arr_junction_indices_last(rb_id,1),:)) < branch_length_threshold))
											 appended_curve = [flipud(queryCurve(new_branch_start:ms-1,:)); target_curve(arr_junction_indices_last(rb_id,1):target_size,:)];
											 %curve_graph{rb_id,1} = target_curve; 
											 is_init_elongated = 1;
											 flip_before_second_append = 1;
											 init_target_min = arr_junction_indices_last(rb_id,1);
											 init_target_max = target_size;
											 %Set the locks
											 appended_lock = lock_map(rb_id,2);
											 %TODO: Adjust the content of the merged branch
											 appended_graph_content = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
										 %If we're close to the ending and      
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 elseif(size_ending_branch<size_beginning_branch && (~is_two_junctions || is_inverted) && ~target_lock_final && (size_ending_branch<3 || get_length(target_curve(arr_junction_indices_last(rb_id,1):target_size,:)) < branch_length_threshold))
											 appended_curve = [target_curve(1:arr_junction_indices_last(rb_id,1),:); queryCurve(new_branch_start:ms-1,:)];
											 %curve_graph{rb_id,1} = target_curve; 
											 is_init_elongated = 1;
											 init_target_min = 1;
											 init_target_max = arr_junction_indices_last(rb_id,1);
											 %Set the locks
											 appended_lock = lock_map(rb_id,1);
											 %TODO: Adjust the content of the merged branch
											 appended_graph_content = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
										 end
									 end
										 
									 if(~is_init_elongated)
										 if(no_init_junction)
											 appended_curve = queryCurve(new_branch_start:ms-1,:);
											 appended_lock = 0;
										 else
											 appended_curve = queryCurve(new_branch_start-1:ms-1,:);
											 appended_lock = 1;
										 end
										 appended_graph_content = [all_views(1,mrv)+1 cc];
									 end
										
									 %Check for elongation at (B) if
									 %there is only 1 attaching branch
									 is_final_elongated = 0;
									 final_target_min = 0;
									 final_target_max = 0;
									 if(num_attachments_cur==1)
										 rb_id = arr_branch_id_cur(1,1);

										 target_curve = curve_graph{rb_id,1};
										 target_size = size(target_curve,1);
										 target_lock_init = lock_map(rb_id,1);
										 target_lock_final = lock_map(rb_id,2);

										 size_beginning_branch = arr_junction_indices(rb_id,1);
										 size_ending_branch = target_size - arr_junction_indices(rb_id,1) + 1;
										 
										 is_inverted = 0;
										 is_two_junctions = 0;
										 id_ind = find(arr_branch_id_last==rb_id,1);
										 %We need to see if this branch
										 %is attached at two points
										 if(~isempty(id_ind))
											 is_two_junctions = 1;
											 if(arr_junction_indices_last(rb_id,1) > arr_junction_indices(rb_id,1))
												 is_inverted = 1;
											 end
										 end
										 
										 %If we're close to the
										 %beginning on this branch and
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 if(size_beginning_branch<size_ending_branch && (~is_two_junctions || is_inverted) && ~target_lock_init && (size_beginning_branch<3 || get_length(target_curve(1:arr_junction_indices(rb_id,1),:)) < branch_length_threshold))
											 if(flip_before_second_append)
												 appended_curve = flipud(appended_curve);
											 end
											 target_curve = [appended_curve; target_curve(arr_junction_indices(rb_id,1):target_size,:)];
											 curve_graph{rb_id,1} = target_curve; 
											 is_final_elongated = 1;
											 final_target_min = arr_junction_indices(rb_id,1);
											 final_target_max = target_size;
											 %Set the locks
											 lock_map(rb_id,1) = appended_lock;
											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; appended_graph_content],'rows');
										 %If we're close to the ending and      
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 elseif(size_ending_branch<size_beginning_branch && (~is_two_junctions || ~is_inverted) && ~target_lock_final && (size_ending_branch<3 || get_length(target_curve(arr_junction_indices(rb_id,1):target_size,:)) < branch_length_threshold))
											 if(flip_before_second_append)
												 appended_curve = flipud(appended_curve);
											 end
											 target_curve = [target_curve(1:arr_junction_indices(rb_id,1),:); flipud(appended_curve)];
											 curve_graph{rb_id,1} = target_curve; 
											 is_final_elongated = 1;
											 final_target_min = 1;
											 final_target_max = arr_junction_indices(rb_id,1);
											 %Set the locks
											 lock_map(rb_id,2) = appended_lock;
											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; appended_graph_content],'rows');
										 end
									 end
									 
									 at_least_one_branch_broken = 0;
									 %If both sides are elongated,
									 %delete the earlier branch
									 if(is_init_elongated && is_final_elongated)
										 curve_graph{init_branch_id,1} = [];
										 curve_graph_content{init_branch_id,1} = [];
										 lock_map(init_branch_id,:) = [0 0];

									 %Break init at junctions if not
									 %elongated
									 elseif(~is_init_elongated && is_final_elongated && ~no_init_junction)

										 branch_junction_processing1
										 
									 elseif(is_init_elongated && ~is_final_elongated)
										 
										 curve_graph{init_branch_id,1} = appended_curve;
										 curve_graph_content{init_branch_id,1} = appended_graph_content;

										 branch_junction_processing2

									 elseif(~is_init_elongated && ~is_final_elongated && (query_branch_size > branch_size_threshold || get_length(queryCurve(new_branch_start:ms,:)) > branch_length_threshold))
									 
										 if(no_init_junction)
											 new_branch = queryCurve(new_branch_start:ms,:);
											 all_new_branches = [all_new_branches; cell(1,1)];
											 anb_size = size(all_new_branches,1);
											 all_new_branches{anb_size,1} = new_branch;
											 all_new_locks = [all_new_locks; [0 1]];
										 else
											 new_branch = queryCurve(new_branch_start-1:ms,:);
											 all_new_branches = [all_new_branches; cell(1,1)];
											 anb_size = size(all_new_branches,1);
											 all_new_branches{anb_size,1} = new_branch; 
											 all_new_locks = [all_new_locks; [1 1]];
											 
											 %Add the junction if it's not added already
											 if(~is_last_junction_saved)
												all_junctions = [all_junctions; queryCurve(new_branch_start-1,:)];
												is_last_junction_saved = 1;
											 end
										 end

										 %Add the junction if it's not added already
										 if(~is_junction_saved)
											all_junctions = [all_junctions; queryCurve(ms,:)];
											is_junction_saved = 1;
										 end

                                         %Go over each branch to see if it
										 %needs to be broken at up to 2
										 %points
                                         branch_at_points

									 end
								 else       
									 
									 is_elongated = 0;
									 
									 elong_curve = [];
									 other_curve1 = [];
									 other_curve2 = [];
									 
									 elong_lock = [];
									 other_lock1 = [];
									 other_lock2 = [];
																		 
									 %CHECK FOR ELONGATION AT (A): YES
									 if(num_attachments_last==1 && ~no_init_junction)
										 
																					 
										 %Retrieve target branches and
										 %curves as if the new branch
										 %only attach
										 rb_id = arr_branch_id_last(1,1);

										 target_curve = curve_graph{rb_id,1};
										 target_size = size(target_curve,1);
										 target_lock_init = lock_map(rb_id,1);
										 target_lock_final = lock_map(rb_id,2);

										 size_beginning_branch = arr_junction_indices_last(rb_id,1);
										 size_ending_branch = target_size - arr_junction_indices_last(rb_id,1) + 1;
										 
										 is_inverted = 0;
										 %We first need to break the
										 %relevant branch at the next
										 %junction
										 if(arr_junction_indices_last(rb_id,1) < arr_junction_indices(rb_id,1))
											 elong_curve = target_curve(arr_junction_indices_last(rb_id,1):arr_junction_indices(rb_id,1),:);
											 other_curve1 = target_curve(1:arr_junction_indices_last(rb_id,1),:);
											 other_curve2 = target_curve(arr_junction_indices(rb_id,1):target_size,:);
										 else
											 elong_curve = target_curve(arr_junction_indices(rb_id,1):arr_junction_indices_last(rb_id,1),:);
											 other_curve1 = target_curve(1:arr_junction_indices(rb_id,1),:);
											 other_curve2 = target_curve(arr_junction_indices_last(rb_id,1):target_size,:);
											 is_inverted = 1;
										 end
									 
										 %If we're close to the
										 %beginning on this branch and
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 if(size_beginning_branch<size_ending_branch && ~is_inverted && ~target_lock_init && (size_beginning_branch<3  || get_length(other_curve1) < branch_length_threshold))
											 target_curve = [flipud(queryCurve(new_branch_start:ms,:)); elong_curve];
											 curve_graph{rb_id,1} = target_curve; 
											 is_elongated = 1;
											 %Set the locks
											 lock_map(rb_id,:) = [1 1];
											 %Add the junction if it's not added already
											 if(~is_junction_saved)
												all_junctions = [all_junctions; queryCurve(ms,:)];
												is_junction_saved = 1;
											 end
											 
											 curve_graph = [curve_graph; cell(1,1)];
											 cg_size = size(curve_graph,1);
											 curve_graph{cg_size,1} = other_curve2;
											 %Set the locks
											 lock_map = [lock_map; [1 target_lock_final]];
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
											 
											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
											 
										 %If we're close to the ending and      
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 elseif(size_ending_branch<size_beginning_branch && is_inverted && ~target_lock_final && (size_ending_branch<3 || get_length(other_curve2) < branch_length_threshold))
											 target_curve = [elong_curve; queryCurve(new_branch_start:ms,:)];
											 curve_graph{rb_id,1} = target_curve; 
											 is_elongated = 1;
											 %Set the locks
											 lock_map(rb_id,:) = [1 1];
											 %Add the junction if it's not added already
											 if(~is_junction_saved)
												all_junctions = [all_junctions; queryCurve(ms,:)];
												is_junction_saved = 1;
											 end
											 
											 curve_graph = [curve_graph; cell(1,1)];
											 cg_size = size(curve_graph,1);
											 curve_graph{cg_size,1} = other_curve1;
											 %Set the locks
											 lock_map = [lock_map; [target_lock_init 1]];
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
											 
											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');
											 
										 end
										 
										 if(is_elongated)
											 %Break all the extra branches
											 %(B) connects to
											 for rb=1:num_attachments_cur

												 rb_id = arr_branch_id_cur(1,rb);

												 if(rb_id==arr_branch_id_last(1,1))
													 continue;
												 end
												 disp('WARNING: Branches should be identical, something is wrong!');

												 target_curve = curve_graph{rb_id,1};
												 target_size = size(target_curve,1);

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
										 end
									 end
									 
									 %CHECK FOR ELONGATION AT (A): NO
									 %CHECK FOR ELONGATION AT (B): YES
									 if(num_attachments_cur==1 && (~is_elongated))
										 
										 %Retrieve target branches and
										 %curves as if the new branch
										 %only attach
										 rb_id = arr_branch_id_cur(1,1);
										 
										 
										 
										 target_curve = curve_graph{rb_id,1};
										 target_size = size(target_curve,1);
										 target_lock_init = lock_map(rb_id,1);
										 target_lock_final = lock_map(rb_id,2);
										 
										 size_beginning_branch = arr_junction_indices(rb_id,1);
										 size_ending_branch = target_size - arr_junction_indices(rb_id,1) + 1;
										 
										 is_inverted = 0;
										 %We first need to break the
										 %relevant branch at the next
										 %junction
										 if(arr_junction_indices_last(rb_id,1) < arr_junction_indices(rb_id,1))
											 if(no_init_junction)
												 elong_curve = target_curve(1:arr_junction_indices(rb_id,1),:);
												 other_curve2 = target_curve(arr_junction_indices(rb_id,1):target_size,:);
											 else
												 elong_curve = target_curve(arr_junction_indices_last(rb_id,1):arr_junction_indices(rb_id,1),:);
												 other_curve1 = target_curve(1:arr_junction_indices_last(rb_id,1),:);
												 other_curve2 = target_curve(arr_junction_indices(rb_id,1):target_size,:);
											 end
										 else
											 if(no_init_junction)
												 elong_curve = target_curve(arr_junction_indices(rb_id,1):target_size,:);
												 other_curve1 = target_curve(1:arr_junction_indices(rb_id,1),:);
											 else
												 elong_curve = target_curve(arr_junction_indices(rb_id,1):arr_junction_indices_last(rb_id,1),:);
												 other_curve1 = target_curve(1:arr_junction_indices(rb_id,1),:);
												 other_curve2 = target_curve(arr_junction_indices_last(rb_id,1):target_size,:);
											 end
											 is_inverted = 1;
										 end

										 %If we're close to the
										 %beginning on this branch and
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 if(size_beginning_branch<size_ending_branch && is_inverted && ~target_lock_init && (size_beginning_branch<3 || get_length(other_curve1) < branch_length_threshold))
											 if(no_init_junction)
												 target_curve = [queryCurve(new_branch_start:ms-1,:); elong_curve];
												 %Set the locks
												 lock_map(rb_id,:) = [0 target_lock_final];
											 else
												 target_curve = [queryCurve(new_branch_start-1:ms-1,:); elong_curve];
												 %Set the locks
												 lock_map(rb_id,:) = [1 1];
											 end
											 curve_graph{rb_id,1} = target_curve;
											 is_elongated = 1;
											 
											 %Add the junction if it's not added already
											 if(~is_last_junction_saved && ~no_init_junction)
												all_junctions = [all_junctions; queryCurve(new_branch_start-1,:)];
												is_last_junction_saved = 1;
											 end

											 if(~isempty(other_curve2))
												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = other_curve2;
												 %Set the locks
												 lock_map = [lock_map; [1 target_lock_final]];
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

											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');

										 %If we're close to the ending and      
										 %If our native branch is not long 
										 %enough, and the new branch attaches to 
										 %only 1 existing branch, then elongate
										 elseif(size_ending_branch<size_beginning_branch && ~is_inverted && ~target_lock_final && (size_ending_branch<3 || get_length(other_curve2) < branch_length_threshold))
											 if(no_init_junction)
												 target_curve = [elong_curve; flipud(queryCurve(new_branch_start:ms-1,:))];
												 %Set the locks
												 lock_map(rb_id,:) = [target_lock_init 0];
											 else
												 target_curve = [elong_curve; flipud(queryCurve(new_branch_start-1:ms-1,:))];
												 %Set the locks
												 lock_map(rb_id,:) = [1 1];
											 end
											 curve_graph{rb_id,1} = target_curve; 
											 is_elongated = 1;
											 %Add the junction if it's not added already
											 if(~is_last_junction_saved && ~no_init_junction)
												 all_junctions = [all_junctions; queryCurve(new_branch_start-1,:)];
												 is_last_junction_saved = 1;
											 end
											 
											 

											 if(~isempty(other_curve1))
												 curve_graph = [curve_graph; cell(1,1)];
												 cg_size = size(curve_graph,1);
												 curve_graph{cg_size,1} = other_curve1;
												 %Set the locks
												 lock_map = [lock_map; [target_lock_init 1]];
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

											 %TODO: Adjust the content of the merged branch
											 curve_graph_content{rb_id,1} = unique([curve_graph_content{rb_id,1}; [all_views(1,mrv)+1 cc]],'rows');

										 end
										 
										 if(is_elongated && ~no_init_junction)
											 %Break all the extra branches
											 %(A) connects to
											 for rb=1:num_attachments_last

												 rb_id = arr_branch_id_last(1,rb);

												 if(rb_id==arr_branch_id_cur(1,1))
													 continue;
												 end
												 disp('WARNING: Branches should be identical, something is wrong!');

												 target_curve = curve_graph{rb_id,1};
												 target_size = size(target_curve,1);

												 size_beginning_branch = arr_junction_indices_last(rb_id,1);
												 size_ending_branch = target_size - arr_junction_indices_last(rb_id,1) + 1;

												 %Junction breaks the existing branch it's
												 %being attached to
												 target_curve1 = target_curve(1:arr_junction_indices_last(rb_id,1),:);
												 target_curve2 = target_curve(arr_junction_indices_last(rb_id,1):target_size,:);

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
										 end
									 end
									 
									 %CHECK FOR ELONGATION AT (B): NO
									 if(~is_elongated && (query_branch_size > branch_size_threshold || get_length(queryCurve(new_branch_start:ms,:)) > branch_length_threshold))
										 if(no_init_junction)
											 %Create and add the new branch
											 new_branch = queryCurve(new_branch_start:ms,:);
											 all_new_branches = [all_new_branches; cell(1,1)];
											 anb_size = size(all_new_branches,1);
											 all_new_branches{anb_size,1} = new_branch;
											 all_new_locks = [all_new_locks; [0 1]];
										 else
											 %Create and add the new branch
											 new_branch = queryCurve(new_branch_start-1:ms,:);
											 all_new_branches = [all_new_branches; cell(1,1)];
											 anb_size = size(all_new_branches,1);
											 all_new_branches{anb_size,1} = new_branch;
											 all_new_locks = [all_new_locks; [1 1]];

											 %Add the junction if it's not added already
											 if(~is_last_junction_saved)
												all_junctions = [all_junctions; queryCurve(new_branch_start-1,:)];
												is_last_junction_saved = 1;
											 end
										 end

										 %Add the junction if it's not added already
										 if(~is_junction_saved)
											all_junctions = [all_junctions; queryCurve(ms,:)];
											is_junction_saved = 1;
										 end

										 %Go over each branch to see if it
										 %needs to be broken at up to 2
										 %points
										 branch_at_points

									 end
								 end
                                 
                             else 
                                 
                                 %If this sample has been merged to a branch that the previous
                                 %sample did not merge to, we need to mark this
                                 %as a junction
                                 
%                                  if(is_cur_non_equivalent && (num_attachments_cur>1 || num_attachments_last>1))
%                                      extra_junctions = [extra_junctions; queryCurve(ms,:)];
%                                  end
%                                  
%                                  if(is_last_non_equivalent && (num_attachments_cur>1 || num_attachments_last>1))
%                                      extra_junctions = unique([extra_junctions; queryCurve(ms-1,:)],'rows');
%                                  end
                                 
                             end
                             
                            %Some branches might've changed in the process
                            %of merging the sample, so we need to recompute
                            %the junction indices so that future curves are
                            %appended at the right location
                            for jn=1:size(arr_junction_indices,1)
                                if(arr_junction_indices(jn,1)<=0)
                                    continue;
                                end
                                inspect_curve = curve_graph{jn,1};
                                junc_index = find(inspect_curve(:,1)==queryCurve(ms,1) & inspect_curve(:,2)==queryCurve(ms,2) & inspect_curve(:,3)==queryCurve(ms,3));
                                
                                if(isempty(junc_index))
                                    arr_junction_indices(jn,1)=0;
                                    arr_prev_sample(jn,1)=0;
                                    arr_next_sample(jn,1)=0;
                                    arr_branch_id_cur(arr_branch_id_cur==jn)=[];
                                else
                                    junction_different = 1;
                                    for jin=1:size(junc_index,2)
                                        if(junc_index(1,jin)==arr_junction_indices(jn,1))
                                            junction_different = 0;
                                            break;
                                        end
                                    end
                                    if(junction_different)
                                        %TODO: How do we pick among many?
                                        %Currently we pick the first one
                                        arr_junction_indices(jn,1) = junc_index(1,1);
                                    end
                                end
                            end
                             
                            %Previous merge info is stored here 
                            arr_branch_id_last = arr_branch_id_cur;
                            arr_junction_indices_last = arr_junction_indices;
                            arr_prev_sample_last = arr_prev_sample;
                            arr_next_sample_last = arr_next_sample;
                            equiv_table_last = equiv_table_cur;
                            
                         end
                    end
                    merged
                end

            end

            merging_iterations_done = 1;
            % Check whether all curves have been merged yet
            for av=1:numViews
                cur_merge_flags = merge_flags{av,1};
                if(any(~cur_merge_flags))
                    merging_iterations_done = 0;
                    break;
                end
            end
            if((~merging_iterations_done) && (~new_curve_merged))
                 merging_iterations_done = 1;

                 for av=1:numViews
                    cur_merge_flags = merge_flags{av,1};
                    for cmf=1:size(cur_merge_flags,2)
%                         if(~cur_merge_flags(1,cmf))
%                              curve_graph = [curve_graph; cell(1,1)];
%                              cg_size = size(curve_graph,1);
%                              cur_id = clusters{all_views(1,av)+1,1}(1,cmf);
%                              curve_graph{cg_size,1} = all_recs_iter{all_views(1,av)+1,1}{1,cur_id};
%                         end
                    end
                 end
            end
        end
        
%         %Post-process the curve graph to break incorrect groupings and
%         %prune short curves
%         for br=1:size(curve_graph,1)
%             cur_branch = curve_graph{br,1};
%             num_branch_samples = size(cur_branch,1);
%             
%             if(num_branch_samples<2)
%                 continue;
%             end
%             
%             startSample = 1;
%             first_branch = 1;
% 
%             for bs=2:num_branch_samples
%                 dist = norm(cur_branch(bs-1,:) - cur_branch(bs,:));
%                 if(dist > 0.05)
%                     if(first_branch)
%                         first_branch = 0;
%                         curve_graph{br,1} = cur_branch(startSample:(bs-1),:);
%                     else
%                         curve_graph = [curve_graph; cell(1,1)];
%                         cg_size = size(curve_graph,1);
%                         curve_graph{cg_size,1} = cur_branch(startSample:(bs-1),:);
%                     end
%                     startSample = bs;
%                 end
%             end
%             if(~first_branch && startSample<num_branch_samples)
%                 curve_graph = [curve_graph; cell(1,1)];
%                 cg_size = size(curve_graph,1);
%                 curve_graph{cg_size,1} = cur_branch(startSample:num_branch_samples,:);
%             end
%         end

        counter = size(all_clusters,1)+1;        
%         for vv=1:numViews
%             for cl=1:size(clusters{all_views(1,vv)+1,1},2)
%                 curveID = clusters{all_views(1,vv)+1,1}(1,cl);
%                 curve = all_recs{all_views(1,vv)+1,1}{1,curveID};
%                 curve_iter = all_recs_iter{all_views(1,vv)+1,1}{1,curveID};
%                 write_curve_to_vrml(curve,[255 0 0],['./ply/before-nocorr/view',num2str(all_views(1,vv)),'_curve',num2str(curveID),'_cluster',num2str(counter),'.ply']);
%                 write_curve_to_vrml(curve_iter,[255 0 0],['./ply/after/view',num2str(all_views(1,vv)),'_curve',num2str(curveID),'_cluster',num2str(counter),'.ply']);
%                 %write_curve_and_correspondence_to_vrml(curve,[255 0 0],['./ply/before/view',num2str(all_views(1,vv)),'_curve',num2str(curveID),'_cluster',num2str(counter),'.ply'],corr_native{all_views(1,vv)+1,1}{1,cl},corr_other{all_views(1,vv)+1,1}{1,cl},255.*corr_colors{all_views(1,vv)+1,1}{1,cl});
%                 %counter = counter + 1;
%             end
%         end
        all_clusters = [all_clusters; cell(1,1)];
        all_clusters{size(all_clusters,1),1} = clusters;
        complete_curve_graph = [complete_curve_graph; curve_graph];
        complete_lock_map = [complete_lock_map; lock_map];
        %save('curve_graph_amsterdam_half.mat','complete_curve_graph','all_junctions');
        %save('curve_graph_amsterdam.mat');
    end
end

% for v=1:numIM
%     if(isempty(all_flags{v,1}))
%        continue;
%     end
%     num_curves = size(all_flags{v,1},1);
%     for c=1:num_curves
%       if(all_flags{v,1}(c,1)<num_shared_edges)
%         complete_curve_graph = [complete_curve_graph; cell(1,1)];
%         cg_size = size(complete_curve_graph,1);
%         complete_curve_graph{cg_size,1} = all_recs{all_views(1,av)+1,1}{1,c};
%       end
%     end
% end

all_junctions = [all_junctions; extra_junctions];

save_curve_drawing;
%visualize_curve_drawing;
