function run_cseg(datasetpath, tdpath, bupath, outfolder, test_indices, bulevel_values, ov_thres_values)
        addpath ../combined
        for bulevel = bulevel_values
                for ov_thres = ov_thres_values
                        save_cseg_testset(datasetpath, tdpath, bupath, [outfolder '/cseg_' num2str(bulevel) '_' num2str(ov_thres)] , test_indices, bulevel, ov_thres);
                end
        end 
end


