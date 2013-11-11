function run_hcseg(datasetpath, tdpath, segtree_path, outfolder, test_indices, ov_thres_values)
        addpath ../combined
        for ov_thres = ov_thres_values
                save_hcseg_testset(datasetpath, tdpath, segtree_path, [outfolder '/hcseg_' num2str(ov_thres)] , test_indices, ov_thres);
        end 
end


