% Brandon A. Mayer
% 1/21/2010
% This file is model the appropriate time scale of a dynamic system using
% an exponential model.
% We assign a gamma distribution as a conjugate prior to the exponential
% distribution, arriving at a gamma distribution as the posterior. We are
% attempting to find the correct time scales and decide when we should
% shift our time scale.
% We use the lambda parameter of the exponential model to determine when to
% split/merge our time scale.
% We introduce the rule that we always split when necessary but when the
% time scale is very unlikely, we reset the lambda to the current MAP
% estimate.
% This conforms to our axioms that only one even can occur simultaneously
% in an interval but we should use as long as an interval as possible.
% alpha, beta = hyper parameters for the prior gamma distribution. We may
% interpret alpha as the number of prior observations, and beta as the sum
% of the prior observations. 
function time_scale = exp_time_scale_model(data_stream,alpha,beta)

