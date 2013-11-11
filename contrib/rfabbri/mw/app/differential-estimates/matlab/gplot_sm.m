clf
hold
plot(gama1_img(:,1),gama1_img(:,2),'k')
axis equal
myquiver2(gama1_g_sm,t_sm,100)
plot(gama1_g_sm(:,1),gama1_g_sm(:,2),'r')
plot(gama1_sm(:,1),gama1_sm(:,2),'rd')
%plot(gama1_sm_circ(:,1),gama1_sm_circ(:,2),'m+')
plot(gama1_round(:,1),gama1_round(:,2),'og')
plot(gama1_g(:,1),gama1_g(:,2),'g')
%knormal=k_sm/max(abs(k_sm))/5;
%plot(gama1_g_sm(:,1)+knormal,gama1_g_sm(:,2)+knormal,'c');
