%------------------------------------------------------------------------------
% Fabbri Boxplots Function TODO -----------------------------------------------
%------------------------------------------------------------------------------

% Inputs -----------------------------------------------------------------------
%  n_top:   number of top levels
%  n_sub:   number of sub levels 
%  distribs:  eg distribs{tp} same shape as 1st param to boxplot, transposed
%  top_tags: tags of the toplevel parts as a cell array with n_top strings
%  sub_tags: similarly for sub
%  sub_axis_label: short string to show indicating sublevel.

%distribs = all_errs_no_badj_all;
%distribs = all_errs_views_all;
distribs = {};
distribs{end+1} = all_times_views_all{end};
%distribs = all_errs_rotation_views_all;
%distribs = all_errs_translation_views_all;

% error check
szd = size(distribs{1})
for i=2:length(distribs)
    if size(distribs{i}) ~= szd
      error('distribs must have equally sized components');
    end
end

n_top = n_perts; % positional perturb
n_sub = 1; % thetas, then p3p
top_tags = cell(1,n_top);
for p=1:n_top
  top_tags{p} = num2str(perturb_levels(p));
end

sub_tags{1} = 'P3P';
sub_axis_label = '';
my_xlabel = '\Delta_{pos} (positional perturbation in pixels)';

% Renamed
% n_perturbs <---> n_top %level
% theta_perts <--> n_sub %level

assert(length(distribs) == n_sub);

% ------------------------------------------------------------------------------
%T={};
%for tp=1:n_sub
%  T{end+1} = ['\Delta_\theta = ', num2str(theta_perturbs_deg(tp))];
%end

delta = linspace(-.25,.25,n_sub);  % define offsets to distinguish plots
%delta = zeros(1:n_sub)+;
width = .08;  % small width to minimize overlap
%legWidth = 1.8; % make room for legend

% plot
figure
hold on
grid on
set(gca,'XGrid','off')
set(groot, 'defaultAxesTickLabelInterpreter','tex'); set(groot, 'defaultLegendInterpreter','latex');
%xlabel('dataset'); 
xlabel(my_xlabel);
ylabel('reprojection error (pixels)');
lines5 = lines(n_sub);
color = min(lines5+0.2,1);
%color(n_sub-1,:) = min(lines5(5,:)+0.2,1); % XXX green rather than y
%color(n_sub,:) = [0.6 0.6 0.6]; % gray for p3p
%color = color([2 1 3:end],:)
%color = [233 83 62; 235 120 34; 0 136 195; 197 51 107; 0 125 28]/255;
%ecolor = max(color - 0.2,0);
ecolor = color;
ax = gca;
for tp = 1:n_sub
  positions = (1:n_top) + delta(tp);
  nbx = size(ax.Children,1);
  bx = boxplot(distribs{tp}', ...
    'positions', positions, 'widths', width, 'labels', top_tags, 'colors', ecolor(tp,:),...
    'symbol','.');
  %set(findobj(gcf,'tag','Box'), 'Color', red);
  nbx = size(ax.Children,1) - nbx;

  h = get(bx(5,:),{'XData','YData'});
  np = size(h,1);
  hp = hggroup;
  set(hp, 'tag','boxen')
  for k=1:np
     patch(h{k,1},h{k,2},color(tp,:), 'edgecolor', ecolor(tp,:), 'linewidth', 1.5,'tag', 'ricbox', 'parent', hp);%, 'facealpha',0.3);
%     ax.Children = ax.Children([end 1:end-1]);
%     ax.Children = ax.Children([2:end 1]);
  end
  %ax.Children = ax.Children([(1:nbx)+np 1:np (nbx+np+1):end]);
end

assert(n_sub*2 == size(ax.Children,1))
id = 1:2*n_sub
for i=0:(n_sub-1)
  % swap each two in visibility
  tmp = id(2*i+1);
  id(2*i+1) = id(2*i+2);
  id(2*i+2) = tmp;
end
ax.Children = ax.Children(id);

set(findobj(gcf,'tag','Box'), 'Visible', 0);
set(gca,'xtick',1:n_top)
set(gca,'xticklabel',top_tags)
llines = findobj(gcf, 'type', 'line', 'Tag', 'Median');
set(llines, 'Color', [0.3 0.3 0.3], 'linewidth', 3);
llines = findobj(gcf, 'Tag', 'Upper Whisker');
set(llines, 'lineStyle', '-');
llines = findobj(gcf, 'Tag', 'Lower Whisker');
set(llines, 'lineStyle', '-');
llines = findobj(gcf, 'Tag', 'Upper Adjacent Value');
set(llines, 'linewidth', 0.5);
llines = findobj(gcf, 'Tag', 'Lower Adjacent Value');
set(llines, 'linewidth', 0.7);
m = findobj(gcf, 'Tag', 'Outliers');
set(m, 'color', [0.9 0.9 0.9], 'markerfacecolor',[0.9 0.9 0.9], 'markeredgecolor', [0.7 0.7 0.7], 'markersize',1);

%xlim([1+2*delta(1) n_sub+2*delta(n_sub)])
%boxplot(distribs{1}','plotstyle','compact');
%hold on
%boxplot(distribs{2}','plotstyle','compact');

% turn llines on
% grey bg on
% overlap boxen


llines = findobj(gcf, 'Tag', 'Upper Whisker');
mlines = findobj(gcf, 'Tag', 'Median');

mx = -inf;
for il=1:size(llines,1)
  l = llines(il);
  x = min(l.XData);
  y = max(l.YData);
  if (x < mx)
    mx = x
  end
  tp = n_sub - floor((il-1)/n_top);
  text(x,0-0.2,sub_tags(tp), 'HorizontalAlignment', 'center', 'Fontsize', 8, 'color', color(tp,:)*0.8)
  %text(x,0-0.1,sub_tags{tp}, 'HorizontalAlignment', 'center', 'Fontsize', 10, 'color', color(tp,:)*0.8)
  %text(x+0.015,0-0.2,[num2str(theta_perturbs_deg(tp)) '^\circ'], 'HorizontalAlignment', 'center', 'Fontsize', 8, 'color', color(tp,:)*0.8)
  set(mlines(il), 'Color', ecolor(tp,:)*0.7);
end
if length(sub_axis_label) ~= 0
  text(min(llines(end).XData)-0.178,0-0.235,'\Delta_\theta \rightarrow', 'HorizontalAlignment', 'right', 'Fontsize', 12, 'color', 'k')
end

%set(gca,'box','off')
xlim([0.5 2.5]);
ylim([-0.3 3.5]);
%xlim([0.5 2.5]);  for rotational error
%ylim([-0.1 0.3]);
set(gca,'plotboxaspectratio',[1.0000    0.5096    0.5096])
%set(gca,'plotboxaspectratio',[1.0000    0.5048    0.5048])
%ylim([-0.05 0.25]);
%set(gca,'plotboxaspectratio',[0.9180    1.0000    0.9180])
%set(gca,'plotboxaspectratio',[1 0.7857 0.7857])
%if badj
%  ylim([-0.4 3]);
%end
set(gca,'yminortick','on')
ax = gca;
ax.YRuler.Axle.ColorData = uint8([0.6*ones(1,3)*255 255]');
ax.XRuler.Axle.ColorData = uint8([0.6*ones(1,3)*255 255]');

%set(groot,'defaultAxesTickLabelInterpreter','latex');  
%set(groot,'defaulttextinterpreter','latex');
%set(groot,'defaultLegendInterpreter','latex');
%set(groot, 'defaultAxesTickLabelInterpreter','latex'); set(groot, 'defaultLegendInterpreter','latex');

%yyaxis right 
%set(gca, 'YTick', 0, 'YTickLabel', '\Delta_{\theta}')
%title('Error distribution for different noise levels');
