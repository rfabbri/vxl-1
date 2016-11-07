npix_x = 1024;
npix_y = 768;
[Ks, Rs, Ts, pts] = read_bundle_file('bundle.out', npix_x, npix_y);
clf;
cplot(pts','g.');
hold on;
ncams = max(size(Ks));
for i=1:ncams; 
  plot_camera(Ks{i}, Rs{i}, Ts{i});
end

axis equal;
