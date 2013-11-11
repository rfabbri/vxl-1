% This is bundler_stdout_parser.m.

% Created on: Apr 16, 2013
%     Author: firat

function max_avg_rep_err = bundler_stdout_parser(outfile)
	fid = fopen(outfile, 'r');
	phrase1 = '[RunSFM] Mean error cam';
	nphrase1 = length(phrase1);
	phrase2 = 'initial motstr-SBA error';
	nphrase2 = length(phrase2);
	phrase3 = '[FixReflectionBug] Reflecting scene...';
	nphrase3 = length(phrase3);
	max_err = -Inf;
	done = false;
	while 1
		tline = fgetl(fid);
		if ~ischar(tline), break, end
		if length(tline) >= nphrase1 && all(tline(1:nphrase1) == phrase1)
			ind = find(tline == ':', 1);
			tline2 = tline(ind+2:end);
			err = sscanf(tline2, '%f', 1);
			if err > max_err
				max_err = err;
			end
		end
		if length(tline) >= nphrase2 && all(tline(1:nphrase2) == phrase2)
			max_err = -Inf;
		end
		if length(tline) >= nphrase3 && all(tline(1:nphrase3) == phrase3)
			done = true;
		end
	end
	if done
		max_avg_rep_err = max_err;
	else
		max_avg_rep_err = Inf;
	end
	fclose(fid);
end

