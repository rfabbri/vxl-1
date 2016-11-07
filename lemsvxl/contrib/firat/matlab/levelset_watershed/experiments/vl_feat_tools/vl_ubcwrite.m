function vl_ubcwrite(f, d, outfile, gzipped)
	num_keys = size(f, 2);
	len_desc = size(d, 1);
	if len_desc ~= 128
		error('NOT SIFT!!!');
	end 
	p=[1 2 3 4 5 6 7 8] ;
    q=[1 8 7 6 5 4 3 2] ;
	fid = fopen(outfile, 'w');
	fprintf(fid, '%d %d\n', num_keys, len_desc);
	for i = 1:num_keys
		f1 = f([2 1 3 4],i);
		f1(1:2) = f1(1:2)-1;
		f1(4) = -f1(4);
		fprintf(fid, '%.6g %.6g %.6g %.6g\n', f1);
		d1 = d(:,i);
		
		for j1=0:3
      		for i1=0:3
        		d1(8*(i1+4*j1)+q,:) = d1(8*(i1+4*j1)+p,:);
      		end
    	end
		
		for j = 1:6
			fprintf(fid, ' %d', d1((j-1)*20+1:j*20));
			fprintf(fid, '\n');
		end
		fprintf(fid, ' %d', d1(121:128));
		if i ~= num_keys
			fprintf(fid, '\n');
		end
	end	
	fclose(fid);
	if gzipped
		gzip(outfile);
		delete(outfile);
	end
end
