build_images



phi = mydt(seg2);

figure(1); 
clf;
myiso(phi,0);
%anisotropic_showiso(phi, dxdydz); 
%showiso(phi);
%figure(2); clf;
%contour(phi(:,:,30),[0 0],'r');


dxdydz = [1 1 1];
%iterations = 150;
iterations = 3;
direction = 1;
delta_t = 0.4;
%delta_t = 0.2;
T = 20;
beta = 0.2;
%moviename = 'test.mpg';
moviename = 'no';
phi2 = my_levelset(phi, img, dxdydz, iterations, direction, delta_t, T, beta, moviename);

% mywrite('ct-spheres/dat/phi-0-5-600x.dat',phi2(:));
