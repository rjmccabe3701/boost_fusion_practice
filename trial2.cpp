#include "fusion_includes.h"
using namespace boost::fusion;


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
   using type = typename Repr<T>::type;

   static auto Generate()
   {
      std::cout << "NO embedded, " << TypeId<T>{} << std::endl;
      return type{};
   }
};

template <typename T>
struct GenEmbedded<T, typename AlwaysVoid<typename T::EmbeddedTypes>::type>
{
   using Types = typename T::EmbeddedTypes;
   using type = typename boost::mpl::transform<Types, GenEmbedded<boost::mpl::_>>::type;

   static auto Generate()
   {
      std::cout << "HAS embedded, " << TypeId<T>{}
         << "\n\tTypes = " << TypeId<Types>{}
         << "\n\tmpl = "  << TypeId<type>{} << std::endl;
      for_each(type{}, [](auto t) {GenEmbedded<decltype(t)>::Generate();});
      return type{};
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

struct sndcf
{
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



int main(void)
{
   //GenEmbedded<FirstTag>::Generate();
   //GenEmbedded<SecondTag::Third>::Generate();
   GenEmbedded<SecondTag>::Generate();
   std::cout << "The Type = " << TypeId<decltype(GenEmbedded<SecondTag>::Generate())>{} << std::endl;
   std::cout << "\n**************\n";

   GenEmbedded<etdrs_stats>::Generate();
   std::cout << "The Type = " << TypeId<decltype(GenEmbedded<etdrs_stats>::Generate())>{} << std::endl;
}

