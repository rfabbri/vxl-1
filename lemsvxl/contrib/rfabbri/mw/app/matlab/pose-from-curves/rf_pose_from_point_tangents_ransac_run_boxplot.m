% ---------
clear all;

%Data
%~/lib/data/dino-iccv2011
%  subfolder iccv-experiment
%recovered plot data: paper/figs/*/dino2-error-distrib-10samples-recovered_data.mat
%also at 

distribs_dat = cell(1,2);
ns = 10000;

% dino 
%load('~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-dino2/dino2-error-distrib-10samples-recovered_data-v2.mat');
%distribs{2} = {all_errs(1,:), all_errs(3,:), all_errs(2,:)};


% recomputed for pami

% p2pt w/o badj
dino_errors = [
    1.238161310102879
   0.000000000000115
   2.696189237907118
   0.603344382672556
   0.712445112712710
   1.154838580754202
   0.948045605083592
   1.348961594902455
                   0
   1.552778359126518
    ]';

dino_err_badj = [
   1.262544808835967
   0.721025457944913
   0.392115040673545
   0.569245705355310
   0.837741017333883
   1.085752529257121
   0.202709388803569
   0.489143177800776
   0.511445489123169
   0.610981092714388
  ]';

dino_errors_gt =  [
  0.914982147970655
   0.975595171290278
   0.368327095961850
   1.368531477706658
   1.173178016333059
   0.694877879507056
   0.571526923220106
   0.128654096513985
   1.305116348164620
   1.229208325544629
  ]';

capitol_errors = [
   0.646402130627464
   3.017749274429595
   0.671322599387427
   0.922889472909766
   0.443891628692555
   0.496748022385526
   0.998638525844348
   1.955866539802135
   0.907090140706001
   0.208324665845161
   0.297868640347893
   1.923405214713215
   0.000000000000064
   1.744912120742778
   1.554666748142138
   0.945514049858211
   1.366608587961096
   1.808443750748744
   0.000000000000064
   1.366161429707266
   1.347704752795589
   0.722235924794607
   1.399332396510940
   0.000000000000064
   1.066092566262834
   0.429415418151093
   1.549790936506825
   1.491331053212976
   1.364510457033985
   1.746705362703691
  ]';

capitol_err_badj = [
   1.213079629888928
   2.308420186299915
   1.362215868378609
   0.480667377922445
   0.398038539754809
   0.468217632741553
   0.961341060806029
   1.798849486133518
   0.645944456095033
   0.104417104525235
   0.192394598394268
   1.641319088295584
   0.207485865804708
   1.557241683773924
   1.529590318940790
   0.706941094204166
   0.622475910416863
   1.266244872477158
   0.372469653387602
   0.521476502660687
   0.455128687460812
   0.140728796374306
   0.626632545152522
   0.905018036430970
   0.441843351558067
   0.254791063616379
   0.053832463165122
   0.442664390925947
   0.291858812767558
   0.053832463165122
  ]';

capitol_errors_gt = [
   2.812274154778642
   2.249937738978408
   2.560663672787010
   0.068877206496915
   0.520670495329338
   0.604003995073538
   2.293845527041773
   2.086838616254410
   1.798043767011556
   0.452005279781819
   0.396288076033937
   0.727249265212770
   0.429492022740563
   2.856778565308832
   2.713180380107624
   0.727464852193362
   0.665800530751712
   0.736120479077446
   1.140112347256656
   1.214899530071121
   0.661880726034180
   0.373110654009178
   1.430579107373789
   0.632552247268695
   0.829602652811823
   0.685363389071309
   0.668213846609725
   0.926482067374493
   0.399054729273942
   1.430579107373789
  ]';

    
distribs_dat{1} = [capitol_errors ; capitol_err_badj; capitol_errors_gt];
distribs_dat{2} = [dino_errors dino_errors dino_errors; dino_err_badj dino_err_badj dino_err_badj; dino_errors_gt dino_errors_gt dino_errors_gt];
    

%distribs_dat{2} = [x_errs(randsmpl(all_errs(1,:),1,ns)); x_errs(randsmpl(all_errs(3,:),1,ns)); x_errs(randsmpl(all_errs(2,:),1,ns))];

%distribs_dat{2} = [x_errs(randsmpl(all_errs(1,:),1,ns)); x_errs(randsmpl(all_errs(3,:),1,ns)); x_errs(randsmpl(all_errs(2,:),1,ns))];

% capitol
%load ('~/cprg/vxlprg/lemsvpe/lemsvxl/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-capitol2/capitol2-error-distrib-30samples-b_adj-recovered_data.mat');
%distribs_dat{1} = [x_errs(randsmpl(all_errs{1},1,ns)); x_errs(randsmpl(all_errs{2},1,ns)); x_errs(randsmpl(all_errs{3},1,ns))];

distribs = { [distribs_dat{1}(1,:); distribs_dat{2}(1,:)], [distribs_dat{1}(2,:); distribs_dat{2}(2,:)], [distribs_dat{1}(3,:); distribs_dat{2}(3,:)]};


% badj=true;

% Remove theta zero
%n_sub = n_sub - 1;
%theta_perturbs_deg = theta_perturbs_deg(2:end);

%------------------------------------------------------------------------------
% Fabbri Boxplots Function TODO -----------------------------------------------
%------------------------------------------------------------------------------

% Inputs -----------------------------------------------------------------------
%  n_top:   number of top levels
%  n_sub:   number of sub levels 
%  distribs:  eg distribs{tp} same shape as 1st param to boxplot, transposed
%  top_tags: tags of the toplevel parts as a cell array with n_top strings
%  sub_tags: similarly for sub
%  sub_axis_label: short string to show indicating sublevel.

top_tags = {'capitol','dino'};
sub_tags = {'a', 'b', 'c'};  % relate to legend done by hand
n_top = 2; % capitol, dino
n_sub = 3; % p2pt, p2pt bundle, dataset
sub_axis_label = ''; 

% Renamed
% n_perturbs <---> n_top %level
% theta_perts <--> n_sub %level

% ------------------------------------------------------------------------------
%T={};
%for tp=1:n_sub
%  T{end+1} = ['\Delta_\theta = ', num2str(theta_perturbs_deg(tp))];
%end

delta = linspace(-.25,.25,n_sub);  % define offsets to distinguish plots
%delta = zeros(1:n_sub)+;
width = .08;  % small width to minimize overlap
%legWidth = 1.8; % make room for legend

% plot
figure
hold on
grid on
set(gca,'XGrid','off')
set(groot, 'defaultAxesTickLabelInterpreter','tex'); set(groot, 'defaultLegendInterpreter','latex');
%xlabel('dataset'); 
ylabel('reprojection error (pixels)');
color = min(lines(n_sub)+0.2,1);
lines5 = lines(5);
color(n_sub,:) = min(lines5(5,:)+0.2,1); % XXX green rather than y
%color = color([2 1 3:end],:)
%color = [233 83 62; 235 120 34; 0 136 195; 197 51 107; 0 125 28]/255;
%ecolor = max(color - 0.2,0);
ecolor = color;
ax = gca;
%ticks = zeros(1,n_sub);
for tp = 1:n_sub
  positions = (1:n_top) + delta(tp);
  nbx = size(ax.Children,1);
  bx = boxplot(distribs{tp}', ...
    'positions', positions, 'widths', width, 'labels', top_tags, 'colors', ecolor(tp,:),...
    'symbol','.');
  %set(findobj(gcf,'tag','Box'), 'Color', red);
  nbx = size(ax.Children,1) - nbx;

  h = get(bx(5,:),{'XData','YData'});
  np = size(h,1)
  hp = hggroup;
  set(hp, 'tag','boxen')
  for k=1:np
     patch(h{k,1},h{k,2},color(tp,:), 'edgecolor', ecolor(tp,:), 'linewidth', 1.5,'tag', 'ricbox', 'parent', hp);%, 'facealpha',0.3);
%     ax.Children = ax.Children([end 1:end-1]);
%     ax.Children = ax.Children([2:end 1]);
  end
  %ax.Children = ax.Children([(1:nbx)+np 1:np (nbx+np+1):end]);
end

assert(n_sub*2 == size(ax.Children,1))
id = 1:2*n_sub
for i=0:(n_sub-1)
  % swap each two in visibility
  tmp = id(2*i+1);
  id(2*i+1) = id(2*i+2);
  id(2*i+2) = tmp;
end
ax.Children = ax.Children(id);

set(findobj(gcf,'tag','Box'), 'Visible', 0);

set(gca,'xtick',1:n_top)
set(gca,'xticklabel',top_tags)

lines = findobj(gcf, 'type', 'line', 'Tag', 'Median');
set(lines, 'Color', [0.3 0.3 0.3], 'linewidth', 3);

lines = findobj(gcf, 'Tag', 'Upper Whisker');
set(lines, 'lineStyle', '-');
lines = findobj(gcf, 'Tag', 'Lower Whisker');
set(lines, 'lineStyle', '-');

lines = findobj(gcf, 'Tag', 'Upper Adjacent Value');
set(lines, 'linewidth', 0.5);
lines = findobj(gcf, 'Tag', 'Lower Adjacent Value');
set(lines, 'linewidth', 0.7);

m = findobj(gcf, 'Tag', 'Outliers');
set(m, 'color', [0.9 0.9 0.9], 'markerfacecolor',[0.9 0.9 0.9], 'markeredgecolor', [0.7 0.7 0.7], 'markersize',1);

%xlim([1+2*delta(1) n_sub+2*delta(n_sub)])
%boxplot(distribs{1}','plotstyle','compact');
%hold on
%boxplot(distribs{2}','plotstyle','compact');

% turn lines on
% grey bg on
% overlap boxen


lines = findobj(gcf, 'Tag', 'Upper Whisker');
mlines = findobj(gcf, 'Tag', 'Median');

mx = -inf;
for il=1:size(lines,1)
  l = lines(il);
  x = min(l.XData);
  y = max(l.YData);
  if (x < mx)
    mx = x
  end
  tp = n_sub - floor((il-1)/n_top);
  text(x,0-0.1,sub_tags{tp}, 'HorizontalAlignment', 'center', 'Fontsize', 10, 'color', color(tp,:)*0.8)
  %text(x+0.015,0-0.2,[num2str(theta_perturbs_deg(tp)) '^\circ'], 'HorizontalAlignment', 'center', 'Fontsize', 8, 'color', color(tp,:)*0.8)
  set(mlines(il), 'Color', ecolor(tp,:)*0.7);
end
if length(sub_axis_label) ~= 0
  text(min(lines(end).XData)-0.178,0-0.235,'\Delta_\theta \rightarrow', 'HorizontalAlignment', 'right', 'Fontsize', 12, 'color', 'k')
end

%set(gca,'box','off')
xlim([0.5 2.5]);
ylim([-0.3 3.5]);
set(gca,'plotboxaspectratio',[1.0000    0.5096    0.5096])
%set(gca,'plotboxaspectratio',[1.0000    0.5048    0.5048])
%ylim([-0.05 0.25]);
%set(gca,'plotboxaspectratio',[0.9180    1.0000    0.9180])
%set(gca,'plotboxaspectratio',[1 0.7857 0.7857])
%if badj
%  ylim([-0.4 3]);
%end
set(gca,'yminortick','on')
ax = gca;
ax.YRuler.Axle.ColorData = uint8([0.6*ones(1,3)*255 255]');
ax.XRuler.Axle.ColorData = uint8([0.6*ones(1,3)*255 255]');

%set(groot,'defaultAxesTickLabelInterpreter','latex');  
%set(groot,'defaulttextinterpreter','latex');
%set(groot,'defaultLegendInterpreter','latex');
%set(groot, 'defaultAxesTickLabelInterpreter','latex'); set(groot, 'defaultLegendInterpreter','latex');

%yyaxis right 
%set(gca, 'YTick', 0, 'YTickLabel', '\Delta_{\theta}')
%title('Error distribution for different noise levels');
