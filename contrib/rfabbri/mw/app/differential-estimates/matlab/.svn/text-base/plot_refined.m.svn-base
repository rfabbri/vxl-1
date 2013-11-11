% plots results of gradient descent refinement,
% to be used after a suitable call to smooth_compute


nfig = 10;
clearfigs =01;

%color = cmap(round((63)*rand())+1,:);

color = hsv2rgb(rand()*0.95,rand()/4+0.75,0.6*rand()+0.1);

figure(nfig)
if (clearfigs), 
  clf, 
  hold on
  title 'tangent angle'
  plot(len_sm_ss,t_angle_el_fine,'r');
  plot(len_sm,t_angle_sm,'k');
end

plot(len_ref,t_angle_ref,'Color',color);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure (nfig+1)
if (clearfigs), 
  clf
  hold on
  title 'curvature derivative';
  plot(len_sm,kdot_sm,'k');
  plot(len_sm_ss,kdot_el_fine,'r');
end

plot(len_ref,kdot_ref,'Color',color);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure(nfig+2);
if (clearfigs), 
  clf, 
  hold on
  title 'curvature'

  plot(len_sm,k_sm,'k');
  plot(len_sm_ss,k_el_fine,'r');
end

plot(len_ref,k_ref,'Color',color);
