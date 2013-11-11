//---------------------------------------------------------------------
// This is brcv/rec/dbasn/tests/dbasn_test_artificial_def.cxx
//:

//Test graph database.
float n_equal[] = {1, 1, 1};
float n_ident[] = {1, 2, 3};
float n_scale[] = {2, 4, 6};
float n_modi1[] = {1, 2, 4};
float n_modi2[] = {1, 2.5, 3.5};
float n_modi3[] = {1.3, 2.2, 4.5};

float n_rotat[] = {3, 1, 2};

float n_flip_id[] = {1, 3, 2};
float n_flip_m1[] = {1, 4, 2};
float n_flip_m2[] = {1, 3.5, 2.5};
float n_flip_m3[] = {1.3, 4.5, 2.2};

float n_pertb[] = {1.1, 1.2, 1.3};
float n_pertb2[] = {1.2, 1.1, 1.3};
float n_pert_rot[] = {1.3, 1.1, 1.2};

char n_types[] = {'3', '3', '4'};

float l_zeros[] = {0, 0, 0};
float l_equal[] = {1, 1, 1};
float l_ident[] = {1, 2, 3}; //{1, 2, 2.08}; //
float l_scale[] = {2, 4, 6};
float l_modi1[] = {1, 2, 4};
float l_modi2[] = {1, 2.5, 3.5};
float l_modi3[] = {1.3, 2.2, 4.5};

float l_rotat[] = {3, 1, 2};

float l_flip_id[] = {1, 3, 2}; //{1, 2.08, 2}; //
//float l_flip_m1[] = {1, 2, 4};
//float l_flip_m2[] = {1, 2.5, 3.5};
//float l_flip_m3[] = {1.3, 2.2, 4.5};

float l_pertb[] = {1.1, 1.2, 1.3};
float l_pertb2[] = {1.2, 1.1, 1.3};
float l_pert_rot[] = {1.3, 1.1, 1.2};

char l_types[] = {'R', 'A', 'R'};

float c_zeros[] = {0, 0, 0};
float c_equal[] = {1, 1, 1};
float c_ident[] = {1, 2, 3};
float c_scale[] = {2, 4, 6};
float c_modi1[] = {1, 2, 4};
float c_modi2[] = {1, 2.5, 3.5};
float c_modi3[] = {1.3, 2.2, 4.5};

float c_rotat[] = {3, 1, 2};

float c_pertb[] = {1.1, 1.2, 1.3};
float c_pertb2[] = {1.2, 1.1, 1.3};
float c_pert_rot[] = {1.3, 1.1, 1.2};

//ground truth for the standard cases.
int labelgG[] = {0, 1, 2}; //labelgG [g's node id] = G's node id.

//ground truth for the rotated case.
int labelg_rotat[] = {2, 0, 1};

//ground truth for the flip2 case.
int labelg_flip2[] = {1, 0, 2};

//##################################################################
// Defining Hypergraphs (Corners)

int l_nodes[][2] = {{0, 1}, {1, 2}, {0, 2}, {0, 3}, {0, 4}, {1, 5}, {1, 6}, {2, 7}, {2, 8},
                    {3, 5}, {4, 6}, {5, 7}, {6, 8}, {3, 7}, {4, 8}, {3, 4}, {5, 6}, {7, 8}};

int c_nodes[][3] = {{1, 0, 2}, {3, 0, 1}, {3, 0, 2}, {4, 0, 1}, {4, 0, 2}, {3, 0, 4},
                    {0, 1, 2}, {5, 1, 0}, {5, 1, 2}, {6, 1, 0}, {6, 1, 2}, {5, 1, 6},
                    {0, 2, 1}, {7, 2, 0}, {7, 2, 1}, {8, 2, 0}, {8, 2, 1}, {7, 2, 8},
                    {7, 3, 0}, {5, 3, 0}, {4, 3, 0}, 
                    {3, 4, 0}, {6, 4, 0}, {8, 4, 0}, 
                    {3, 5, 1}, {7, 5, 1}, {6, 5, 1},
                    {4, 6, 1}, {5, 6, 1}, {8, 6, 1},
                    {3, 7, 2}, {5, 7, 2}, {8, 7, 2},
                    {7, 8, 2}, {6, 8, 2}, {4, 8, 2}};

int c_nodes2[][3] = {{1, 0, 2}, {3, 0, 1}, {4, 0, 1}, {4, 0, 2}, {3, 0, 4},
                     {0, 1, 2}, {5, 1, 0}, {5, 1, 2}, {6, 1, 0}, {5, 1, 6},
                     {0, 2, 1}, {7, 2, 1}, {8, 2, 0}, {7, 2, 8},
                     {5, 3, 0}, {4, 3, 0}, 
                     {3, 4, 0}, {6, 4, 0}, {8, 4, 0}, 
                     {3, 5, 1}, {7, 5, 1}, {6, 5, 1},
                     {4, 6, 1}, {5, 6, 1},
                     {5, 7, 2}, {8, 7, 2},
                     {7, 8, 2}, {4, 8, 2}};

//ground truth for the standard cases.
int str_labelgG[] = {0, 1, 2, 3, 4, 5, 6, 7, 8}; //labelgG [g's node id] = G's node id.

