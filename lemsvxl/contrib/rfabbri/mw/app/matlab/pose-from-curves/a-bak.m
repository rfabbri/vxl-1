% ------------------------------------------------------------------------------
% ------------------------------------------------------------------------------
% ------------------------------------------------------------------------------
% ------------------------------------------------------------------------------
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
