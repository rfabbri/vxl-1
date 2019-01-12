# apply to all cmakelists.txt Wed Aug  8 22:29:13 -03 2018

s/add_test(/& NAME /g
s/\(NAME.*\)\(\b[a-zA-Z_1-9-][a-zA-Z_1-9-]*_test_all\)/\1COMMAND \$<TARGET_FILE:\2>/g

