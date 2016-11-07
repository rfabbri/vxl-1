%
% Plot / compare different methods for 3D rec.
%
%function [r,crv]=plot_tracer_rec_methods()
  ccrop=true;

  r = myreadv('dat/reconstr-tracer-multi.dat');
  r_2v = myreadv('dat/reconstr-tracer-multi-2v.dat');
%  r_2v_kanatani = myreadv('dat/reconstr-tracer-multi-2v_kanatani.dat');
  r_3v = myreadv('dat/reconstr-tracer-multi-3v.dat');
  r_linear= myreadv('dat/reconstr-tracer-multi-linear.dat');

  npts = size(r,1);

%  id=5:18;
  id=15:(npts-10);

  if ccrop
    r = r(id,:);
    r_2v = r_2v(id,:);
%    r_2v_kanatani = r_2v_kanatani(id,:);
    r_3v = r_3v(id,:);
    r_linear = r_linear(id,:);
  end
  figure(113);
%  figure(1);
  clf
  hold on;
  cplot(r,'b-');
  cplot(r_linear,'r-');
  h=cplot(r_2v,'g-');
%  h=cplot(r_2v_kanatani,'k-');
%  set(h,'visible','off');
  cplot(r_3v,'c-');
  axis equal;
  axis tight;

  legend('n views optimized','n views linear','2 views', '3 views');
  xlabel x;
  ylabel y;
  zlabel z;
  title ([num2str(max(size(r))) ' pts']);

%  figure(114);
  figure(4);
  clf
  crv =  myreadv2('dat/curve2d-view0-tracer-multi.dat');
  if ccrop
    crv = crv(id,:)
  end
  cplot2(crv,'r');
  axis equal;

