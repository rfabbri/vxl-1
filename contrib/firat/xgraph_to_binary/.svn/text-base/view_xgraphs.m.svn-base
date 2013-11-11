function view_xgraphs(xfolder, M, N)
    files = dir([xfolder '/*.xml']);
    figure;
    for i = 1:length(files)
        binary_mask = dbsksp_convert_xgraph_to_binary_mask([xfolder '/' files(i).name], '', 1);
        subplot(M,N,i); imagesc(binary_mask); colormap gray; axis off; axis image
    end
end
