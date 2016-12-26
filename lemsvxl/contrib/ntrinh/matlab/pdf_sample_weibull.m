function y_vec = pdf_sample_weibull(x_vec, lambda, k)
% function y_vec = pdf_sample_weibull(lambda, k, x_vec)
% Sample a 2-parameter Weibull distribution.
% (c) Nhon Trinh
% Date: June 10, 2009

y_vec = wblpdf(x_vec, lambda, k);