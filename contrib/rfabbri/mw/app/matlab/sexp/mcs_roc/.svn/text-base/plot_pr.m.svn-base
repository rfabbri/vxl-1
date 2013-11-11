% to be invoked after read_all_pr

clear M dtheta_list;
%ids_to_plot=1:length(fnames);
ids_to_plot=plotted_ids;
%ids_to_plot = [1    61    91   121 31 ]


distance_query=[3 5];
dtheta_query=[1 5 90];
min_inliers_per_view_query = [0];

%distance_query=[];
%dtheta_query=[];
%min_inliers_per_view_query = [];

best_distance_query=[5];
best_dtheta_query=[1];
best_min_inliers_per_view_query = [0];

show_best_style = true;

clf;
hold on;

mrkv=['.s*+^p'];
n=1;
plotted_ids=[];
for i=ids_to_plot
  fname = fnames{i};

  % parse the file name to find out what parameters were used
  [ret,dtheta]=unix(['echo ' fname ' |grep dtheta| sed ''s/.*dtheta_\([^-]*\)-.*/\1/g''']);
  [ret,distance]=unix(['echo ' fname ' | grep distance| sed ''s/.*distance_\([^-]*\)-.*/\1/g''']);
  [ret,min_inliers_per_view]=unix(['echo ' fname ' | grep min_inliers| sed ''s/.*min_inliers_per_view_\([^-_]*\)[-_].*/\1/g''']);
  [ret,ratio]=unix(['echo ' fname ' | grep ratio|sed ''s/.*ratio_\([^-]*\)-.*/\1/g''']);
  [ret,lonely]=unix(['echo ' fname ' | grep lonely|sed ''s/.*lonely_\([^-]*\)-.*/\1/g''']);

  dtheta = strip_trailing_blanks(dtheta);
  distance = strip_trailing_blanks(distance);
  min_inliers_per_view = strip_trailing_blanks(min_inliers_per_view);
  if length(dtheta)
    dtheta_title = '\tau_\theta=';
    if ((length(dtheta_query) ~= 0) & (eval(dtheta) ~= dtheta_query))
      continue;
    end
  else
    dtheta_title = '';
  end

  if length(distance)
    distance_title = '\tau_d=';
    if ((length(distance_query) ~= 0) & (eval(distance) ~= distance_query))
      continue;
    end

  else
    distance_title = '';
  end

  if length(min_inliers_per_view)
    min_inliers_per_view_title = '\tau_v=';
    if ((length(min_inliers_per_view_query) ~= 0) & (eval(min_inliers_per_view) ~= min_inliers_per_view_query))
      continue;
    end
  else
    min_inliers_per_view_title = '';
  end

  if length(ratio)
    ratio_title = '\alpha =';
  else
    ratio_title = '';
  end

  if length(lonely)
    lonely_title = '\tau_{\mathcal l}=';
  else
    lonely_title = '';
  end

%  mrk = mrkv(mod(n-1,length(mrkv))+1);
%  h = plot(recall{i}, precision{i}, [mrk '-']);
%  plotted_ids(end+1) = i;
%  if (show_best_style)
%    mycolor(1) = 1;
%    mycolor(2) = 0;
%    mycolor(3) = 0;
%  else
%    mycolor = rand(1,3)*0.7;
%    mycolor(1) = min(mycolor(1)+rand()*0.4,1);
%    mycolor(2) = min(mycolor(2)+rand()*0.2,1);
%    mycolor(3) = min(mycolor(3)+rand()*0.1,1);
%  end
  mrk = mrkv(mod(n-1,length(mrkv))+1);
  plotted_ids(end+1) = i;
  if (show_best_style & eval(dtheta) == best_dtheta_query & ...
      eval(distance) == best_distance_query & ...
      eval(min_inliers_per_view) == best_min_inliers_per_view_query)
    mycolor(1) = 1;
    mycolor(2) = 0;
    mycolor(3) = 0;
    mrk = 'o';
  else
%    mycolor = rand(1,3);
%    mycolor(1) = min(mycolor(1),1);
%    mycolor(2) = min(mycolor(2)+rand()*0.7,1);
%    mycolor(3) = min(0.7);
%    mycolor = hsv2rgb(mycolor);
    mycolor = rand(1,3)*0.7;
    mycolor(1) = min(mycolor(1)+rand()*0.4,1);
    mycolor(2) = min(mycolor(2)+rand()*0.2,1);
    mycolor(3) = min(mycolor(3)+rand()*0.1,1);
  end
  h = plot(recall{i}, precision{i}, [mrk '-']);
  set(h,'color',mycolor);
  set(h,'linewidth',2);
%  set(h,'markersize',8);

  M{n} = [distance_title distance ' ' dtheta_title dtheta ' ' min_inliers_per_view_title ...
  min_inliers_per_view ' ' ratio_title ratio ' ' lonely_title lonely];
  n = n+1;
end

axis([0 1 0 1.005]);
xlabel(['recall (N_+ = ' num2str(tree.dataset.ATTRIBUTE.num_positive) ' curves)'],'FontSize',12)
ylabel('precision','FontSize',12)
h=title(['PR curves for ' tree.dataset.ATTRIBUTE.name],'FontSize',12);
set(h,'Interpreter','none');
h=legend(M);
set(h,'fontsize',8);
grid on;
axis normal;
% for export:
% set (gcf,'position',[4   674   626   351]);
