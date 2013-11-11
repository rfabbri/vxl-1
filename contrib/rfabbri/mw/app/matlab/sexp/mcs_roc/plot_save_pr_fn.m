function plot_save_pr_fn(mydir)
  load([mydir '/pr.mat']);
  plot_pr;
  I = getframe(gcf);
  imwrite(I.cdata, [mydir '/allplots.png']);
