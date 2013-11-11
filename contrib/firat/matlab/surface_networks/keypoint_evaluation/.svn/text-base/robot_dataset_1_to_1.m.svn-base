% This is robot_dataset_1_to_1.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Oct 18, 2012

function [C1,C2] = robot_dataset_1_to_1(calib_file, pt_cloud_file, cid1, cid2, M, N)
	Morig = 1200;
	Norig = 1600;
	sf = Morig/M;
	load(calib_file);
	load(pt_cloud_file);
	C1 = -ones(M,N);
	C2 = C1;
	K = [fc(1)/sf 0 cc(1)/sf; 0 fc(2)/sf cc(2)/sf; 0 0 1];
 	eval(sprintf('P1 = K * [Rc_%d Tc_%d];',cid1,cid1));
 	eval(sprintf('P2 = K * [Rc_%d Tc_%d];',cid2,cid2));
 	X3D = [pts3D_far(1:3,:); ones(1, size(pts3D_far,2))];
 	X1 = P1 * X3D;
 	X1 = X1 ./ repmat(X1(end,:),3,1);
 	X2 = P2 * X3D;
 	X2 = X2 ./ repmat(X2(end,:),3,1);
end
