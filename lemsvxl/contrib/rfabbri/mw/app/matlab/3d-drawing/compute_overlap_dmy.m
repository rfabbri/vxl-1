clear all;
close all;

numIM = 27;
colors = distinguishable_colors(numIM);

all_recs = cell(numIM,1);
all_nR = zeros(numIM,1);
all_links_3d = cell(numIM,1);
all_offsets_3d = cell(numIM,1);
all_edge_support_3d = cell(numIM,1);

all_num_im_contours = zeros(numIM,1);

read_curve_sketch4;
all_recs{11,1} = recs;
all_nR(11,1) = size(recs,2);
clear recs;
read_curve_sketch4a;
all_recs{14,1} = recs;
all_nR(14,1) = size(recs,2);
clear recs;
read_curve_sketch4b;
all_recs{7,1} = recs;
all_nR(7,1) = size(recs,2);
clear recs;
read_curve_sketch4c;
all_recs{8,1} = recs;
all_nR(8,1) = size(recs,2);
clear recs;
read_curve_sketch4d;
all_recs{9,1} = recs;
all_nR(9,1) = size(recs,2);
clear recs;

load edge-curve-index_yuliang.mat;
load edge-curve-offset_yuliang.mat;
load clusters-close-edges.mat;

%All the views that will be used in the clustering.
%The first view is where the initial seed/query curve is located.
all_views = [10 7];
numViews = size(all_views,2);

% recs = all_recs{11,1};
% plot_all_recs;
% hold on;
% recs = all_recs{8,1};
% plot_all_recs_overlay;
% hold on;

%DETERMINE OVERLAP USING SAMPLE ASSOCIATIONS

disp('READING DATA');
for v=1:numViews
%for v=1:1
    all_views(1,v)
    cons = read_cons(['./curves_yuliang/',num2str(all_views(1,v),'%08d'),'.cemv'],[num2str(all_views(1,v),'%08d'),'.jpg'],0,-1);
    num_im_contours = size(cons,2);
    [all_links_3d{all_views(1,v)+1,1}, all_offsets_3d{all_views(1,v)+1,1}, all_edge_support_3d{all_views(1,v)+1,1}] = read_association_attributes(all_views(1,v),num_im_contours,all_nR(all_views(1,v)+1,1),numIM);
    all_num_im_contours(all_views(1,v)+1,1) = num_im_contours;
end

first_anchor = all_views(1,1);
other_view = all_views(1,2);

queryID = clusters{first_anchor+1,1}(1,1);
otherID = clusters{other_view+1,1}(1,1);

curve1 = all_recs{first_anchor+1,1}{1,queryID};
curve2 = all_recs{other_view+1,1}{1,otherID};

order_curve = curve1;
size1 = size(curve1,1);
index_curve = (1:size1)';

edge_support_3d = all_edge_support_3d{first_anchor+1,1};
support1 = edge_support_3d{queryID,1};
edge_support_3d = all_edge_support_3d{other_view+1,1};
support2 = edge_support_3d{otherID,1};

numSamples = size(curve1,1);
disp('COMPUTING ASSOCIATIONS');

%The edge support for both of the curves on the other's first anchor
edge_support_for1 = support1{other_view+1,1};
edge_support_for2 = support2{first_anchor+1,1};

links1 = all_links_3d{first_anchor+1,1};
links2 = all_links_3d{other_view+1,1};
 
offsets1 = all_offsets_3d{first_anchor+1,1};
offsets2 = all_offsets_3d{other_view+1,1};
 
[edg1 edgmap] = load_edg(['./edges/',num2str(first_anchor,'%08d'),'.edg']);
[edg2 edgmap] = load_edg(['./edges/',num2str(other_view,'%08d'),'.edg']);

%Find first correspondence in the beginning

%Flags
corresp_found = 0;
is_junction = 0;
is_curve2_longer = 0;
s = 0;

while(~corresp_found)
   
    s=s+1;
    if(~isempty(edge_support_for1))
        edges = edge_support_for1{s,1};

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
                        if(curve_to_link==otherID)
                            corresp_found = 1;
                        end
                    end
                end
            else
                disp('WARNING: Weird edge ID!!!');
            end
        end

    elseif(~isempty(edge_support_for2))
       
    else
        
    end
    
end

%Find first correspondence at the end
