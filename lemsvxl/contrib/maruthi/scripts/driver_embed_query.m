clear all
close all

%same for all query sigmas
exemp_data='/home/mn/research/proximity_graphs/synthetic_cluster_nn_test/exemplar_dataset_scg3d.txt';
exemp_labels='/home/mn/research/proximity_graphs/synthetic_cluster_nn_test/exemplar_scg3d.txt';

%query data
query_sqrt3_data='/home/mn/research/proximity_graphs/synthetic_cluster_nn_test/query_dataset_scg3d_sqrt3.txt';
query_sqrt4_data='/home/mn/research/proximity_graphs/synthetic_cluster_nn_test/query_dataset_scg3d_sqrt4.txt';
query_sqrt9_data='/home/mn/research/proximity_graphs/synthetic_cluster_nn_test/query_dataset_scg3d_sqrt9.txt';
query_sqrt25_data='/home/mn/research/proximity_graphs/synthetic_cluster_nn_test/query_dataset_scg3d_sqrt25.txt';

%results
gg_sqrt3_stats='/home/mn/research/proximity_graphs/synthetic_cluster_nn_test/stats/query1/gg_graph_scg3d_fnn_stats.txt';
gg_sqrt4_stats='/home/mn/research/proximity_graphs/synthetic_cluster_nn_test/stats/query2/gg_graph_scg3d_fnn_stats.txt';
gg_sqrt9_stats='/home/mn/research/proximity_graphs/synthetic_cluster_nn_test/stats/query3/gg_graph_scg3d_fnn_stats.txt';
gg_sqrt25_stats='/home/mn/research/proximity_graphs/synthetic_cluster_nn_test/stats/query4/gg_graph_scg3d_fnn_stats.txt';

%Read in exemp data
exemplar_matrix=textread(exemp_data);
exemp_labels=textread(exemp_labels,'%s');
nn=2;

%sqrt of 3
query_matrix=textread(query_sqrt3_data);
[pg_stats_sqrt3,nn_stats_sqrt3] = embed_query(exemplar_matrix,query_matrix,exemp_labels,gg_sqrt3_stats,nn);

%sqrt of 4
query_matrix=textread(query_sqrt4_data);
[pg_stats_sqrt4,nn_stats_sqrt4] = embed_query(exemplar_matrix,query_matrix,exemp_labels,gg_sqrt4_stats,nn);

%sqrt of 9
query_matrix=textread(query_sqrt9_data);
[pg_stats_sqrt9,nn_stats_sqrt9] = embed_query(exemplar_matrix,query_matrix,exemp_labels,gg_sqrt9_stats,nn);

%sqrt of 25
query_matrix=textread(query_sqrt25_data);
[pg_stats_sqrt25,nn_stats_sqrt25] = embed_query(exemplar_matrix,query_matrix,exemp_labels,gg_sqrt25_stats,nn);