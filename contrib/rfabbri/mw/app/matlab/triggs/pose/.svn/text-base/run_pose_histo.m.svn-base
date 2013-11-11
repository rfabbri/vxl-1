% histograms for pose errors
nbins = 50;
% TMP;
if 0
   N = 100000;
   in = fopen('TMP1','r');
   med_R = zeros(1,N);
   for i=1:N, med_R(i) = fscanf(in,'%g','C'); end
   med_t = zeros(1,N);
   for i=1:N, med_t(i) = fscanf(in,'%g','C'); end
   med_d = zeros(1,N);
   for i=1:N, med_d(i) = fscanf(in,'%g','C'); end
   fclose(in);
end
if 0
   x = med_R;
   fprintf(stdout,'med_R\n');
   bins = logspace(log10(1e-2),log10(2e2),nbins);
   [h,v,s] = histo(x,bins);
   for i=1:nbins+1, fprintf(stdout,'%12g\t%12g\n',v(i),h(i)/s(i)/size(x,2)); end
end
if 0
   x = med_t;
   fprintf(stdout,'med_t\n');
   bins = logspace(log10(1e-2),log10(1e4),nbins);
   [h,v,s] = histo(x,bins);
   for i=1:nbins+1, fprintf(stdout,'%12g\t%12g\n',v(i),h(i)/s(i)/size(x,2)); end
end
if 1
   x = med_d;
   fprintf(stdout,'med_d\n');
   bins = logspace(log10(1e-2),log10(1e2),nbins);
   [h,v,s] = histo(x,bins);
   for i=1:nbins+1, fprintf(stdout,'%12g\t%12g\n',v(i),h(i)/s(i)/size(x,2)); end
end