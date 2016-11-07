clf
hold on
axis equal
cplot2(gama1_sm_arc)
cplot2(mycs,'.')
h=myquiver2(mycs,dk_vec_arc,1);
set(h,'LineWidth',6);
set(h,'Color','c');
set(h,'ShowArrowHead','off');
set(h,'DisplayName','D_K');
myquiver2(mycs,dt_vec_arc,1);
