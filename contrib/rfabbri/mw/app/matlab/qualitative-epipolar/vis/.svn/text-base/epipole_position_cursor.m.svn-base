% This is to be executed after epipole_position_exp, in order to obtain a data
% point and display the epipolar line.

% NOTE: only works for clicks in first view.
if exist('hg0') && sum(ishandle('hg0'))
  delete(hg0,hg1,h_epi_0,h_epi_1);
end;

dcm_obj = datacursormode(n_fig);

c_info = getCursorInfo(dcm_obj);

p_id = c_info.DataIndex;

% Is it a blue or green point? - do this by inspecting a tag in the
% lineseries object.

tg = get(c_info.Target,'Tag');

if strcmp(tg,'green')
  val = 1;
elseif strcmp(tg,'red')
  val = 0;
else
  error('Invalid object');
end

all_vals = find(vpass == val);

% Now p_id over whole of vpass
run_id  = all_vals(p_id);
clear p_id;



p_id8 = p_id8_lst(run_id,:);

pts0 = ap0(p_id8,:);
pts1 = ap1(p_id8,:);


fm = fmatrix(pts0,pts1,'vpgl8pt');
fm8pt = fm;
fm_fail = eye(3);
fm_fail(3,3)=0;
if (norm(fm-fm_fail) > 0.0001) 
  a = null(fm);
  e0_new = [a(1)/a(3) a(2)/a(3)];
  a = null(fm');
  e1_new = [a(1)/a(3) a(2)/a(3)];
end

% Confirm we got the right thing

e0_lst(run_id,:)
c_info.Position
e0_new

e1_lst(run_id,:)
e1_new

% Now, inspect epipolar lines.

% Show lines for the 8 points in white
% Show lines for consensus set in yellow
% Show lines for remaining points in blue

%    plot_homg_line(el0_hmg); 

[hg0,hg1,el0_lst,el1_lst] = epi_plot_lines_fm(fm8pt,n_fig,n_fig+1,pts0,pts1,10000);
set(get(hg0,'Children'),'Color','y');
set(get(hg1,'Children'),'Color','y');

%[hg0,hg1,el0_lst,el1_lst] = epi_plot_lines_fm(fm8pt,n_fig,n_fig+1,pts0,pts1,10000);
%set(get(hg0,'Children'),'Color','y');
%set(get(hg1,'Children'),'Color','y');


figure(n_fig);
h_epi_0=cplot2(e0_new,'swhite');
set(h_epi_0,'MarkerSize',12);
figure(n_fig+1);
h_epi_1=cplot2(e1_new,'swhite');
set(h_epi_1,'MarkerSize',12);


figure(n_fig);  
hold on;
cplot2(pts0,'rp');
cplot2(pts0,'ro');


figure(n_fig+1);  
hold on;
cplot2(pts1,'rp');
cplot2(pts1,'ro');
