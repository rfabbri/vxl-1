% Use after epipole_position_conditioning
%
%

img_domain_pts0 = [
  0    0
  0    im0_ymax
  im0_xmax im0_ymax
  im0_xmax 0
  0    0
];

img_domain_pts1 = [
  0    0
  0    im1_ymax
  im1_xmax im1_ymax
  im1_xmax 0
  0    0
];


n_fig = 1;

h0 = n_fig;
h1 = n_fig+1;

figure(h0);
clf;
figure(h1);
clf;
%% Plot epilines
if max(size(e0_gt)) ~= 0
  [cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles_35(e0_gt',e1_gt',pts0,pts1);
  [hg0,hg1,el0_lst,el1_lst] = epi_plot_lines_epi(epi_s,n_fig,n_fig+1,pts0,pts1);
  
  set(get(hg0,'children'),'color','y');
  set(get(hg1,'children'),'color','y');
end

if exist('e0_gt_opt')
    [cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles_35(e0_gt_opt',e1_gt_opt',pts0,pts1);
    [hg0,hg1,el0_lst,el1_lst] = epi_plot_lines_epi(epi_s,n_fig,n_fig+1,pts0,pts1);
    
    set(get(hg0,'children'),'color','m');
    set(get(hg1,'children'),'color','m');
end

if exist('e0_far')
    [cost,vv,n_inst,epi_s] = epi_estimate_h_3pt_epipoles_35(e0_far',e1_far',pts0,pts1);
    [hg0,hg1,el0_lst,el1_lst] = epi_plot_lines_epi(epi_s,n_fig,n_fig+1,pts0,pts1);

    set(get(hg0,'children'),'color','c');
    set(get(hg1,'children'),'color','c');
end

% ---- The points
figure(h0);
cplot2(pts0,'whiteo');
axis equal;
hold on;
if max(size(e0_gt)) ~= 0
  e0i = e0_gt;
  h=cplot2(e0i,'ys');
  set(h,'markersize',get(h,'markersize')+5);
end
cplot2(img_domain_pts0,'w');

figure(h1);
cplot2(pts1,'whiteo');
axis equal;
hold on;
if max(size(e0_gt)) ~= 0
  e1i = e1_gt;
  h=cplot2(e1i,'ys');
  set(h,'markersize',get(h,'markersize')+5);
end
cplot2(img_domain_pts1,'w');



% label the numbers

lbl_dwn_y(1) = .03*(max(pts0(:,2))-min(pts0(:,2)));
lbl_dwn_x(1) = .03*(max(pts0(:,1))-min(pts0(:,1)));
lbl_dwn_y(2) = .03*(max(pts1(:,2))-min(pts1(:,2)));
lbl_dwn_x(2) = .03*(max(pts1(:,1))-min(pts1(:,1)));

figure(h0);
set(gca,'color','k');
for i=1:size(pts0,1)
  text(pts0(i,1)+lbl_dwn_x(1),pts0(i,2)+lbl_dwn_y(1),num2str(i),'Color','white');
end
%axis tight;

figure(h1);
set(gca,'color','k');
for i=1:size(pts0,1)
  text(pts1(i,1)+lbl_dwn_x(2),pts1(i,2)+lbl_dwn_y(2),num2str(i),'Color','white');
end
%axis tight;


if exist('e0_gt_opt')
    figure(n_fig);
    h=cplot2(e0_gt_opt,'ms');
    set(h,'markersize',get(h,'markersize')+5);
    figure(n_fig+1);
    h=cplot2(e1_gt_opt,'ms');
    set(h,'markersize',get(h,'markersize')+5);
end

if exist('e0_far')
    figure(n_fig);
    h=cplot2(e0_far,'cs');
    set(h,'markersize',get(h,'markersize')+5);
    figure(n_fig+1);
    h=cplot2(e1_far,'cs');
    set(h,'markersize',get(h,'markersize')+5);
end

%rand_pts0 = random_polygon_points(specific_poly0{2},1000);
%rand_pts1 = random_polygon_points(specific_poly1{2},1000);

%figure(n_fig);
%cplot2(rand_pts0,'b.');

%figure(n_fig+1)
%cplot2(rand_pts1,'b.');
