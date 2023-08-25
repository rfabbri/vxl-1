% clear all;
% close all;
% 
% %load curve_graph_fountain.mat;
% load curve_graph_amsterdam.mat;

diff_factor = 0.000001;
points = [];
links = [];
colors = distinguishable_colors(200);
%Start from 2 to skip blue
color_index = 2;
point_index = 1;
link_index = 1;
size(complete_curve_graph,1)
% for ccg=1:size(complete_curve_graph,1)
%     cur_branch = complete_curve_graph{ccg,1};
%     branch_size = size(cur_branch,1);
%     startSample = 1;
%     firstpart = 1;
% 
%         for s=2:branch_size
%             distVec = [cur_branch(s,1)-cur_branch(s-1,1) cur_branch(s,2)-cur_branch(s-1,2) cur_branch(s,3)-cur_branch(s-1,3)];
%             dist = norm(distVec);
%             if(dist > 10^10)
%                 if(s>startSample+1)
%                     if(firstpart)
%                         complete_curve_graph{ccg,1} = cur_branch(startSample:(s-1),:);
%                         firstpart = 0;
%                     else
%                         complete_curve_graph = [complete_curve_graph; cell(1,1)];
%                         cg_size = size(complete_curve_graph,1);
%                         complete_curve_graph{cg_size,1} = cur_branch(startSample:(s-1),:);
%                     end
%                 else
%                     disp('ERROR! Something wrong with samples');
%                 end
%                 startSample = s;
%             end
%         end
%         if(~firstpart)
%             complete_curve_graph = [complete_curve_graph; cell(1,1)];
%             cg_size = size(complete_curve_graph,1);
%             complete_curve_graph{cg_size,1} = cur_branch(startSample:branch_size,:);
%         end
% 
% end

size(complete_curve_graph,1)
for ccg=1:size(complete_curve_graph,1)
    cur_branch = complete_curve_graph{ccg,1};
    
    if(~complete_lock_map(ccg,1) && ~complete_lock_map(ccg,2) && get_length(cur_branch) < 0.0001)
        continue;
    end
    
    branch_size = size(cur_branch,1);
    points = [points; zeros(2*branch_size-1,6)];
    links = [links; zeros(branch_size-1,3)];
    
    for bs=1:branch_size
        
        if(bs>1)
            mid_point = (cur_branch(bs,:) + cur_branch(bs-1,:))/2;
            points(point_index,:) = [mid_point round(255*colors(color_index,:))];
            point_index=point_index+1;
        end
            
        
        points(point_index,:) = [cur_branch(bs,:) round(255*colors(color_index,:))];
        point_index=point_index+1;
%         points(point_index,:) = [cur_branch(bs,:)+diff_factor round(255*colors(color_index,:))];
%         point_index=point_index+1;
        
        if(bs>1)
            links(link_index,:) = [point_index-4 point_index-3 point_index-2];
            link_index=link_index+1;
        end
        
    end
    
    color_index=color_index+1;
    if(color_index>200)
        color_index=2;
    end
    
end

%Open the file
fid = fopen('./ply/curve_graph_pavilion_mixed_9.ply','w');

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

%Open the file
fid = fopen('./ply/curve_junctions_pavilion_mixed_9.ply','w');

num_points = size(all_junctions,1);

%Write the header
fprintf(fid,'ply\n');
fprintf(fid,'format ascii 1.0\n');
fprintf(fid,'element vertex ');
fprintf(fid,'%d',num_points);%num_points);
fprintf(fid,'\n');
fprintf(fid,'property float x\n');
fprintf(fid,'property float y\n');
fprintf(fid,'property float z\n');
fprintf(fid,'property uchar red\n');
fprintf(fid,'property uchar green\n');
fprintf(fid,'property uchar blue\n');
fprintf(fid,'end_header\n');

for p=1:num_points
    fprintf(fid,'%f %f %f %d %d %d\n',all_junctions(p,1),all_junctions(p,2),all_junctions(p,3),255,255,255);
end

fclose(fid);
