% ---------
clear all;

% by default this loads all state variables
% load('/home/rfabbri/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/working-state-synthetic-ransac_results-paper.mat')


load ('~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/work-views-77-78/all_pairs_experiment_perturb-maxcount_5117-ransac-sph.mat')

n_theta_perts = max(size(theta_perturbs_deg));
n_perturbs = max(size(perturb_levels));
% Input: 
%   - perturb_levels
%   - all_errs
%   - n_theta_perts
%   - n_perturbs

% ------------------------------------------------------------------------------

% TODO consolidate all_errs_*views to all_errs XXX
% put p3p last

% Remove theta zero
%n_theta_perts = n_theta_perts - 1;
%theta_perturbs_deg = theta_perturbs_deg(2:end);

n_perturbs = n_perturbs -1;
perturb_levels = perturb_levels(2:end);
for tp=1:(n_theta_perts+1)
  all_errs{tp} = all_errs{tp}(2:end,:);
end

% ------------------------------------------------------------------------------
X={};

for p=1:n_perturbs
  X{end+1} = num2str(perturb_levels(p));
end

%T={};
%for tp=1:n_theta_perts
%  T{end+1} = ['\Delta_\theta = ', num2str(theta_perturbs_deg(tp))];
%end


delta = linspace(-.25,.25,n_theta_perts+1);  % define offsets to distinguish plots
%delta = zeros(1:n_theta_perts)+;
width = .08;  % small width to minimize overlap
%legWidth = 1.8; % make room for legend


% plot
figure
hold on
grid on
set(gca,'XGrid','off')
set(groot, 'defaultAxesTickLabelInterpreter','tex'); set(groot, 'defaultLegendInterpreter','latex');

xlabel('\Delta_{pos}');
ylabel('reprojection error');

color = min(lines(n_theta_perts+1)+0.2,1);
%color = color([2 1 3:end],:)


%color = [233 83 62; 235 120 34; 0 136 195; 197 51 107; 0 125 28]/255;

%ecolor = max(color - 0.2,0);
ecolor = color;

%-------------------------------------------------------------------------------
ax = gca;
ticks = zeros(1,tp);
for tp = 1:(n_theta_perts+1)
  positions = (1:n_perturbs) + delta(tp);
  nbx = size(ax.Children,1);
  bx = boxplot(all_errs{tp}', ...
    'positions', positions, 'widths', width, 'labels', X, 'colors', ecolor(tp,:),...
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

%-------------------------------------------------------------------------------

assert((n_theta_perts+1)*2 == size(ax.Children,1))
id = 1:2*(n_theta_perts+1)
for i=0:((n_theta_perts+1)-1)
  % swap each two in visibility
  tmp = id(2*i+1);
  id(2*i+1) = id(2*i+2);
  id(2*i+2) = tmp;
end
ax.Children = ax.Children(id);

set(findobj(gcf,'tag','Box'), 'Visible', 0);

set(gca,'xtick',1:n_perturbs)
set(gca,'xticklabel',X)

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

%xlim([1+2*delta(1) n_theta_perts+2*delta(n_theta_perts)])
%boxplot(all_errs{1}','plotstyle','compact');
%hold on
%boxplot(all_errs{2}','plotstyle','compact');

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
  tp = n_theta_perts +1 - floor((il-1)/n_perturbs);
  if tp == n_theta_perts+1 % p3p
    text(x,0-0.2,num2str('p3p'), 'HorizontalAlignment', 'center', 'Fontsize', 8, 'color', color(tp,:)*0.8)
  else
    text(x,0-0.2,num2str(theta_perturbs_deg(tp)), 'HorizontalAlignment', 'center', 'Fontsize', 8, 'color', color(tp,:)*0.8)
  end
  %text(x+0.015,0-0.2,[num2str(theta_perturbs_deg(tp)) '^\circ'], 'HorizontalAlignment', 'center', 'Fontsize', 8, 'color', color(tp,:)*0.8)
  set(mlines(il), 'Color', ecolor(tp,:)*0.7);
end
text(min(lines(end).XData)-0.178,0-0.235,'\Delta_\theta \rightarrow', 'HorizontalAlignment', 'right', 'Fontsize', 12, 'color', 'k')

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
