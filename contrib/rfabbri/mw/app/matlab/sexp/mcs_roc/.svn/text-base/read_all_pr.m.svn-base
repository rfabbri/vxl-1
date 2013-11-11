%clear all;

% Call this from the evaluation directory


[ret,fail_str]=unix('grep non-zero\ status output');
fail_str = strip_trailing_blanks(fail_str);

if length(fail_str) ~= 0
  disp('ERROR: invalid tests in this directory!');
  fail_str
  return
end


[ret,allfiles]=unix('ls stats*|xargs echo');

if length(allfiles) == 0
  error('no test files in pwd.');
  return
end

allfiles_orig = allfiles;
i=1;
while length(allfiles) ~=0
  disp(['Reading stats file #' num2str(i)]);
  [fname,rem]=strtok(allfiles);
  allfiles = strip_trailing_blanks(rem);
  fname= strip_trailing_blanks(fname);
  fnames{i} = fname;

  [prec, rec, tree] = read_pr(fname);
  precision{i} = prec;
  recall{i} = rec;

  i = i + 1;
end
save('pr.mat','precision','recall','tree','fnames');

