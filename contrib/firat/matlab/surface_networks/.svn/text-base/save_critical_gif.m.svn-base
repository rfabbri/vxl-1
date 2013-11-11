% This is save_critical_gif.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 25, 2012

function [max_count, min_count, sad_count] = save_critical_gif(I, sigmas, outfile)
	max_count = zeros(1, length(sigmas));
	min_count = zeros(1, length(sigmas));
	sad_count = zeros(1, length(sigmas));
	for i = 1:length(sigmas)
		[S, mins, maxs, saddles] = get_min_max_saddle(I, sigmas(i));
		max_count(i) = size(maxs, 1);
		min_count(i) = size(mins, 1);
		sad_count(i) = size(saddles, 1);
		set(gcf, 'Position', get(0,'Screensize')); % maximize window
		frame = getframe(1);
		im = frame2im(frame);
		[imind,cm] = rgb2ind(im,256);
		imwrite(imind, cm, [outfile '.' num2str(i) '.png']);
		if i == 1;
			imwrite(imind,cm,outfile,'gif', 'Loopcount',inf,'DelayTime', 1.5);
		else
			imwrite(imind,cm,outfile,'gif','WriteMode','append','DelayTime', 1.5);
		end
		close all;
	end
	plot(sigmas,max_count, 'g.-', ...
	sigmas,min_count, 'r.-', ...
	sigmas,sad_count, 'b.-');
	legend('max','min','saddle')
	xlabel('sigma')
	ylabel('count')
end
