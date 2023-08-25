clear all;
close all;

numIM = 27;
colors = distinguishable_colors(numIM);

all_recs = cell(numIM,1);
all_nR = zeros(numIM,1);
all_links_3d = cell(numIM,1);
all_offsets_3d = cell(numIM,1);
all_edge_support_3d = cell(numIM,1);
all_flags = cell(numIM,1);

all_num_im_contours = zeros(numIM,1);

read_curve_sketch4;
all_recs{11,1} = recs;
all_nR(11,1) = size(recs,2);
all_flags{11,1} = zeros(size(recs,2),1);
clear recs;
read_curve_sketch4a;
all_recs{14,1} = recs;
all_nR(14,1) = size(recs,2);
all_flags{14,1} = zeros(size(recs,2),1);
clear recs;
read_curve_sketch4b;
all_recs{7,1} = recs;
all_nR(7,1) = size(recs,2);
all_flags{7,1} = zeros(size(recs,2),1);
clear recs;
read_curve_sketch4c;
all_recs{8,1} = recs;
all_nR(8,1) = size(recs,2);
all_flags{8,1} = zeros(size(recs,2),1);
clear recs;
read_curve_sketch4d;
all_recs{9,1} = recs;
all_nR(9,1) = size(recs,2);
all_flags{9,1} = zeros(size(recs,2),1);
clear recs;

load edge-curve-index_yuliang.mat;
load edge-curve-offset_yuliang.mat;

%All the views that will be used in the clustering.
%The first view is where the initial seed/query curve is located.
all_views = [10 13 6 7 8];
numViews = size(all_views,2);
num_shared_edges = 10;

disp('READING DATA');
for v=1:numViews
%for v=1:1
    all_views(1,v)
    cons = read_cons(['./curves_yuliang/',num2str(all_views(1,v),'%08d'),'.cemv'],[num2str(all_views(1,v),'%08d'),'.jpg'],0,-1);
    num_im_contours = size(cons,2);
    [all_links_3d{all_views(1,v)+1,1}, all_offsets_3d{all_views(1,v)+1,1}, all_edge_support_3d{all_views(1,v)+1,1}] = read_association_attributes(all_views(1,v),num_im_contours,all_nR(all_views(1,v)+1,1),numIM);
    all_num_im_contours(all_views(1,v)+1,1) = num_im_contours;
end

all_clusters = cell(0,0);

for crv = 1:all_nR(11,1)
    crv
    if(all_flags{11,1}(crv,1)<num_shared_edges)
        
        clusters = cell(numIM,1);
        process_flags = cell(numIM,1);
        %votes = cell(numIM,1);

        %The ID of the initial seed/query curve where clustering will begin
        clusters{11,1} = crv;
        all_flags{11,1}(crv,1) = num_shared_edges;
        %votes{11,1} = 3;
        process_flags{11,1} = 0;

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
                other_views;

                edge_support_3d = all_edge_support_3d{first_anchor+1,1};
                cur_cluster = clusters{first_anchor+1,1};
                cur_flags = process_flags{first_anchor+1,1};
                c_size = size(cur_cluster,2);

                for cc=1:c_size

                    queryID = cur_cluster(1,cc);            
                    if(~cur_flags(1,cc))

                        queryCurve = all_recs{first_anchor+1,1}{1,queryID}; 
                        querySupport = edge_support_3d{queryID,1};
                        numSamples = size(queryCurve,1);

                        cur_flags(1,cc) = 1;
                        process_flags{first_anchor+1,1} = cur_flags;

                        %disp('COMPUTING ASSOCIATIONS');
                        allEdges = [];
                        for v=1:size(other_views,2);
                            [first_anchor other_views(1,v)];
                            other_cluster = clusters{other_views(1,v)+1,1};
                            other_flags = process_flags{other_views(1,v)+1,1};
                            %other_votes = votes{other_views(1,v)+1,1};
                            edge_support = querySupport{other_views(1,v)+1,1};
                            cur_links_3d = all_links_3d{other_views(1,v)+1,1};
                            cur_offsets_3d = all_offsets_3d{other_views(1,v)+1,1};
                            [edg edgmap] = load_edg(['./edges/',num2str(other_views(1,v),'%08d'),'.edg']);

                            fid = fopen(['./calibration/',num2str(other_views(1,v),'%08d'),'.projmatrix']);
                            curP = (fscanf(fid,'%f',[4 3]))';

                            for s=1:numSamples
                            %for s=6:13    
                                if(~isempty(edge_support))
                                    edges = edge_support{s,1};

                                    for e=1:size(edges,2)

                                        edge = edges(1,e);
                                        if(edge < size(curveIndices{other_views(1,v)+1,1},1))

                                            cur_sample = [queryCurve(s,:)';1];
                                            imSample = curP*cur_sample;
                                            imSample = imSample./imSample(3,1);
                                            im_pixel = imSample(1:2,1)+1;
                                            cur_edge = edg(edge+1,1:2)';
                                            edge_dist = norm(cur_edge - im_pixel);

                                            curve = curveIndices{other_views(1,v)+1,1}(edge+1,1);
                                            ds = curveOffsets{other_views(1,v)+1,1}(edge+1,1);
                                            if(ds>-1 && edge_dist <= 1)
                                                if(~isempty(cur_links_3d{curve,1}))                                     
                                                    curves_3d = cur_links_3d{curve,1};
                                                    offsets_3d = cur_offsets_3d{curve,1};

                                                    [sorted_offsets, sorting] = sort(offsets_3d);
                                                    sorted_curves = curves_3d(sorting);

                                                    num_sorted_curves = size(sorted_offsets,2);
                                                    sorted_lengths = zeros(1,num_sorted_curves);

                                                    for sc=1:num_sorted_curves
                                                        sorted_lengths(1,sc) = size(all_recs{other_views(1,v)+1,1}{1,sorted_curves(1,sc)+1},1); 
                                                    end

                                                    %Start with no association
                                                    curve_index_to_link = 0;

                                                    for sc=1:num_sorted_curves
                                                       if(ds>=sorted_offsets(1,sc) && ds<sorted_offsets(1,sc)+sorted_lengths(1,sc))
                                                           curve_index_to_link = sc;
                                                           break;
                                                       end
                                                    end

                                                    if(curve_index_to_link==0)
                                                        continue;
                                                    end
                                                    curve_to_link = sorted_curves(1,curve_index_to_link)+1;
                                                    
                                                    all_flags{other_views(1,v)+1,1}(curve_to_link,1) = all_flags{other_views(1,v)+1,1}(curve_to_link,1)+1;
                                                    
                                                    if(all_flags{other_views(1,v)+1,1}(curve_to_link,1) == num_shared_edges)
                                                        other_cluster = unique([other_cluster curve_to_link]);
                                                        %If a new curve ID was added, add
                                                        %an appropriate flag
                                                        fl_size  = size(other_flags,2);
                                                        if(size(other_cluster,2)~=fl_size)
                                                            loc=find(other_cluster==curve_to_link);
                                                            if(loc==1)
                                                                other_flags = [0 other_flags]; 
                                                                %other_votes = [1 other_votes];
                                                            elseif(loc==size(other_cluster,2))
                                                                other_flags = [other_flags 0]; 
                                                                %other_votes = [other_votes 1];
                                                            else
                                                                other_flags = [other_flags(1,1:(loc-1)) 0 other_flags(1,loc:fl_size)]; 
                                                                %other_votes = [other_votes(1,1:(loc-1)) 1 other_votes(1,loc:fl_size)];
                                                            end
                                                            clusters{other_views(1,v)+1,1} = other_cluster;
                                                            process_flags{other_views(1,v)+1,1} = other_flags;
                                                            %votes{other_views(1,v)+1,1} = other_votes;
                                                        end
                                                    end
                                                end
                                            end
                                        else
                                            disp('WARNING: Weird edge ID!!!');
                                        end
                                    end
                                end
                            end
                        end
                    end
                end

            end  

            keep_processing = 0;
            %Check whether you need to keep processing
            for av=1:numViews
                cur_flags = process_flags{all_views(1,av)+1,1};
                if(any(~cur_flags))
                    keep_processing = 1;
                    break;
                end
            end
            
            
            
        end
        
        all_clusters = [all_clusters; cell(1,1)];
        all_clusters{size(all_clusters,1),1} = clusters;
        %all_votes = [all_votes; cell(1,1)];
        %all_votes{size(all_votes,1),1} = votes;
    end
end

