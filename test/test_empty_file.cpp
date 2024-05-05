#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {


CASE_GROUP(EMPTY_FILE)
{

ADD_CASE_TO_GROUP("empty0-nochars",
"",
NOTYPE);


ADD_CASE_TO_GROUP("empty0-multiline", R"(


)", NOTYPE);


ADD_CASE_TO_GROUP("empty0-multiline with spaces", R"(

    
     
      


)", NOTYPE);


ADD_CASE_TO_GROUP("empty0-multiline with spaces and tabs", R"(

    
   		  
      
		


)", NOTYPE);


ADD_CASE_TO_GROUP("empty0-multiline-with-comments 0", R"(
# well hello sir, I see you are fine
# very fine thank you
# send my very best wishes
)", NOTYPE);

ADD_CASE_TO_GROUP("empty0-multiline-with-comments 1", R"(



# well hello sir, I see you are fine
# very fine thank you
# send my very best wishes



)", NOTYPE);

ADD_CASE_TO_GROUP("empty0-multiline-with-comments 2", R"(

      
    		

# well hello sir, I see you are fine
# very fine thank you
# send my very best wishes


    		
      


)", NOTYPE);

}

} // namespace yml
} // namespace c4
