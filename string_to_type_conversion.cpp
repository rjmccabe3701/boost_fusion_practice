#include <boost/fusion/tuple.hpp>
#include <boost/fusion/view.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/type_traits.hpp>
#include <boost/fusion/support/is_sequence.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/begin.hpp>

#include <boost/fusion/include/find.hpp>
#include <boost/fusion/include/deref.hpp>
#include <boost/fusion/include/distance.hpp>
#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <type_traits>
#include <string>
#include <iostream>
#include <array>
#include <functional>
#include <vector>
#include <iterator>
#include <algorithm>

using namespace boost::fusion;


constexpr std::array<const char*, 3> typeStrings
{{
   "FOO_TYPE",
      "BAR_TYPE",
      "BAZ_TYPE"
 }};

std::array<std::function<void(void)>, 3> typeFunctions;

struct Foo{};
struct Bar{};
struct Baz{};

using TypeVec = vector< Foo, Bar, Baz>;
using Begin = result_of::begin<TypeVec>::type;

TypeVec typeVec;

void stringToTypeConversion()
{
   //Fills the typeFunctions array with appropriate callbacks (per type)
   auto fill_func = [&](auto type_val)
   {
      using TheType = decltype(type_val);
      using IterType = typename result_of::find<TypeVec, TheType>::type;
      constexpr auto type_idx = result_of::distance<Begin, IterType>::type::value;
      static_assert(type_idx < result_of::size<TypeVec>::type::value, "BAD TYPE");

      typeFunctions[type_idx] = []()
      {
         std::cout << "Hello from " << typeid(TheType).name() << std::endl;
      };
   };

   for_each(typeVec, fill_func);

   std::vector<std::string> queryStrings = {{
      "BAZ_TYPE",
      "FOO_TYPE",
      "FOO_TYPE",
      "FOO_TYPE",
      "BAD_TYPE",
   }};

   for(auto& qs: queryStrings)
   {
      auto iter = std::find_if(typeStrings.begin(), typeStrings.end(), [&](const auto& s)
            {
               return qs == s;
            });

      if(iter != typeStrings.end())
      {
         auto pos = std::distance(typeStrings.begin(), iter);
         typeFunctions[pos]();
      }
      else
      {
         std::cout << qs << " not found!\n";
      }
   }
   /* Prints:
    Hello from 3Baz
    Hello from 3Foo
    Hello from 3Foo
    Hello from 3Foo
    BAD_TYPE not found!
    */
}

int fun_with_iterators(void)
{
 //  using Begin = result_of::begin<TypeVec>::type;
   //--- found in TypeVec
   using IterTypeFound = result_of::find<TypeVec, Foo>::type;
   std::cout << "Type of IterType (Foo) = " << typeid(IterTypeFound).name()
      << "\n\tdistance = " << result_of::distance<Begin, IterTypeFound>::type::value <<std::endl;

   using DerefTypeFound = std::remove_reference_t<result_of::deref<IterTypeFound>::type>;
   std::cout << "Type of DerefType (Foo) = " << typeid(DerefTypeFound).name() << std::endl;

   //--- not found in TypeVec
   using IterTypeNotFound = result_of::find<TypeVec, int>::type;
   std::cout << "Type of IterType (not found) = " << typeid(IterTypeNotFound).name()
      << "\n\tdistance = " << result_of::distance<Begin, IterTypeNotFound>::type::value <<std::endl;

   using DerefTypeNotFound = std::remove_reference_t<result_of::deref<IterTypeNotFound>::type>;
   std::cout << "Type of DerefType (not found) = " << typeid(DerefTypeNotFound).name() << std::endl;

   /* Prints:

   Type of IterType (Foo) = N5boost6fusion15vector_iteratorINS0_6vectorI3Foo3Bar3BazNS0_5void_ES6_S6_S6_S6_S6_S6_EELi0EEE
           distance = 0
   Type of DerefType (Foo) = 3Foo
   Type of IterType (not found) = N5boost6fusion15vector_iteratorINS0_6vectorI3Foo3Bar3BazNS0_5void_ES6_S6_S6_S6_S6_S6_EELi3EEE
           distance = 3
   Type of DerefType (not found) = N4mpl_5void_E
   */

}


int main(void)
{
   fun_with_iterators();
   stringToTypeConversion();

   return 0;
}

