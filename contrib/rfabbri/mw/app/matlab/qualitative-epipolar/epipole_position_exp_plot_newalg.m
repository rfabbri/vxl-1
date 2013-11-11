figure(n_fig);
clf;
hold on;
set(gca,'color','k');
h_red0 = cplot2(e0_lst(vpass == 0,:),'redx');
axis equal;
set(h_red0,'Tag','red');
h_green0 = cplot2(e0_lst(vpass == 1,:),'greenx');
set(h_green0,'Tag','green');
figure(n_fig+1);
clf;
set(gca,'color','k');
hold on;
h_red1=cplot2(e1_lst(vpass == 0,:),'redx');
axis equal;
set(h_red1,'Tag','red');
h_green1=cplot2(e1_lst(vpass == 1,:),'greenx');
set(h_green1,'Tag','green');

%h_red = hggroup;
%h_red



%figure(n_fig_hists);clf;
%[n,xout] = hist(vmeancost_newalg(vpass == 1),10);
%hist(vmeancost_newalg(vpass == 1),20);
%n2 = (max(vmeancost_newalg) - min(vmeancost_newalg))/(xout(2) - xout(1));
%h = findobj(gca,'Type','patch');
%set(h,'FaceColor','g','EdgeColor','w')
%hold on
%hist(vmeancost_newalg,round(n2)*2);
%alpha(.3);

npass = max(size(find(vpass == 1)))
disp(['percentage pass: ' num2str((npass*100)/nruns)]);



figure(n_fig);  
hold on;
cplot2(ap0,'.');
if max(size(e0)) ~= 0
  e0i = e0;
  if (y_invert == true)
    e0i(:,2) = y_invert- e0(:,2);
  end
  cplot2(e0i,'whiteo');
end

%cplot2(box_pts,'gray');
cplot2(img_domain_pts0,'w');

figure(n_fig+1);  
hold on;
cplot2(ap1,'.');
cplot2(img_domain_pts1,'w');

if max(size(e1)) ~= 0
  e1i = e1;
  if (y_invert == true)
    e1i(:,2) = y_invert- e1(:,2);
  end
  cplot2(e1i,'whiteo');
end
