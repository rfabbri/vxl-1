
suf='-cam01'; %:< suffix
suf2='-cam12'; %:< suffix
ext='.dat';
plot_two_recs = 1;

show_occl

con=myreadv2(['ct-spheres/dat/analytic-reconstr-2-0-con' suf ext]);
con_match=myreadv2(['ct-spheres/dat/analytic-reconstr-2-0-conmatch' suf ext]);
valid=myread(['ct-spheres/dat/analytic-reconstr-2-0-valid' suf ext]);

figure(50);
clf
axis equal
hold on

cplot2(con);
con_match_ok=con_match(valid==1,:);
cplot2(con_match_ok,'r.');
cplot2(v2,'r');

for i=1:size(valid,1)
  if (con_match(i,:) ~= 0)
    if(valid(i))
      cplot2([con(i,:); con_match(i,:)],'g');
    else
      cplot2([con(i,:); con_match(i,:)],'r');
    end
  end
end

figure(15);
hold on;
[x,y,z]=sphere(40);
x=5*x;
y=5*y;
z=5*z + 60;
hs=surf(x,y,z);
set(hs,'LineStyle','none')
alpha(0.1);

c3=plotrec_f(suf,ext,'.');
if (plot_two_recs)
  c3_2=plotrec_f(suf2,ext,'.g');
end

% Show camera centers & perturbation

% figure(1923);
% clf
% hold on;
% cplot(c0','x')
% xlabel x;
% ylabel y;
% zlabel z;
% cplot(c1','xr')
% cplot([c0 c1+vp]','k');
% 
% figure(1924);
% clf
% cplot(c0','x')
% hold on;
% cplot(c1','xr')
% cplot([c0 (c1+vp)]','k');
% cplot([c0 (c1)]','g');
% plotrec_f;
% cplot(G);
% cplot(G2,'r');
