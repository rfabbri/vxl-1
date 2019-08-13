# v0 : full run (too long)
#parallel --xapply rf_all_pairs_experiment_perturb_ransac_sph_p3p.sh ::: 01 06 11 16 21 26 31 41 46 51 56 61 71 81 91 ::: 05 10 15 20 25 30 40 45 50 55 60 70 80 90 100
# v1 : 10 views if memory blows up (dataset viedws already random)
# v2 : 10 views, inside RANSAC set to 3, to test
parallel --xapply rf_all_pairs_experiment_perturb_ransac_sph.sh ::: 41 43 45 47 49 51 53 55 57 59 ::: 42 44 46 48 50 52 54 56 58 60
