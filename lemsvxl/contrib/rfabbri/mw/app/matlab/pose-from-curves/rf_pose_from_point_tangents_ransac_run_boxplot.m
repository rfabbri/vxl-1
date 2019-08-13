% ---------
clear all;

%Data
%~/lib/data/dino-iccv2011
%  subfolder iccv-experiment
%recovered plot data: paper/figs/*/dino2-error-distrib-10samples-recovered_data.mat
%also at 

distribs = cell(1,2);


% capitol
load ('~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-capitol2/working-state-capitol2-30samples-ransac_results.mat');
distribs{1} = 


% dino 
load('~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-dino2/dino2-error-distrib-10samples-recovered_data.mat');
distribs{2} = {all_errs{1}, all_errs{3}, all_errs{2}}


badj=true;

% Generate distribs 
distribs{1} = {all_}% capitol
distribs{2} = {}%dino



% Remove theta zero
%n_sub = n_sub - 1;
%theta_perturbs_deg = theta_perturbs_deg(2:end);

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

top_tags = {'capitol','dino'};
sub_tags = {'A', 'B', 'C'};  % relate to legend done by hand
n_top = 2; % capitol, dino
n_sub = 3; % p2pt, p2pt bundle, dataset
sub_axis_label = ''; 



% Renamed
% n_perturbs <---> n_top %level
% theta_perts <--> n_sub %level

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
xlabel('dataset'); 
ylabel('reprojection error');
color = min(lines(n_sub)+0.2,1);
%color = color([2 1 3:end],:)
%color = [233 83 62; 235 120 34; 0 136 195; 197 51 107; 0 125 28]/255;
%ecolor = max(color - 0.2,0);
ecolor = color;
ax = gca;
%ticks = zeros(1,n_sub);
for tp = 1:n_sub
  positions = (1:n_top) + delta(tp);
  nbx = size(ax.Children,1);
  bx = boxplot(distribs{tp}', ...
    'positions', positions, 'widths', width, 'labels', top_tags, 'colors', ecolor(tp,:),...
    'symbol','.');
  %set(findobj(gcf,'tag','Box'), 'Color', red);
  nbx = size(ax.Children,1) - nbx;

  h = get(bx(5,:),{'XData','YData'});
  np = size(h,1)
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

lines = findobj(gcf, 'type', 'line', 'Tag', 'Median');
set(lines, 'Color', [0.3 0.3 0.3], 'linewidth', 3);

lines = findobj(gcf, 'Tag', 'Upper Whisker');
set(lines, 'lineStyle', '-');
lines = findobj(gcf, 'Tag', 'Lower Whisker');
set(lines, 'lineStyle', '-');

lines = findobj(gcf, 'Tag', 'Upper Adjacent Value');
set(lines, 'linewidth', 0.5);
lines = findobj(gcf, 'Tag', 'Lower Adjacent Value');
set(lines, 'linewidth', 0.7);

m = findobj(gcf, 'Tag', 'Outliers');
set(m, 'color', [0.9 0.9 0.9], 'markerfacecolor',[0.9 0.9 0.9], 'markeredgecolor', [0.7 0.7 0.7], 'markersize',1);

%xlim([1+2*delta(1) n_sub+2*delta(n_sub)])
%boxplot(distribs{1}','plotstyle','compact');
%hold on
%boxplot(distribs{2}','plotstyle','compact');

% turn lines on
% grey bg on
% overlap boxen


lines = findobj(gcf, 'Tag', 'Upper Whisker');
mlines = findobj(gcf, 'Tag', 'Median');

mx = -inf;
for il=1:size(lines,1)
  l = lines(il);
  x = min(l.XData);
  y = max(l.YData);
  if (x < mx)
    mx = x
  end
  tp = n_sub - floor((il-1)/n_top);
  text(x,0-0.2,sub_tags{tp}, 'HorizontalAlignment', 'center', 'Fontsize', 8, 'color', color(tp,:)*0.8)
  %text(x+0.015,0-0.2,[num2str(theta_perturbs_deg(tp)) '^\circ'], 'HorizontalAlignment', 'center', 'Fontsize', 8, 'color', color(tp,:)*0.8)
  set(mlines(il), 'Color', ecolor(tp,:)*0.7);
end
if length(sub_axis_label) ~= 0
  text(min(lines(end).XData)-0.178,0-0.235,'\Delta_\theta \rightarrow', 'HorizontalAlignment', 'right', 'Fontsize', 12, 'color', 'k')
end

%set(gca,'box','off')
xlim([0.5 3.5]);
ylim([-0.4 6]);
set(gca,'plotboxaspectratio',[1 0.7857 0.7857])
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
