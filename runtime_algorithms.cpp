#include "fusion_includes.h"
#include <tuple>
using namespace boost::fusion;


std::array<std::function<void(int)>, 3> typeFunctions;

struct Base
{
   int storage;
};

struct Foo: Base{};
struct Bar: Base{};
struct Baz: Base{};

auto typeVec = make_vector(
      std::make_tuple(Foo{}, "FOO_TYPE"),
      std::make_tuple(Bar{}, "BAR_TYPE"),
      std::make_tuple(Baz{}, "BAZ_TYPE")
      );

using TypeVec = decltype(typeVec);
using Begin = result_of::begin<TypeVec>::type;

struct SearchForString
{
    mutable int str_loc = -1;
    mutable std::string str = "N/A";

   template<typename T>
      void operator()(T& t) const
      {
         if(str == std::get<1>(t))
         {
            using IterType = typename result_of::find<TypeVec, T>::type;
            constexpr auto type_idx = result_of::distance<Begin, IterType>::type::value;
            str_loc = type_idx;
            std::cout << "Type found for str = " << str << ", index = " << type_idx << std::endl;
         }
         else
         {
            std::cout << "Type NOT found for str = " << str << ", typeid = " << typeid(T).name() << std::endl;
         }
      }
};

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
         std::cout << "Hello from " << typeid(std::tuple_element_t<0,TheType>).name() << std::endl;
         std::cout << "\tchanging to = " << i << std::endl;
         std::get<0>(type_val).storage = i;
      };
   };


   for_each(typeVec, fill_func);

   auto print_func = [](auto type_val)
   {
       std::cout << "\t" << typeid(std::get<0>(type_val)).name() << ": " << std::get<0>(type_val).storage << std::endl;
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
      SearchForString stringSearcher;
      stringSearcher.str = qs;

      for_each(typeVec, stringSearcher);
      if(stringSearcher.str_loc >= 0)
      {
         std::cout << qs << " FOUND! location = " << stringSearcher.str_loc << "\n";
         typeFunctions[stringSearcher.str_loc](++i);
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
   Type NOT found for str = BAZ_TYPE, typeid = St5tupleII3FooPKcEE
   Type NOT found for str = BAZ_TYPE, typeid = St5tupleII3BarPKcEE
   Type found for str = BAZ_TYPE, index = 2
   BAZ_TYPE FOUND! location = 2
   Hello from 3Baz
           changing to = 1
   Type found for str = FOO_TYPE, index = 0
   Type NOT found for str = FOO_TYPE, typeid = St5tupleII3BarPKcEE
   Type NOT found for str = FOO_TYPE, typeid = St5tupleII3BazPKcEE
   FOO_TYPE FOUND! location = 0
   Hello from 3Foo
           changing to = 2
   Type NOT found for str = BAR_TYPE, typeid = St5tupleII3FooPKcEE
   Type found for str = BAR_TYPE, index = 1
   Type NOT found for str = BAR_TYPE, typeid = St5tupleII3BazPKcEE
   BAR_TYPE FOUND! location = 1
   Hello from 3Bar
           changing to = 3
   Type found for str = FOO_TYPE, index = 0
   Type NOT found for str = FOO_TYPE, typeid = St5tupleII3BarPKcEE
   Type NOT found for str = FOO_TYPE, typeid = St5tupleII3BazPKcEE
   FOO_TYPE FOUND! location = 0
   Hello from 3Foo
           changing to = 4
   Type NOT found for str = BAD_TYPE, typeid = St5tupleII3FooPKcEE
   Type NOT found for str = BAD_TYPE, typeid = St5tupleII3BarPKcEE
   Type NOT found for str = BAD_TYPE, typeid = St5tupleII3BazPKcEE
   BAD_TYPE not found!
   typeVec after change:
           3Foo: 4
           3Bar: 3
           3Baz: 1
   */
}

int main(void)
{
   stringToTypeConversion();
   return 0;
}

