#include <biob/biob_grid_worldpt_roster.h>
#include <vcl_vector.h>
#include <biob/biob_worldpt_roster_sptr.h>


/*: Populates the given vector which_sample:
 For each integer i (0 <= i <= output_roster.num_points()), 
 which_sample[i] is the worldpt_index in sample_roster that is closest to point i of output_roster
*/

void splr_roster_to_grid_mapping(const biob_worldpt_roster_sptr sample_roster, const biob_grid_worldpt_roster & grid_roster,   vcl_vector<biob_worldpt_index>  & which_sample);
