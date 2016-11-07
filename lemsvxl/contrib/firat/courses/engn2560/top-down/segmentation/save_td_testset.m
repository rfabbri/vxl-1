function save_td_testset(datasetpath, detectionpath, outfolder, test_indices, coeff, minscore)
        for id = test_indices
                try
                        fprintf('Processing image id: %d\n', id)
                        tdseg2(datasetpath, detectionpath, outfolder, id, coeff, minscore);
                        fprintf('Finished %d\n', id)
                catch ex
                        fprintf('Error Message: %s\n', ex.message);
                        continue
                end
        end
end
