

// dbgl headers
#include <shape_align/dbskr_align_shapes.h>


#include <vcl_sstream.h>

int main( int argc, char *argv[] )
{

    vcl_stringstream stream_m(argv[1]);
    vcl_string input_model_file;
    stream_m>>input_model_file;

    vcl_stringstream stream_q(argv[2]);
    vcl_string input_query_file;
    stream_q>>input_query_file;

    dbskr_align_shapes matcher(input_model_file,
                               input_query_file);

    matcher.match();

    return 0;
}
