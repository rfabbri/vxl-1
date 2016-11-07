typedef enum {_VOID, _CHAR, _STRING, _SHORT, _INT, _FLOAT, _DOUBLE} OPT_TYPE;

typedef struct {
    char *flag_string;
    OPT_TYPE value_type;
    int *present_p;
    void *value_addr;
    char *comment;
} OPTTABLE;

extern void opt_parse_args (int argc, char *argv[], OPTTABLE *table);

extern int opt_usage (int argnum, int argc, char *argv[], OPTTABLE *tbl);
