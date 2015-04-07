% to be called after rf_all_pairs_experiment_perturb
figure
clf
hold on;
M={};
mymarker = ['o'; 'x'; '.'];
n_markers = 3;
for tp = 1:n_theta_perts
  for p=1:n_perturbs
    if (tp==1 | p == 1)
      disp('skipping 0 err');
      continue;
    end
    mycolor = rand(1,3)*0.7;
    mycolor(1) = min(mycolor(1)+rand()*0.4,1);
    mycolor(2) = min(mycolor(2)+rand()*0.2,1);
    mycolor(3) = min(mycolor(3)+rand()*0.1,1);

    x_err = [0:0.2:7];
    hi = histc(all_errs{tp}(p,:), x_err);
    h = plot(x_err, hi/maxcount);

%    [hi,xout] = hist(all_errs{tp}(p,:),80);
%    h = plot(xout,hi/maxcount);

    set(h,'color',mycolor);
    set(h,'linewidth',2);
    set(h,'marker',mymarker(mod(p,n_markers)));
    M{end+1} = ['\Delta_{pos} = ' num2str(perturb_levels(p))...
                ', \Delta_\theta = ', num2str(theta_perturbs_deg(tp))];
  end
end
title('Error distribution for different noise levels');
ylabel('frequency');
xlabel('reprojection error');
h=legend(M);
set(h,'Interpreter','tex');


% Now break up the plot 


figure
clf
hold on;
M={};
tp = 4;
for p=1:n_perturbs
    if (tp==1 | p == 1)
      disp('skipping 0 err');
      continue;
    end
    mycolor = rand(1,3)*0.7;
    mycolor(1) = min(mycolor(1)+rand()*0.4,1);
    mycolor(2) = min(mycolor(2)+rand()*0.2,1);
    mycolor(3) = min(mycolor(3)+rand()*0.1,1);

    x_err = [0:0.2:7];
    hi = histc(all_errs{tp}(p,:), x_err);
    h = plot(x_err, hi/maxcount);

%    [hi,xout] = hist(all_errs{tp}(p,:),80);
%    h = plot(xout,hi/maxcount);

    set(h,'color',mycolor);
    set(h,'linewidth',2);
    M{end+1} = ['\Delta_{pos} = ' num2str(perturb_levels(p))...
                ', \Delta_\theta = ', num2str(theta_perturbs_deg(tp))];
end
title('Error distribution for different noise levels');
ylabel('frequency');
xlabel('reprojection error');
h=legend(M);
set(h,'Interpreter','tex');


figure
clf
hold on;
M={};
p = 2;
for tp = 1:n_theta_perts
    if (tp==1 | p == 1)
      disp('skipping 0 err');
      continue;
    end
    mycolor = rand(1,3)*0.7;
    mycolor(1) = min(mycolor(1)+rand()*0.4,1);
    mycolor(2) = min(mycolor(2)+rand()*0.2,1);
    mycolor(3) = min(mycolor(3)+rand()*0.1,1);

    x_err = [0:0.2:7];
    hi = histc(all_errs{tp}(p,:), x_err);
    h = plot(x_err, hi/maxcount);

%    [hi,xout] = hist(all_errs{tp}(p,:),80);
%    h = plot(xout,hi/maxcount);

    set(h,'color',mycolor);
    set(h,'linewidth',2);
    M{end+1} = ['\Delta_{pos} = ' num2str(perturb_levels(p))...
                ', \Delta_\theta = ', num2str(theta_perturbs_deg(tp))];
end
title('Error distribution for different noise levels');
ylabel('frequency');
xlabel('reprojection error');
h=legend(M);
set(h,'Interpreter','tex');

