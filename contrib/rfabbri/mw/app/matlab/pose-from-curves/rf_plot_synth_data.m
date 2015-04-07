% To be called after rf_pose_from_point_tangents_root_find

% true if plot stuff for all_pairs experiment
all_pairs = true;

% uncomment next block to see entire data when available
clf;
cplot([0,0,0],'o');
hold on
cplot(Gama_all_cam,'.');
cplot(Gama1_cam','rs');
cplot(Gama2_cam','gs');
cplot(gama_all,'k.');
cplot(gama1','ro')
cplot(gama2','go');
axis equal;

if (all_pairs)
  load('all-pairs.mat');
  max_ae = max(allerrs');
  max_ae_ok = max_ae;
  max_ae_ok(max_ae==-1)=0;

  x_fail = find(max_ae == -1);
  y_fail = zeros(size(x_fail));

  bad_ratio = max(size(find(abs(max_ae) > 0.01)))/max(size(max_ae));

  cplot(Gama_all_cam(ids1(x_fail),:),'ro');
  cplot(Gama_all_cam(ids2(x_fail),:),'go');

  x_bad_error = find(max_ae > 0.01);
  h = cplot(Gama_all_cam(ids1(x_bad_error),:),'ro');
  set(h,'markersize',9);
  h=cplot(Gama_all_cam(ids2(x_bad_error),:),'go');
  set(h,'markersize',9);
  
end

%cplot([C_gt'; Gama1],'r');
%cplot([C_gt'; Gama2],'r');
%cplot(Gama1,'g.');
%cplot(Gama2,'gs');

%cplot([C_gt'; Gama1],'r');
%cplot([C_gt'; Gama2],'r');

%cplot([C_gt'; gama1'],'k');
%cplot([C_gt'; gama2'],'k');

%axis equal
%axis tight

for i=1:length(t_vector);
  yy(i) = rf_pose_from_point_tangents_2_fn_t(t_vector(i));
end

figure;
clf;
plot(t_vector, yy);
hold on;
plot(ts, zeros(size(ts)),'ro');
grid on;
%title(['data ids: ' num2str(id1) '-' num2str(id2)]);

