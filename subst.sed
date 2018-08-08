# multiple commands in each line is one on top of the output of the other:
s/add_test(/& NAME /g
s/\(NAME.*\)\(\b[a-zA-Z_1-9-][a-zA-Z_1-9-]*_test_all\)/\1COMMAND \$<TARGET_FILE:\2>/g
