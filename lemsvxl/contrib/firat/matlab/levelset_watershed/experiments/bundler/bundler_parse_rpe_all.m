% This is bundler_parse_rpe_all.m.

% Created on: Apr 16, 2013
%     Author: firat

function bundler_parse_rpe_all(main_folder, cp_type)
	p = .1:.1:1;
	minval = Inf;
	ind = -1;
	for i = 1:length(p)
		max_avg_rep_err = bundler_stdout_parser(sprintf('%s/%s_%d_result/out', main_folder, cp_type, uint8(100*p(i))));
		if minval > max_avg_rep_err
			minval = max_avg_rep_err;
			ind = uint8(100*p(i));
		end
	end
	minval
	ind
end

