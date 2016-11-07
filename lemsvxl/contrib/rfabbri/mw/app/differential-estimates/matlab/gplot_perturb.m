%
% Input convention (variables that must exist prior to call):
%
% el: ellipse samples (see smooth_compute.m)
% pel: perturbed ellipse samples (see perturb_normal.m)
% mycs: smoothed curve (it is pel smoothed)
%

replot = 1;
plot_refined_stuff=1;
plot_posrefined_stuf=1;
plot_arc_stuff=1;
plot_eulerspiral_stuff=0;

ksm_color = 'b';
nfig = 20;

if replot
  %%%% plot analytic and perturbed

  % Plot curve
  figure(nfig)
  clf; axis equal; hold
  h=plot(el(:,1), el(:,2),'r.-');
  set(h,'DisplayName','Original Analytic')
  h=plot(pel(:,1), pel(:,2),'ro-');
  set(h,'DisplayName','Perturbed')

  % Plot curvature, etc
  figure(nfig+1)
  clf; hold
  plot(len_sm,k_el,'r');
  title 'curvature'

  figure(nfig+2)
  clf; hold
  plot(len_sm,kdot_el,'r');
  title 'curvature derivative'

  figure(nfig+3)
  clf; hold
  plot(len_sm, dk_sm,'r');
  plot(len_sm, dk_ref,'g');
  title '\Delta k'

  % Plot stuff w/o refinement:
  figure(nfig);
  h=plot(mycs(:,1), mycs(:,2),[ksm_color '.']);
  set(h,'DisplayName','CSM samples')

  if plot_eulerspiral_stuff
    h=cplot2(gama1_ref_ss,[ksm_color '-']);
    set(h,'DisplayName','Tangent-refined eulerspiral interpolation')
  end
  if plot_arc_stuff
    h=cplot2(gama1_sm_arc,[ksm_color '-']);
    set(h,'DisplayName','ARC GENO on CSM')
  end

  figure(nfig+1);
  plot(len_sm,k_sm,ksm_color);

  figure(nfig+2);
  plot(len_sm,kdot_sm,ksm_color);

  figure(nfig+4)
  clf
  hold on
  title '\Delta \theta'
  h=plot(len_sm, dt_sm_arc,ksm_color);
  set(h,'DisplayName','Dt arc on CSM')
end

figure(nfig);
if plot_posrefined_stuf
  h=cplot2(gama1_g_posref,['k' '.']);
  set(h,'DisplayName',['Posref samples;' n_it_pos])

  if plot_eulerspiral_stuff
    h=cplot2(gama1_posref_ss, ['k' '-']);
    set(h,'DisplayName',['Posref ss;' n_it_pos])
  end
  if plot_arc_stuff
    h=cplot2(gama1_posref_circ, ['k' '-']);
    set(h,'DisplayName',['Posref ARC;' n_it_pos])
  end
end

figure(nfig+1);
if plot_refined_stuff
plot(len_sm,k_ref,'g');
end
if plot_posrefined_stuf
plot(len_sm,k_posref,'k');
end

figure(nfig+2);
if plot_refined_stuff
plot(len_sm,kdot_ref,'g');
end
if plot_posrefined_stuf
plot(len_sm,kdot_posref,'k');
end


if plot_posrefined_stuf
  figure(nfig+3)
  if plot_eulerspiral_stuff
    h=plot(len_sm, dk_posref,'k');
    set(h,'DisplayName','DK eulerspiral')
  end
  if plot_arc_stuff
    h=plot(len_sm, dk_posref_arc,'k');
    set(h,'DisplayName','DK arc')
  end
  figure(nfig+4)
  if plot_eulerspiral_stuff
    h=plot(len_sm, dt_posref,'k');
    set(h,'DisplayName',['Dt PosrefSpiral;' n_it_pos])
  end
  if plot_arc_stuff
    h=plot(len_sm, dt_posref_arc,'k');
    set(h,'DisplayName',['Dt PosrefArc;' n_it_pos])
  end
end
