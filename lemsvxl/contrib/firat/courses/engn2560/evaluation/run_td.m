function run_td(datasetpath, detectionpath, outfolder, test_indices, coeff_values, minscore_values)
        addpath ../top-down/segmentation
        for coeff = coeff_values
                for minscore = minscore_values
                        save_td_testset(datasetpath, detectionpath, [outfolder '/td_' num2str(coeff) '_' num2str(minscore)] , test_indices, coeff, minscore);
                end
        end 
end


