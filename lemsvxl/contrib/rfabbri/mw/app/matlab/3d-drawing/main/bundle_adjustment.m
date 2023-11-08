% Curve based bundle adjustment is an iterative averaging 
% process that operates on curve samples marked to have 
% correspondences on the other curves.

av_cntr = 0;

        while(~averaging_iterations_done)
            av_cntr = av_cntr+1

            for av=1:numViews
                av
                cur_clusters = clusters{all_views(1,av)+1,1};

                if(isempty(cur_clusters))
                    continue;
                end

                num_curves_in_cluster = size(cur_clusters,2);

                for cc=1:num_curves_in_cluster

                    %Check to see if this curve has converged at all samples
                    cur_converge_flags = converge_flags{all_views(1,av)+1,1}{1,cc};

                    if(all(cur_converge_flags))
                        continue;
                    end

                    curveID = cur_clusters(1,cc);
                    queryCurve = all_recs_iter{all_views(1,av)+1,1}{1,curveID};
                    queryAlignment = alignment_curves{all_views(1,av)+1,1}{1,cc};
                    numSamples = size(queryCurve,1); 

                    corresp_masks = all_corresp_masks{all_views(1,av)+1,1}{1,cc};

                    %Process the masks to iterate curve samples
                    queryCurve_next = queryCurve;%zeros(numSamples,3);

                    for ss=1:numSamples

                        if(cur_converge_flags(ss,1))
                            continue;
                        end

                        querySample = queryCurve(ss,:);
                        sampleSum = querySample;
                        sampleCount = 1;

                        for vcl=1:numIM

                            cur_mask = corresp_masks{vcl,1};

                            if(isempty(cur_mask))
                                continue;
                            end

                            for cl=1:size(cur_mask,2)

                                if(~cur_mask(ss,cl))
                                    continue;
                                end                    

                                curve = all_recs_iter{vcl,1}{1,clusters{vcl,1}(1,cl)};

                                if(size(curve,1)<2)
                                    continue;
                                end

                                [closest_pt,outside_init_range,outside_final_range,prev_pt,next_pt] = find_closest_point_on_curve(curve,querySample);

                                sampleSum = sampleSum + closest_pt;
                                sampleCount = sampleCount + 1;

                            end

                        end
                        newSample = sampleSum./sampleCount;
                        queryCurve_next(ss,:) = newSample;

                        cur_motion = norm(querySample - newSample);

                        if(cur_motion <= 0.001)
                            converge_flags{all_views(1,av)+1,1}{1,cc}(ss,1) = 1;
                        end

                    end


                    all_recs_iter{all_views(1,av)+1,1}{1,curveID} = queryCurve_next;
                    %write_curve_to_vrml(queryCurve_next,[255 0 0],['./ply/after/view',num2str(all_views(1,av)),'_curve',num2str(curveID),'.ply']);

                end

            end


            averaging_iterations_done = 1;

            % Check whether all curve samples converged
            for av=1:numViews
                cur_converge_flags = converge_flags{all_views(1,av)+1,1};
                num_cluster_curves = size(cur_converge_flags,2);
                for ccv=1:num_cluster_curves
                    cur_flags = cur_converge_flags{1,ccv};
                    if(any(~cur_flags))
                        averaging_iterations_done = 0;
                        break;
                    end
                end
            end

        end
