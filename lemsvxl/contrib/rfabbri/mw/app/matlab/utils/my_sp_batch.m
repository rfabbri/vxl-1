%
% The input images are in a directory called images/ relative to the working
% directory. The output images will be placed in a directory called results/
%

[s,fnames] = unix('ls images/*|xargs echo');
fnames = strip_trailing_blanks(fnames);

while length(fnames) ~= 0
  [my_fname, rem] = strtok(fnames);
  fnames = rem;
  fnames = strip_trailing_blanks(fnames);

  my_sp;
end
