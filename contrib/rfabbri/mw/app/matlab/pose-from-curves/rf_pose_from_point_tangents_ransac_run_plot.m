figure;
hold on;
x_err = [0:0.2:3.5]; 
hi = histc(errors, x_err); 
h = plot(x_err, hi/ntrips);
set(h,'linewidth',2);
set(h,'color',[11 132 199]/255);

hi = histc(errors_gt, x_err); 
h = plot(x_err, hi/ntrips);
set(h,'linewidth',2);
set(h,'color',[255 163 71]/255);

hi = histc(err_badj, x_err); 
h = plot(x_err, hi/ntrips);
set(h,'linewidth',2);
set(h,'color',[255 50 50]/255);

ylabel('frequency');
xlabel('reprojection error');

M{1} = ['proposed method (w/o bundle adj.)'];
M{2} = ['dataset pose'];
M{3} = ['proposed method (w/ bundle adj.)'];
h=legend(M);
set(h,'Interpreter','tex');


figure 
hist([errors errors_gt])
