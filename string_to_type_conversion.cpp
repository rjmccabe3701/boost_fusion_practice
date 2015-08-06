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
#include <ostream>
#include <array>
#include <functional>
#include <vector>
#include <iterator>
#include <algorithm>
#include <ostream>

using namespace boost::fusion;


constexpr std::array<const char*, 3> typeStrings
{{
   "FOO_TYPE",
   "BAR_TYPE",
   "BAZ_TYPE",
}};

std::array<std::function<void(int)>, 3> typeFunctions;

struct Base
{
   int storage;
};

std::ostream& operator<<(std::ostream& out, Base& b)
{
   out << b.storage;
   return out;
}

struct Foo: Base{};
struct Bar: Base{};
struct Baz: Base{};

using TypeVec = vector< Foo, Bar, Baz>;
using Begin = result_of::begin<TypeVec>::type;

TypeVec typeVec;

void stringToTypeConversion()
{
   //Fills the typeFunctions array with appropriate callbacks (per type)
   auto fill_func = [&](auto& type_val)
   {
      using TheType = std::remove_reference_t<decltype(type_val)>;
      using IterType = typename result_of::find<TypeVec, TheType>::type;
      constexpr auto type_idx = result_of::distance<Begin, IterType>::type::value;
      static_assert(type_idx < result_of::size<TypeVec>::type::value, "BAD TYPE");

      typeFunctions[type_idx] = [&type_val](int i)
      {
         std::cout << "Hello from " << typeid(TheType).name() << std::endl;
         std::cout << "\tchanging to = " << i << std::endl;
         type_val.storage = i;
      };

   };

   for_each(typeVec, fill_func);

   auto print_func = [](auto type_val)
   {
      std::cout << "\t" << typeid(type_val).name() << ": " << type_val.storage << std::endl;
   };
   std::cout << "typeVec before change:\n ";
   for_each(typeVec, print_func);

   std::vector<std::string> queryStrings = {{
      "BAZ_TYPE",
      "FOO_TYPE",
      "BAR_TYPE",
      "FOO_TYPE",
      "BAD_TYPE",
   }};

   int i = 0;
   for(auto& qs: queryStrings)
   {
      auto iter = std::find_if(typeStrings.begin(), typeStrings.end(), [&](const auto& s)
            {
               return qs == s;
            });

      if(iter != typeStrings.end())
      {
         auto pos = std::distance(typeStrings.begin(), iter);
         typeFunctions[pos](++i);
      }
      else
      {
         std::cout << qs << " not found!\n";
      }
   }
   std::cout << "typeVec after change:\n ";
   for_each(typeVec, print_func);
   /* Prints:
   typeVec before change:
           3Foo: 0
           3Bar: 0
           3Baz: 0
   Hello from 3Baz
           changing to = 1
   Hello from 3Foo
           changing to = 2
   Hello from 3Bar
           changing to = 3
   Hello from 3Foo
           changing to = 4
   BAD_TYPE not found!
   typeVec after change:
           3Foo: 4
           3Bar: 3
           3Baz: 1
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

