#include "fusion_includes.h"
using namespace boost::fusion;


#if 0
template <typename U,
         typename C2 = C,
         typename Dummy = typename C2::Meow>
#endif
template <typename U> struct AlwaysVoid {
    typedef void type;
};

//struct StatEngine

template <typename T>
struct Repr
{
   using type = int;
};


template <typename T, typename Dummy = void>
struct GenEmbedded
{
   using ReprType = vector<typename Repr<T>::type>;

   static auto Generate()
   {
      std::cout << "NO embedded, " << TypeId<T>{} << std::endl;
      std::cout << "\tis ref = " << std::is_reference<T>::value << std::endl;
      return ReprType{};
   }
};

template <typename T>
struct GenEmbedded<T, typename AlwaysVoid<typename T::EmbeddedTypes>::type>
{
   using Types = typename T::EmbeddedTypes;
   using Head = std::remove_reference_t<typename result_of::at_c<Types,0>::type>;
   using Tail = typename result_of::pop_front<Types>::type;
   using HeadGen = GenEmbedded<Head>;
   using TailGen = GenEmbedded<Tail>;
   using ReprType = typename result_of::as_vector<
      typename result_of::join< typename HeadGen::ReprType, typename TailGen::ReprType>::type
      >::type;
   static ReprType Generate()
   {
      std::cout << "HAS embedded, " << TypeId<T>{} << std::endl;
      std::cout << "\t HEAD =  " << TypeId<Head>{} << std::endl;

      HeadGen::Generate();
      TailGen::Generate();

      return ReprType{};
   }
};


#if 1
struct FirstTag
{
};

struct SecondTag
{
   struct Third {
#if 1
      struct Deep {};
      struct Blah {};
      using EmbeddedTypes = vector<Deep, Blah>;
#endif

   };
   struct Forth {};
   using EmbeddedTypes = vector<Third, Forth>;
};
#endif


template <bool IsStatGen, typename Stats>
struct WfStat
{
#if 0
   using StatEngine = StatEngineT<IsStatGen, Stats>;

   WfStat(Stat& stat)
   {
      // We nee this actual copy of the original stat
      // structure because it holds the string information
      // the Stat type does not
   }

   //Need to using mpl/fusion magic to convert the
   // Stats fusion sequence into either a engine
   //  usable for stat generation (for the WF) or
   //  for stat observations (wfstat)
   StatEngine engine;
#endif

};


template <typename T>
struct ShmReprType
{
   using type = int;
};

#define NAME static constexpr const char* name

namespace sndcf
{
   struct IpDwnTag{
      NAME = "IP_DOWN";
   };
   struct IpUpTag{
      NAME = "IP_UP";
   };

   using Stats = vector<IpDwnTag, IpUpTag>;
}

namespace sis_adapt
{
   struct BlahTag{
      NAME = "BLAH";
   };
   struct NodeStatsTag{
      NAME = "NODE_STATS";
      struct RxCountTag{
         NAME = "RX_COUNTERS";
      };
      struct CorrScoreTag{
         NAME = "CORR_SCORE";
      };
      struct LinkIndTag{
         NAME = "LINK_IND";
      };

      using EmdeddedTypes = vector<RxCountTag, CorrScoreTag, LinkIndTag>;
   };

   using Stats = vector<BlahTag, NodeStatsTag>;
}



int main(void)
{
   GenEmbedded<FirstTag>::Generate();
   //GenEmbedded<SecondTag::Third>::Generate();
   GenEmbedded<SecondTag>::Generate();

   using WTF = SecondTag::EmbeddedTypes;
   using Head = typename result_of::at_c<WTF,0>::type;
   using Tail = typename result_of::pop_front<WTF>::type;
   using InnerHead = typename GenEmbedded<Head>::ReprType;
   using InnerTail = typename GenEmbedded<Tail>::ReprType;
   using ReprType = typename result_of::join<
         InnerHead,
         InnerTail
     >::type;

   std::cout << "assd " << TypeId<Head>{}
      // << "\n" << TypeId<result_of::deref(Tail)>{}
      << "\n" << TypeId<WTF>{}
      << "\n" << TypeId<result_of::as_vector<Tail>::type>{}
      << "\n" << TypeId<InnerHead>{}
      << "\n" << TypeId<InnerHead>{}
      << "\n" << TypeId<ReprType>{}
      << "\n" << TypeId<Tail>{} << std::endl;


   ShmReprType<int> shm;
   const auto& shmr = shm;
   std::cout << TypeId<decltype(shmr)>{} << std::endl;
}

#if 0
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
            std::cout << "Type NOT found for str = " << str << ", typeid = " << TypeId<T>{} << std::endl;
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
         std::cout << "Hello from " << TypeId<std::tuple_element_t<0,TheType>>{} << std::endl;
         std::cout << "\tchanging to = " << i << std::endl;
         std::get<0>(type_val).storage = i;
      };
   };


   for_each(typeVec, fill_func);

   auto print_func = [](auto type_val)
   {
      std::cout << "\t" << TypeId<std::get<0>(type_val)>{} << ": " << std::get<0>(type_val).storage << std::endl;
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

#endif
