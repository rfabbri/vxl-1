clear all;
close all;

load weird_edges.mat;

n=24;

fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
curP = (fscanf(fid,'%f',[4 3]))';

[edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);
supporting_edg = [];

curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
h=figure;imshow(curIM);
set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
hold on;

num_links = size(weird_edges,1);

for ln=1:num_links
   cur_link = weird_edges(ln,:);
   
   if(cur_link(1,1)==n)
       edge_stats = edg(cur_link(1,2)+1,:);
       supporting_edg = [supporting_edg; edge_stats];
   end
   
end

disp_edg(supporting_edg,0);

hold off;