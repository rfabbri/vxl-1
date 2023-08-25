% clear all;
% close all;
% 
% numIM = 25;
colors = distinguishable_colors(5000);
% 
% all_recs = cell(numIM,1);
% all_nR = zeros(numIM,1);
% all_links_3d = cell(numIM,1);
% all_offsets_3d = cell(numIM,1);
% all_edge_support_3d = cell(numIM,1);
% all_flags = cell(numIM,1);
% 
% all_num_im_contours = zeros(numIM,1);

% for v=1:numIM
% 
%     mypath=['./paper-runs/pavilion-midday-half/',num2str(v-1),'/4/'];
%     %mypath=['./paper-runs/fountain/fountain-old/'];
%     recs = cell(0,0);
%     first_recs = cell(0,0);
%     
%     [ret, myfiles] = unix(['ls ' mypath '/crvs/*-3dcurve-*-points*dat | xargs echo']);
% 
%     myfiles;
% 
%     while length(myfiles) ~= 0
%       [f,rem]=strtok(myfiles);
%       myfiles = rem;
%       if length(f) == 0
%         break;
%       end
%       f = strip_trailing_blanks(f);
% 
%       r = myreadv(f);
%       if isempty(r)
%         warning(['file is empty: ' f]);
%       else
%         first_recs{1,end+1} = r;
%       end
%     end
% 
% 
%     disp(['first_ncurves = ' num2str(max(size(first_recs)))]);
% 
%     ncurves = size(first_recs,2);
%     broken_recs = cell(1,5*ncurves);
%     brokenIND = 1;
% 
%     for c=1:ncurves
% 
%         numsamples = size(first_recs{c},1);
%         startSample = 1;
% 
%         for s=2:numsamples
%             distVec = [first_recs{c}(s,1)-first_recs{c}(s-1,1) first_recs{c}(s,2)-first_recs{c}(s-1,2) first_recs{c}(s,3)-first_recs{c}(s-1,3)];
%             dist = norm(distVec);
%             if(dist > 0.1)
%                 if(s>startSample+1)
%                     broken_recs{1,brokenIND} = first_recs{c}(startSample:(s-1),:);
%                     brokenIND = brokenIND+1;
%                 end
%                 startSample = s;
%             end
%         end
%         broken_recs{1,brokenIND} = first_recs{c}(startSample:numsamples,:);
%         brokenIND = brokenIND+1;
% 
%     end
%     
%     temp_recs = cell(1,brokenIND-1);
%     numtotal=0;
% 
%     for bc=1:(brokenIND-1)
% 
%         if(size(broken_recs{1,bc},1)>=30)
%             temp_recs{1,bc} = broken_recs{1,bc};
%             numtotal=numtotal+1;
%         end
% 
%     end
% 
%     for pc=1:(brokenIND-1)
% 
%         if(~isempty(temp_recs{1,pc}))
%             recs{1,end+1} = temp_recs{1,pc};
%         end
% 
%     end
% 
%     recs = first_recs;
% 
%     disp(['final_ncurves = ' num2str(max(size(recs)))]);
%     
%     
%     %At this point, all curves are loaded into recs
%     all_recs{v,1} = recs;
%     all_nR(v,1) = size(recs,2);
%     all_flags{v,1} = zeros(size(recs,2),1);
% end

% clear all;
% close all;
% 
% load full-results.mat;

points = [];
links = [];
point_index = 1;
link_index = 1;
color_index = 2;

recs = recs';

% points = zeros(20000000,6);
% links = zeros(20000000,4);

for vv=1:1%size(all_recs,1)
    %vv
    %recs = all_recs{vv,1};
    for cc=1:size(recs,1)
        cc
        cur_branch = recs{cc,1};
        branch_size = size(cur_branch,1);
        %points = [points; zeros(2*branch_size,6)];
        %links = [links; zeros(branch_size-1,4)];
        
        branch_size = size(cur_branch,1);
        points = [points; zeros(2*branch_size-1,6)];
        links = [links; zeros(branch_size-1,3)];

        for bs=1:branch_size
            
            if(bs>1)
                mid_point = (cur_branch(bs,:) + cur_branch(bs-1,:))/2;
                points(point_index,:) = [mid_point round(255*colors(color_index,:))];
                point_index=point_index+1;
            end

            cur_color = round(255.*colors(color_index,:));
            %cur_color = [0 0 255];
            points(point_index,:) = [cur_branch(bs,:) cur_color];
            point_index=point_index+1;

            if(bs>1)
                links(link_index,:) = [point_index-4 point_index-3 point_index-2];
                link_index=link_index+1;
            end

        end
        color_index=color_index+1;
        if(color_index>5000)
            color_index = 2;
        end
        
    end
    
end

%Open the file
fid = fopen('./ply/vase-sketch.ply','w');

num_points = size(points,1);
num_links = size(links,1);

%Write the header
fprintf(fid,'ply\n');
fprintf(fid,'format ascii 1.0\n');
fprintf(fid,'element vertex ');
fprintf(fid,'%d',num_points);
fprintf(fid,'\n');
fprintf(fid,'property float x\n');
fprintf(fid,'property float y\n');
fprintf(fid,'property float z\n');
fprintf(fid,'property uchar red\n');
fprintf(fid,'property uchar green\n');
fprintf(fid,'property uchar blue\n');
fprintf(fid,'element face ');
fprintf(fid,'%d',num_links);
fprintf(fid,'\n');
fprintf(fid,'property list uchar int vertex_index\n');
fprintf(fid,'end_header\n');

for p=1:num_points
    fprintf(fid,'%f %f %f %d %d %d\n',points(p,1),points(p,2),points(p,3),points(p,4),points(p,5),points(p,6));
end

for l=1:num_links
    fprintf(fid,'%d %d %d %d %d\n',3,links(l,1),links(l,2),links(l,3));
end

fclose(fid);