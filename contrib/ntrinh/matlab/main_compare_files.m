edgemap_file1 = 'V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_edgemaps-clean_using_kovesi-I_15-len_10/giraffes_dragon_edges.tif'
edgeorient_file1 = 'V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_edgeorients-txt-clean_using_kovesi-I_15-len_10/giraffes_dragon_orient.txt'
cemv_file1 = 'V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_cemv-using_kovesi-I_15-len_10/giraffes_dragon.cemv'

edgemap_file2 = 'D:/vision/data/ETHZ-shape/all_edgemaps-clean_using_kovesi-I_15-len_10/giraffes_dragon_edges.tif'
edgeorient_file2 = 'D:/vision/data/ETHZ-shape/all_edgeorients-txt-clean_using_kovesi-I_15-len_10/giraffes_dragon_orient.txt'
cemv_file2 = 'D:/vision/data/ETHZ-shape/all_cemv-using_kovesi-I_15-len_10/giraffes_dragon.cemv'

edgemap1 = imread(edgemap_file1);
edgemap2 = imread(edgemap_file2);

edgemap_diff = double(edgemap1) - double(edgemap2);
fprintf(1, 'sum edgemap diff = %f\n', sum(sum(abs(edgemap_diff))));

edgeorient1 = dlmread(edgeorient_file1, ' ');
edgeorient2 = dlmread(edgeorient_file2, ' ');
edgeorient_diff = edgeorient1 - edgeorient2;
fprintf(1, 'sum edgeorient diff = %f\n', sum(sum(abs(edgeorient_diff))));

