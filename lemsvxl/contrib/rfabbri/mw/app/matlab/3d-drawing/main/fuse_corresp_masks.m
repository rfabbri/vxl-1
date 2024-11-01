%Samples that have no correspondence on any other curve in the
                %cluster, are already converged
                fused_corresp_masks = zeros(numSamples,1);
                for vcl=1:numIM
                    cur_view = corresp_masks{vcl,1};
                    if(isempty(cur_view))
                        continue;
                    end
                    for cl=1:size(cur_view,2)
                        cur_mask = cur_view(:,cl);
                        fused_corresp_masks = fused_corresp_masks | cur_mask;
                    end
                end
                converge_flags{all_views(1,av)+1,1}{1,cc} = ~fused_corresp_masks;