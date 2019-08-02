% ---------
%load('/home/rfabbri/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-synth/working-state-synthetic-ransac_results-paper.mat')
% 



% ------------------------------------------------------------------------------
X={};

for p=1:n_perturbs
  X{end+1} = num2str(perturb_levels(p));
end

T={};
for p=1:n_theta_perts
  T{end+1} = ['\Delta_\theta = ', num2str(theta_perturbs_deg(tp))];
end


delta = linspace(-.25,.25,n_theta_perts);  % define offsets to distinguish plots
%delta = zeros(1:n_theta_perts)+;
width = .08;  % small width to minimize overlap
%legWidth = 1.8; % make room for legend


% plot
figure
hold on
grid on
set(gca,'XGrid','off')
set(groot, 'defaultAxesTickLabelInterpreter','latex'); set(groot, 'defaultLegendInterpreter','latex');

xlabel('\Delta_{pos}');
ylabel('reprojection error');
title('Error distribution for different noise levels');

ax = gca;
ticks = zeros(1,tp);
for tp = 1:n_theta_perts
  positions = (1:n_perturbs) + delta(tp);
  nbx = size(ax.Children,1);
  bx = boxplot(all_errs{tp}', ...
    'positions', positions, 'widths', width, 'labels', X, 'colors', [0.5 0.5 0.5],...
    'symbol','.');
  %set(findobj(gcf,'tag','Box'), 'Color', red);
  nbx = size(ax.Children,1) - nbx;

  h = get(bx(5,:),{'XData','YData'});
  np = size(h,1)
  hp = hggroup;
  set(hp, 'tag','boxen')
  for k=1:np
     patch(h{k,1},h{k,2},[0.6 0.6 0.6], 'edgecolor',[0.5 0.5 0.5]-0.05, 'linewidth', 1.2,'tag', 'ricbox', 'parent', hp);%, 'facealpha',0.5);
%     ax.Children = ax.Children([end 1:end-1]);
%     ax.Children = ax.Children([2:end 1]);
  end
  %ax.Children = ax.Children([(1:nbx)+np 1:np (nbx+np+1):end]);
end

assert(n_theta_perts*2 == size(ax.Children,1))
id = 1:2*n_theta_perts
for i=0:(n_theta_perts-1)
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
set(lines, 'Color', 'k', 'linewidth', 3);

lines = findobj(gcf, 'Tag', 'Upper Whisker');
set(lines, 'lineStyle', '-');
lines = findobj(gcf, 'Tag', 'Lower Whisker');
set(lines, 'lineStyle', '-');

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

mx = -inf;
for il=1:size(lines,1)
txt = '10';
  l = lines(il);
  x = min(l.XData);
  y = max(l.YData);
  if (x < mx)
    mx = x
  end
  tp = n_theta_perts - floor((il-1)/n_perturbs);
  text(x-0.01,0-0.2,num2str(theta_perturbs_deg(tp)), 'HorizontalAlignment', 'center', 'Fontsize', 8, 'color', [0.7 0.7 0.7])
end
text(x-0.123,0-0.2,'\Delta_\theta = ', 'HorizontalAlignment', 'center', 'Fontsize', 8, 'color', [0.7 0.7 0.7])

return
% ------------------------
% to be called after rf_all_pairs_experiment_perturb
figure
clf
hold on;
M={};
for tp = 1%:n_theta_perts
  for p=1:n_perturbs
    if (tp==1 | p == 1)
      disp('skipping 0 err');
      continue;
    end

    boxplot(all_errs{tp}');


    
    mycolor = rand(1,3)*0.7;
    mycolor(1) = min(mycolor(1)+rand()*0.4,1);
    mycolor(2) = min(mycolor(2)+rand()*0.2,1);
    mycolor(3) = min(mycolor(3)+rand()*0.1,1);

    x_err = [0:0.2:7];
%    hi = histc(all_errs{tp}(p,:), x_err);
%    h = plot(x_err, hi/maxcount);

%    [hi,xout] = hist(all_errs{tp}(p,:),80);
%    h = plot(xout,hi/maxcount);

    set(h,'color',mycolor);
    set(h,'linewidth',2);
    M{end+1} = ['\Delta_{pos} = ' num2str(perturb_levels(p))...
                ', \Delta_\theta = ', num2str(theta_perturbs_deg(tp))];
  end
end
title('Error distribution for different noise levels');
ylabel('frequency');
xlabel('reprojection error');
h=legend(M);
set(h,'Interpreter','tex');





return

figure('Color', 'w');
c = colormap(lines(3));


A = randn(60,7);        % some data
A(:,4) = NaN;           % this is the trick for boxplot
C = [c; ones(1,3); c];  % this is the trick for coloring the boxes


% regular plot
boxplot(A, 'colors', C, 'plotstyle', 'compact', ...
    'labels', {'','ASIA','','','','USA',''}); % label only two categories
hold on;
for ii = 1:3
    plot(NaN,1,'color', c(ii,:), 'LineWidth', 4);
end

title('BOXPLOT');
ylabel('MPG');
xlabel('ORIGIN');
legend({'SUV', 'SEDAN', 'SPORT'});

set(gca, 'XLim', [0 8], 'YLim', [-5 5]);
