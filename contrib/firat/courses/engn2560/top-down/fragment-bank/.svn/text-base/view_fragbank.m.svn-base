function view_fragbank(fb, N, M)
        files = dir([fb '/*.mat']);
        fbsize = length(files);
        perpage = N*M;
        npages = ceil(fbsize / perpage);
        
        
        for p = 1:npages
                if p == npages
                        range = ((p-1)*perpage + 1):length(files);
                else
                        range = ((p-1)*perpage + 1):(p*perpage);
                end
                figure;
                for i = 1:length(range)
                        load([fb '/' files(range(i)).name]);
                        
                        subplot(N,M,i); imshow(padarray(gt_frag, [2 2]))              
                end
        end
end


