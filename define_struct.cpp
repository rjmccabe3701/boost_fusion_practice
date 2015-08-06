#include "fusion_includes.h"

using namespace boost::fusion;


BOOST_FUSION_DEFINE_STRUCT(
      (demo), employee,
      (std::string, name)
      (int, age))

using namespace demo;
int main(void)
{

   employee rob{"rob", 32};

   rob.age = 11;

   std::cout << rob << std::endl;

   return 0;
}
