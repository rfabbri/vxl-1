% see lemsvpe/doc/3d-curve-drawing.md


yy = load('curve_graph_amsterdam.mat');

Curves = yy.complete_curve_graph;    %> All curve points (could be very noisy)

sz = [size(Curves, 1) 3];
contour_RGB_color = unifrnd(0,1,sz);

figure;
for ci = 1:size(Curves, 1)
    curve = Curves{ci, 1};
    if isempty(curve), continue; end
    %if size(curve, 1) < 250, continue; end
    plot3(curve(:,1), curve(:,2), curve(:,3), 'Color', contour_RGB_color(ci,:), 'Marker', '.', 'MarkerSize', 5); 
    hold on;
end
xlabel("x");
ylabel("y");
zlabel("z");
axis equal;
set(gcf,'color','w');
