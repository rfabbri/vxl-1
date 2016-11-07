% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector/third_order_edge_detector_wrapper.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 20, 2011

function [edg, edgemap] = third_order_edge_detector_wrapper(img_file, sigma, grad_thresh, N)
	alphabet = 'a' + (0:25);
	do_delete = 0;
	rand_img_file = [alphabet(unidrnd(26,1,15)) '.png'];
	if ~ischar(img_file)
		I = img_file;				
	else
		I = imread(img_file);		
	end	
	zero_pad = 8;
	%I = padarray(I,[zero_pad zero_pad]);
	I = [repmat(I(1,:),zero_pad,1);I];
	I = [I;repmat(I(end,:),zero_pad,1)];
	I = [repmat(I(:,1), 1, zero_pad), I];
	I = [I, repmat(I(:,end), 1, zero_pad)];
	imwrite(uint8(I), rand_img_file);
	rand_out = [alphabet(unidrnd(26,1,15)) '.edg'];
	cmd = sprintf('!/home/firat/lemsvxl/bin/contrib/firat/third_order_edge_detector/third_order_edge_detector %s %s %f %f %d', rand_img_file, rand_out, sigma, grad_thresh, N);
	eval(cmd);
	[edg, edgemap] = load_edg(rand_out);
	edgemap = edgemap(zero_pad+1:end-zero_pad, zero_pad+1:end-zero_pad);
	edg(:,1:2) = edg(:,1:2) - zero_pad;
	cmd2 = sprintf('!rm -rf %s', rand_out);
	eval(cmd2);
	cmd3 = sprintf('!rm -rf %s', rand_img_file);
	eval(cmd3);	
end
