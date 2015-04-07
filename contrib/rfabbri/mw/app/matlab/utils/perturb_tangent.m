function [pert_tgt] = perturb_tangent(tgt, delta_theta)
% given a nx2 tangent vector tgt, and tangent perturbation level delta_theta in
% radians, this perturbs the tangents.

pert_tgt = tan(delta_theta);

%pert_tgt = tgt + pert_tgt*(rand(size(tgt))-0.5);
pert_tgt = tgt + pert_tgt*(2*rand(size(tgt))-1);

ssum = sqrt(sum(pert_tgt.*pert_tgt, 2));
norms =  [ssum ssum];

pert_tgt = pert_tgt ./ norms;
