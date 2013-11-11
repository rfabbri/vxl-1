% intrinsic

fname = 'calib.intrinsic'

save(fname,'KK','-ascii','-double');

% extrinsic

%% for each image


for i=1:n_ima
  n_str = sprintf('%i',image_numbers(i));
  fname = [calib_name n_str '.extrinsic'];
  i_str = sprintf('%i',i);

  Rcurrent = eval(['Rc_' i_str]);

  if ~isnan(Rcurrent(1,1))

    ccenter= -Rcurrent'*eval(['Tc_' i_str]);
    ccenter= ccenter';

    save(fname,['Rc_' i_str],'-ascii','-double');
    save(fname,'ccenter','-ascii','-double','-append');

    disp(['Writing ' fname]);
  end
end
