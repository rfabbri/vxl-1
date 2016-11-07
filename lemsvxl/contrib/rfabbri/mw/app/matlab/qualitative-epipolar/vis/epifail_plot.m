% Use after
% load epifali.mat
% [d,e0_best,e1_best,epi_s,rand_pts0,rand_pts1] = epi_cost_of_polygon(specific_poly0{2},specific_poly1{2},pts0,pts1);
%   This takes ~ 12s for 50 pps
%
%


n_fig = 5;

h0 = n_fig;
h1 = n_fig+1;

ip0 = 2;
ip1 = 2;

% ---- The points
figure(h0);
clf;
cplot2(pts0,'whiteo');
axis equal;
hold on;
if max(size(e0)) ~= 0
  e0i = e0;
  cplot2(e0i,'whites');
end

figure(h1);
clf;
cplot2(pts1,'whiteo');
axis equal;
hold on;
if max(size(e0)) ~= 0
  e1i = e1;
  cplot2(e1i,'whites');
end

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


figure(h0);
x = specific_poly0{ip0}(:,1);
y = specific_poly0{ip0}(:,2);
hp = fill(x,y,[0.7 0.7 0.7]);
alpha(hp,0.8);
set(hp,'EdgeAlpha',0.8);
c0 = [mean(x); mean(y)];

figure(h1);
x = specific_poly1{ip1}(:,1);
y = specific_poly1{ip1}(:,2);
hp = fill(x,y,[0.7 0.7 0.7]);
alpha(hp,0.8);
set(hp,'EdgeAlpha',0.8);
c1 = [mean(x); mean(y)] ;


if exist('e0_best')
    figure(n_fig);
    hold on;
    cplot2(e0_best','rs');
    cplot2(c0','w+');
    figure(n_fig+1);
    hold on;
    cplot2(e1_best','rs');
    cplot2(c1','w+');
end



%rand_pts0 = random_polygon_points(specific_poly0{2},1000);
%rand_pts1 = random_polygon_points(specific_poly1{2},1000);

figure(n_fig);
cplot2(rand_pts0,'b.');

figure(n_fig+1)
cplot2(rand_pts1,'b.');


%
% [d,e0_best,e1_best,epi_s,rand_pts0,rand_pts1,dr,ir] = ...
%     epi_cost_of_polygon_random(specific_poly0{2},specific_poly1{2},pts0,pts1);
%
% figure(n_fig+1)
% hold on;
 stem3(rand_pts1(:,1),rand_pts1(:,2),dr,'fill')
 axis tight;

% hold on;
% x = specific_poly1{ip1}(:,1);
% y = specific_poly1{ip1}(:,2);
% hp = fill(x,y,[0.7 0.7 0.7]);
