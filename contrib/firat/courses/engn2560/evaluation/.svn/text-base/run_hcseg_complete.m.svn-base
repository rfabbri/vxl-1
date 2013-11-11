function run_hcseg_complete(toppath, dpath, tdpath, bupath, seg_tree_out_folder, hcseg_outfolder, img_ext, saliency_measure, nbins_values, npad_values, ov_thres_values, test_indices)
        addpath ../bottom-up/hierarchy
        addpath ../combined
        datasetpath = [toppath '/' dpath];
        bu_folder = [toppath '/' bupath];
        seg_tree_out_folder = [toppath '/' seg_tree_out_folder];
        hcseg_outfolder = [toppath '/' hcseg_outfolder];
        tdpath = [toppath '/' tdpath];
        for nbins = nbins_values
                for npad = npad_values
                        fprintf('Begin segtree\n')
                        segtreepath = [seg_tree_out_folder '/segtree_' num2str(nbins) '_' num2str(npad)];
                        save_segtree_dataset(datasetpath, img_ext, test_indices, bu_folder, nbins, segtreepath, saliency_measure, npad);
                        fprintf('Begin hcseg\n')
                        for ov_thres = ov_thres_values
                                save_hcseg_testset(datasetpath, tdpath, segtreepath, [hcseg_outfolder '/hcseg_' num2str(nbins) '_' num2str(npad) '_' num2str(ov_thres)] , test_indices, ov_thres);
                        end  
                end
        end
end


