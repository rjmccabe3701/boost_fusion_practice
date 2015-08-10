#include "fusion_includes.h"
using namespace boost::fusion;


template <typename U> struct AlwaysVoid {
    typedef void type;
};

#define NAME static constexpr const char* name

//struct StatEngine


template <typename T, typename R = int>
struct Repr
{
   NAME = T::name;
   using tag = T;
   using type = R;
   type storage;

};


template <typename T, typename Dummy = void>
struct GenEmbedded
{
   // using type = typename Repr<T>::type;
   using type = Repr<T>;

   static auto Generate()
   {
      // std::cout << "NO embedded, " << TypeId<T>{} << std::endl;
      return type{};
   }
};

template <typename T>
struct GenEmbedded<T, typename AlwaysVoid<typename T::EmbeddedTypes>::type>
{
   using Types = typename T::EmbeddedTypes;
   using child_type_vec = typename boost::mpl::transform<Types, GenEmbedded<boost::mpl::_>>::type;
   using type = Repr<T, child_type_vec>;
   // using type = typename boost::mpl::transform<Types, GenEmbedded<boost::mpl::_>>::type;

   static auto Generate()
   {
      std::cout << "HAS embedded, " << TypeId<T>{}
         << "\n\tTypes = " << TypeId<Types>{}
         << "\n\tmpl = "  << TypeId<type>{} << std::endl;
      // for_each(type{}, [](auto t) {GenEmbedded<decltype(t)>::Generate();});
      return type{};
   }
};

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

struct sndcf
{
   NAME = "SNDCF";
   struct IpDwnTag{
      NAME = "IP_DOWN";
   };
   struct IpUpTag{
      NAME = "IP_UP";
   };

   using EmbeddedTypes = vector<IpDwnTag, IpUpTag>;
};

struct sis_adapt
{
   NAME = "SIS_ADAPT";
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

      using EmbeddedTypes = vector<RxCountTag, CorrScoreTag, LinkIndTag>;
   };

   using EmbeddedTypes = vector<BlahTag, NodeStatsTag>;
};

struct etdrs_stats
{
   NAME = "TOP_LEVEL";
   using EmbeddedTypes = vector<sndcf, sis_adapt>;
   // using EmbeddedTypes = vector<sis_adapt::Stats>;
};

namespace my_detail
{
   using namespace boost;

   template <typename T>
   struct is_parent
   {
      //T is of type "struct Repr"
      //Then T::type should be either another seqeuence (if parent) or int
      static const bool value = mpl::is_sequence<typename T::type>::value;
   };

   template <typename Vec, typename Stat>
   struct tag_inserter
   {
      //T is of type "struct Repr"
      using tag = typename Stat::tag;
      using type = typename mpl::push_back<Vec, tag>::type;
   };

   template <typename Vec, typename Stat>
   struct repr_inserter
   {
      //T is of type "struct Repr"
      using type = typename mpl::push_back<Vec, Stat>::type;
   };

   template <typename T>
   struct get_seq
   {
      using type = typename T::type;
   };

   template <typename Vec, typename Stat>
   struct proxy_builder_helper
   {
         // _1 == the Vector
         // _2 == the Stat iterator
         using type = typename mpl::fold<
            Stat,
            Vec,
            mpl::eval_if<
               is_parent<mpl::_2>,
               proxy_builder_helper<mpl::_1, get_seq<mpl::_2>>,
               repr_inserter<mpl::_1, mpl::_2>
               >
            >::type;
   };

   template <typename Stat>
   struct lookup_proxy
   {
      using type = typename proxy_builder_helper<mpl::vector<>,
            typename Stat::type>::type;
   };

   template <typename Tag>
   struct matches_tag
   {
      template <typename Repr>
      struct apply
         : std::is_same<typename Repr::tag, Tag>::type
      {};
   };
}


int level = 0;
struct PrintStrings
{
   static void indent()
   {
      int i = level;
      while(i-- > 0)
         std::cout << "\t";
   }
   template <typename T>
   void operator()(T& t) const
   {
      print(t, typename traits::is_sequence<typename T::type>::type{});
   }

   template <typename T>
   static void print(T type, boost::mpl::true_)
   {
      indent();
      std::cout << "(PARENT) Type is = " << T::name << std::endl;
      level++;
      for_each(type.storage, PrintStrings{});
      level--;
   }

   template <typename T>
   static void print(T type, boost::mpl::false_)
   {
      indent();
      std::cout << "Type is = " << T::name
         << ", value = " << type.storage <<  std::endl;
   }
};

template <typename Tag, typename T>
auto& getValue(T& stats)
{
   return (*find_if<my_detail::matches_tag<Tag>>(stats)).storage;
}


int main(void)
{
   auto generated = GenEmbedded<etdrs_stats>::Generate();
   using GenStat = decltype(generated);
   std::cout << "The Type = " << TypeId<decltype(GenEmbedded<etdrs_stats>::Generate())>{} << std::endl;
   std::cout << "SIZE = " << sizeof(generated) << std::endl;

#if 1
   std::cout << "\n\n, all strings\n";
   PrintStrings::print(generated, boost::mpl::true_{});
#endif

   result_of::as_vector<my_detail::lookup_proxy<GenStat>::type>::type blah;

   std::cout << "Tags: = " << TypeId<decltype(blah)>{} << std::endl;

   getValue<sis_adapt::BlahTag>(blah) = 42;
   std::cout << getValue<sis_adapt::BlahTag>(blah) << std::endl;
}

