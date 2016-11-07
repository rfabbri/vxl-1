%
% inputs: supports, recs
%
top_n = 55; % top 55
newrecs = orig_recs;
newsupports = supports;

tau_t = 0;
while length(newrecs) > top_n
  [newrecs,newsupports] = mcs_prune_curves(newrecs, newsupports, tau_t);
  tau_t = tau_t + 10;
end
