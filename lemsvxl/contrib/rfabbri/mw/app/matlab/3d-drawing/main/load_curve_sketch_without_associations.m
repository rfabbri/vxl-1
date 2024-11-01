% to be called in _main.m
% [variaveis definidas abaixo] = f(variaveis ja definidas)

for v=1:numIM
    
    recs = cell(0,0);
    tangs = cell(0,0);
    %mypath = load_pattern(v);
    
    %put the path that accesses the .dat files below before crvs
    mypath = load_pattern_12(v);

    [ret, myfiles] = unix(['ls ' mypath '/crvs/*-3dcurve-*-points*dat | xargs echo']);
    [ret_t, myfiles_t] = unix(['ls ' mypath '/crvs/*-3dcurve-*-tangents*dat | xargs echo']);

%    dmy_files = dir([mypath,'crvs/']);
%    if(size(dmy_files,1)<=2)
%        continue;
%    end

    myfiles;

    while length(myfiles) ~= 0
      [f,rem]=strtok(myfiles);
      myfiles = rem;
      if length(f) == 0
        break;
      end
      f = strip_trailing_blanks(f);

      r = myreadv(f);
      if isempty(r)
        warning(['file is empty: ' f]);
      else
        recs{1,end+1} = r;
      end
    end

    while length(myfiles_t) ~= 0
      [f_t,rem_t]=strtok(myfiles_t);
      myfiles_t = rem_t;
      if length(f_t) == 0
        break;
      end
      f_t = strip_trailing_blanks(f_t);

      r_t = myreadv(f_t);
      if isempty(r_t)
        warning(['file is empty: ' f_t]);
      else
        tangs{1,end+1} = r_t;
      end
    end
    
    
    %At this point, all curves are loaded into recs
    all_recs{v,1} = recs;
    all_nR(v,1) = size(recs,2);
    all_flags{v,1} = zeros(size(recs,2),1);
end

% rfabbri: these are basically the same code as above
% TODO: see if there is anything they add in the commented parts
%
% read_curve_sketch4;
% all_recs{11,1} = recs;
% all_nR(11,1) = size(recs,2);
% all_flags{11,1} = zeros(size(recs,2),1);
% clear recs;
% read_curve_sketch4a;
% all_recs{14,1} = recs;
% all_nR(14,1) = size(recs,2);
% all_flags{14,1} = zeros(size(recs,2),1);
% clear recs;
% read_curve_sketch4b;
% all_recs{7,1} = recs;
% all_nR(7,1) = size(recs,2);
% all_flags{7,1} = zeros(size(recs,2),1);
% clear recs;
% read_curve_sketch4c;
% all_recs{8,1} = recs;
% all_nR(8,1) = size(recs,2);
% all_flags{8,1} = zeros(size(recs,2),1);
% clear recs;
% read_curve_sketch4d;
% all_recs{9,1} = recs;
% all_nR(9,1) = size(recs,2);
% all_flags{9,1} = zeros(size(recs,2),1);
% clear recs;
