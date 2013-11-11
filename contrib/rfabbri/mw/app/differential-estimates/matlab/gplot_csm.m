% input: con
function gplot_csm(arg1,nfig)

if ischar(arg1)
  con=loadcon(arg1,'xy');
else
  con=arg1;
end

if (nargin < 2)
  nfig=10;
end



% compute smoothings


c1  = csm(con,1,1);
c2  = csm(con,1,2);
c3  = csm(con,1,3);
c4  = csm(con,1,4);
c5  = csm(con,1,5);
c10 = csm(con,1,10);
c15 = csm(con,1,15);

% plot in each window

figure(nfig)
clf
axis equal

subplot 331
hold on
axis equal
h=cplot2(con,'r.-');
title 'Original w/ Samples'
%h=cplot2(c1,'b');
%h=cplot2(c2,'g');
%set(h,'Color',[0,0.7,0]);
%h=cplot2(c3,'k');
%h=cplot2(c4,'b');
%h=cplot2(c5,'g');
%set(h,'Color',[0,0.7,0]);
%h=cplot2(c10,'k');
%h=cplot2(c15,'b');


subplot 332
h=cplot2(con,'r');
title 'Original'
axis equal

subplot 333
h=cplot2(c1,'b');
title '1 Step'
axis equal

subplot 334
h=cplot2(c2,'g');
title '2 Steps'
set(h,'Color',[0,0.7,0]);
axis equal

subplot 335
h=cplot2(c3,'k');
title '3 Steps'
axis equal

subplot 336
h=cplot2(c4,'b');
title '4 Steps'
axis equal

subplot 337
h=cplot2(c5,'g');
title '5 Steps'
set(h,'Color',[0,0.7,0]);
axis equal

subplot 338
h=cplot2(c10,'k');
title '10 Steps'
axis equal

subplot 339
h=cplot2(c15,'b');
title '15 Steps'
axis equal

figure(nfig+1)
clf
hold on 
axis equal
h=cplot2(con,'r.-');
set(h,'DisplayName','Original + Mark');
h=cplot2(con,'r');
set(h,'DisplayName','Original');

h=cplot2(c1,'b');
set(h,'DisplayName','1 step');

h=cplot2(c2,'g');
set(h,'Color',[0,0.7,0]);
set(h,'DisplayName','2 step');

h=cplot2(c3,'k');
set(h,'DisplayName','3 step');
h=cplot2(c4,'b');
set(h,'DisplayName','4 step');

h=cplot2(c5,'g');
set(h,'Color',[0,0.7,0]);
set(h,'DisplayName','5 step');

h=cplot2(c10,'k');
set(h,'DisplayName','10 step');
h=cplot2(c15,'b');
set(h,'DisplayName','15 step');

