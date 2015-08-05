#include <boost/fusion/view/iterator_range.hpp>
#include <boost/fusion/include/iterator_range.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/sequence/io/out.hpp>

#include <boost/fusion/include/define_struct.hpp>



#include <iostream>
#include <string>
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
