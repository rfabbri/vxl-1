function [prec, rec] = det_convert_dr_fppi_to_prec_rec(det_rate, fppi, num_images, num_objects)
%function [prec, rec] = det_convert_dr_fppi_to_prec_rec(det_rate, fppi, num_images, num_objects)
% Convert DR-FPPI measurements to Prec-Recall
% (c) Nhon Trinh
% May 2, 2009

% recall = detection rate !
rec = det_rate;

% to compute precision, we need TP and FP
tp = det_rate * num_objects;
fp = fppi * num_images;

prec = tp ./ (tp + fp);
