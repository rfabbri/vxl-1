new_clusters = previous_clusters;
new_process_flags = previous_flags;

new_corr_native = previous_corr_native;
new_corr_other = previous_corr_other;
new_corr_colors = previous_corr_colors;

%We process the reliable correspondences to do clustering &
%averaging
for ss=1:size(queryAlignment,1)
  cur_alignment = queryAlignment{ss,1};
  for a=1:size(cur_alignment,1)
    cur_corr = cur_alignment(a,:);

    corr_view = cur_corr(1,1);
    corr_curve = cur_corr(1,2);
    corr_sample = cur_corr(1,3);
    corr_votes = cur_corr(1,4);

    if(all_flags{corr_view,1}(corr_curve,1)==num_shared_edges)
      continue;
    end
    %This correspondence votes on the respective curve
    all_flags{corr_view,1}(corr_curve,1) = all_flags{corr_view,1}(corr_curve,1)+1;

    %If we have received the minimum amount of votes
    %required, add the curve to the cluster
    if(all_flags{corr_view,1}(corr_curve,1)<num_shared_edges)
      continue;
    end

    new_clusters{corr_view,1} = unique([new_clusters{corr_view,1} corr_curve]);
    cur_cluster_size = size(new_clusters{corr_view,1},2);
    cur_flag_size = size(new_process_flags{corr_view,1},2);
    if(cur_cluster_size > cur_flag_size)

      %if(~isempty(all_flags{corr_view,1}))
        %all_flags{corr_view,1}(corr_curve,1) = num_shared_edges;
      %end

      cluster_index = find(new_clusters{corr_view,1}==corr_curve);
      if(size(cluster_index,1)>1)
        disp('ERROR: There is duplication in the cluster IDs!!');
      end
      %flag_index = find(clusters{corr_view,1}==corr_curve);
      %if(size(flag_index,1)>1)
        %disp('ERROR: There is duplication in the cluster IDs!!');
      %end

      if(cluster_index==1)
        %new_process_flags{corr_view,1} = [process_flags{corr_view,1}(1,flag_index) new_process_flags{corr_view,1}];
        new_process_flags{corr_view,1} = [0 new_process_flags{corr_view,1}];
        new_corr_native{corr_view,1} = [cell(1,1) new_corr_native{corr_view,1}];
        new_corr_other{corr_view,1} = [cell(1,1) new_corr_other{corr_view,1}];
        new_corr_colors{corr_view,1} = [cell(1,1) new_corr_colors{corr_view,1}];
        alignment_curves{corr_view,1} = [cell(1,1) alignment_curves{corr_view,1}];
      elseif(cluster_index>cur_flag_size)
        new_process_flags{corr_view,1} = [new_process_flags{corr_view,1} 0];
        new_corr_native{corr_view,1} = [new_corr_native{corr_view,1} cell(1,1)];
        new_corr_other{corr_view,1} = [new_corr_other{corr_view,1} cell(1,1)];
        new_corr_colors{corr_view,1} = [new_corr_colors{corr_view,1} cell(1,1)];
        alignment_curves{corr_view,1} = [alignment_curves{corr_view,1} cell(1,1)];
      else
        new_process_flags{corr_view,1} = [new_process_flags{corr_view,1}(1,1:cluster_index-1) 0 new_process_flags{corr_view,1}(1,cluster_index:cur_flag_size)];
        new_corr_native{corr_view,1} = [new_corr_native{corr_view,1}(1,1:cluster_index-1) cell(1,1) new_corr_native{corr_view,1}(1,cluster_index:cur_flag_size)];
        new_corr_other{corr_view,1} = [new_corr_other{corr_view,1}(1,1:cluster_index-1) cell(1,1) new_corr_other{corr_view,1}(1,cluster_index:cur_flag_size)];
        new_corr_colors{corr_view,1} = [new_corr_colors{corr_view,1}(1,1:cluster_index-1) cell(1,1) new_corr_colors{corr_view,1}(1,cluster_index:cur_flag_size)];
        alignment_curves{corr_view,1} = [alignment_curves{corr_view,1}(1,1:cluster_index-1) cell(1,1) alignment_curves{corr_view,1}(1,cluster_index:cur_flag_size)];
      end
%new_process_flags{corr_view,1} = [new_process_flags{corr_view,1}(1,1:cluster_index-1) process_flags{corr_view,1}(1,flag_index) new_process_flags{cur_link(1,1),1}(1,cluster_index:cur_flag_size)];
    end
  end
end
