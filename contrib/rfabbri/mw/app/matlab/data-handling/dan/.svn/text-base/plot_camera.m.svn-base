function plot_camera(K,R,T,color,plot_axes)

if (nargin < 4)
    color = 'k';
end

if (nargin < 5)
    plot_axes = 0;
end

center = -R'*T;
axis_len = 0.2;

lookv = R(3,:)';
lookp = center + lookv*axis_len;

plot3(center(1),center(2),center(3),'.','Color',color,'MarkerSize',15);
plot3([center(1) lookp(1)],[center(2) lookp(2)],[center(3) lookp(3)],'-','Color',color,'LineWidth',1.1);
if (plot_axes)
    camx = R(1,:)';
    xp = center + camx*(axis_len/4);
    plot3([center(1) xp(1)],[center(2) xp(2)],[center(3) xp(3)],'-','Color','red','LineWidth',1.1);
    
    camy = R(2,:)';
    yp = center + camy*(axis_len/4);
    plot3([center(1) yp(1)],[center(2) yp(2)],[center(3) yp(3)],'-','Color','green','LineWidth',1.1);
  
end


return





