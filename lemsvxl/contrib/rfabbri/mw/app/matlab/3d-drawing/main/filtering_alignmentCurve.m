just filter the alignment curve to only keep the links that
have enough support.

 %Here we filter correspondences to throw out unreliable
                    %ones
                    for ss=1:size(queryAlignment,1)
                       cur_alignment = queryAlignment{ss,1};
                       filtered_alignment = [];
                       filtered_alignment_dense = [];
                       for a=1:size(cur_alignment,1)

                           if(cur_alignment(a,4)>2)
                               filtered_alignment = [filtered_alignment; cur_alignment(a,:)];
                           end

                           if(cur_alignment(a,4)>1)
                               filtered_alignment_dense = [filtered_alignment_dense; cur_alignment(a,:)];
                           end

                       end
                       queryAlignment{ss,1} = filtered_alignment;
                       queryAlignment_dense{ss,1} = filtered_alignment_dense;
                    end

                    new_clusters = previous_clusters;
                    new_process_flags = previous_flags;

                    new_corr_native = previous_corr_native;
                    new_corr_other = previous_corr_other;
                    new_corr_colors = previous_corr_colors;


