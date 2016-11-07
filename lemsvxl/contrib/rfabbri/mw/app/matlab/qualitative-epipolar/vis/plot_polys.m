% To be called by epi.m or epi_polycost.m

h0 = figure(n_fig); clf
%h0 = figure(n_fig); 
axis equal;
hold on;
%set(h0,'Renderer','OpenGL')
%set(h0,'Renderer','painters')
set(h0,'Renderer','zbuffer')


h1 = figure(n_fig+1); clf
%h1 = figure(n_fig+1); 
axis equal;
hold on;
%set(h1,'Renderer','painters')
%set(h1,'Renderer','OpenGL')
set(h1,'Renderer','zbuffer')
%set(h1,'Back','OpenGL')



% ---- The domain
figure(h0);
box_pts = [
  box(1,1) box(1,2); 
  box(2,1) box(1,2); 
  box(2,1) box(2,2); 
  box(1,1) box(2,2); 
  box(1,1) box(1,2)];
fill(box_pts(:,1),box_pts(:,2),'k');

figure(h1);
fill(box_pts(:,1),box_pts(:,2),'k');


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

if (plot_costs)
  figure(h0);
  colormap(gray(max(size(polys0)))*0.9);
  figure(h1)
  colormap(gray(max(size(polys0)))*0.9);
else
  figure(h0);
  colormap(hsv(600));
%  mycmap = rand(256,3);
%  colormap(mycmap);

  figure(h1)
  colormap(hsv(600));
end


% the polygons

myalpha = 0.5;
cmap = colormap;
mycolors = zeros(max(size(polys0)),3);

figure(h0);

if (plot_costs)
  if (max(size(polys0)) ~= max(size(sorted_ip)))
    error('polys and costs not in 1-1 correspondence');
  end
  rank_order = Inf; % only plot top x-1
end


for i=1:max(size(polys0))

  if plot_costs
    ip = sorted_ip(i);
    if (max(size(polys0))-i+1 <= rank_order && vcost(i) < Inf)
      mycolor = cmap(i,:);
    else
      mycolor = [0 0 0];
    end
    mycolors(i,:) = mycolor;
  else
    ip = i;

    mycolor = rand(1,3);%cmap(mod(round(rand()*31+1)*5*ip,size(cmap,1)) + 1,:);
    mycolor(1) = min(mycolor(1)+rand()*0.5,1);
    mycolor(2) = min(mycolor(2)+rand()*0.3,1);
    mycolor(3) = min(mycolor(3)+rand()*0.3,1);
    mycolors(ip,:) = mycolor;
  end

%  disp (['ip:' num2str(ip) ' color:[' num2str(mycolor) ']']);
  for isheet = 1:max(size(polys0{ip}))
    x = polys0{ip}{isheet}(:,1);
    if (y_invert == true)
      y = img_max_y - polys0{ip}{isheet}(:,2);
    else
      y = polys0{ip}{isheet}(:,2);
    end
%    hp = fill(x,y, mycolor,'EraseMode','none');
    hp = fill(x,y, mycolor);
    set(hp,'EdgeColor',mycolor);
    alpha(hp,myalpha);
    set(hp,'EdgeAlpha',myalpha);

    if show_centroids
      ctroid = [mean(x) mean(y)];
      cplot2(ctroid,'white+');
    end
  end

end

figure(h1);
for i=1:max(size(polys0))
  if (plot_costs)
    ip = sorted_ip(i);
  else
    ip = i;
  end

  for isheet = 1:max(size(polys1{ip}))
    x = polys1{ip}{isheet}(:,1);
    y = polys1{ip}{isheet}(:,2);
    if (y_invert == true)
      y = img_max_y - polys1{ip}{isheet}(:,2);
    else
      y = polys1{ip}{isheet}(:,2);
    end
%    hp = fill(x,y,mycolor,'EraseMode','none');
    hp = fill(x,y,mycolors(i,:));
    set(hp,'EdgeColor',mycolors(i,:));
    alpha(hp,myalpha);
    set(hp,'EdgeAlpha',myalpha);

    if show_centroids
      ctroid = [mean(x) mean(y)];
      cplot2(ctroid,'white+');
    end
  end
end

% ---- The points
figure(h0);
cplot2(pts0,'whiteo');
if max(size(e0_new)) ~= 0
  e0_newi = e0_new;
  if (y_invert == true)
    e0_newi(:,2) = y_invert- e0_new(:,2);
  end
  cplot2(e0_newi,'whitex');
end
if max(size(e0)) ~= 0
  e0i = e0;
  if (y_invert == true)
    e0i(:,2) = y_invert- e0(:,2);
  end
  cplot2(e0i,'whites');
end

figure(h1);
cplot2(pts1,'whiteo');
if max(size(e0_new)) ~= 0
  e1_newi = e1_new;
  if (y_invert == true)
    e1_newi(:,2) = y_invert- e1_new(:,2);
  end
  cplot2(e1_newi,'whitex');
end
if max(size(e0)) ~= 0
  e1i = e1;
  if (y_invert == true)
    e1i(:,2) = y_invert- e1(:,2);
  end
  cplot2(e1i,'whites');
end

% label the numbers

lbl_dwn_y(1) = .03*(max(pts0(:,2))-min(pts0(:,2)));
lbl_dwn_x(1) = .03*(max(pts0(:,1))-min(pts0(:,1)));
lbl_dwn_y(2) = .03*(max(pts1(:,2))-min(pts1(:,2)));
lbl_dwn_x(2) = .03*(max(pts1(:,1))-min(pts1(:,1)));

figure(h0);
for i=1:size(pts0,1)
  text(pts0(i,1)+lbl_dwn_x(1),pts0(i,2)+lbl_dwn_y(1),num2str(i),'Color','white');
end
axis tight;
cplot2(img_domain_pts0,'w');

figure(h1);
for i=1:size(pts0,1)
  text(pts1(i,1)+lbl_dwn_x(2),pts1(i,2)+lbl_dwn_y(2),num2str(i),'Color','white');
end
axis tight;
cplot2(img_domain_pts1,'w');

