% plot_dual.m

load('./debug/struct_from_sil.mat');
dual = ss_cell2mat(dualcell);
dualpolar = dual(1:3,:).*repmat(dual(4,:),3,1);
plot3(dualpolar(1,:), dualpolar(2,:), dualpolar(3,:), '.');

filename_vrml_dualpointcloud = './result/dual_pointcloud_bust_sil21.wrl';
fid_vrmldualpc = fopen(filename_vrml_dualpointcloud, 'w+');
pcvrmlwrite(fid_vrmldualpc, dualpolar);
fclose(fid_vrmldualpc);
