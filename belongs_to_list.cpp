#include "fusion_includes.h"

#define USE_CONS

struct Foo{};
struct Bar{};
struct Baz{};
struct NotFound{};


using namespace boost::fusion;

using MyList =
#ifdef USE_CONS
cons<Foo,
   cons<Bar,
         cons<Baz>
       >
    >
#else
list<Foo, Bar, Baz>;
#endif
;

template <typename Tag, typename List>
struct BelongsToList
{
   using Iter = typename result_of::find<List, Tag>::type;
   static constexpr bool value = !std::is_same<
         Iter,
         typename result_of::end<List>::type
      >::value;
};


int main(void)
{
   std::cout << "MyList = " << TypeId<MyList>{} << std::endl;
   using FooB = BelongsToList<Foo, MyList>;
   std::cout << "Iter(Foo) = " << TypeId<FooB::Iter>{}
      << ", found = " << FooB::value <<  std::endl;
   using NotFoundB = BelongsToList<NotFound, MyList>;
   std::cout << "Iter(NotFound) = " << TypeId<NotFoundB::Iter>{}
      << ", found = " << NotFoundB::value <<  std::endl;
}
