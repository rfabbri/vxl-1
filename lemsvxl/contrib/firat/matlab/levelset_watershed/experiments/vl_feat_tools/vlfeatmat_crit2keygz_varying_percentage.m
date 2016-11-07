% This is vlfeatmat_crit2keygz_varying_percentage.m.

% Created on: Apr 16, 2013
%     Author: firat

function vlfeatmat_crit2keygz_varying_percentage(mat_folder, out_folder)
	p = 1:-.1:.1;
	for i = 1:length(p)
		minkeygz_folder = sprintf('%s/min_%d', out_folder, uint8(100*p(i)));
		maxkeygz_folder = sprintf('%s/max_%d', out_folder, uint8(100*p(i)));
		saddlekeygz_folder = sprintf('%s/saddle_%d', out_folder, uint8(100*p(i)));
		vlfeatmat_crit2keygz(mat_folder, minkeygz_folder, maxkeygz_folder, saddlekeygz_folder, p(i));
	end
end

