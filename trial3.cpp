#include "fusion_includes.h"
using namespace boost::fusion;

template <typename U> struct AlwaysVoid {
    typedef void type;
};

#define NAME static constexpr const char* name

template <typename T, typename Tags, typename R = int>
struct Repr
{
   using MatchingTags = Tags;
   using type = R;
   type storage;
};


#if 1
namespace my_detail
{
   using namespace boost;

   template <typename T>
      struct is_parent
      {
         //T is of type "struct TagString"
         //Then T::child_list should be either another sequence (if parent) or int
         static const bool value = mpl::is_sequence<typename T::child_list>::value;
      };

   template <typename GlobalTagVec, typename ParentVec, typename ThisStat>
      struct repr_inserter
      {
         //ThisStat is of type "struct TagString"
         using ThisTag = typename ThisStat::tag;
         //Add the entire lineage of parents to the matching tags for
         // this statistic
         using MatchingTags =
            typename mpl::push_back<ParentVec,ThisTag>::type;
         using this_stat = Repr<ThisTag, MatchingTags>;
         //Finally add this Repr to the global tag vec
         using type = typename mpl::push_back<GlobalTagVec, this_stat>::type;
      };

   template <typename GlobalTagVec, typename ParentVec, typename TagHierarchy>
      struct stat_creator_helper
      {

         using ThisLineage = typename mpl::push_back<ParentVec,
               typename TagHierarchy::tag>::type;
         using ChildTagHierarchy = typename TagHierarchy::child_list;
         // _1 == GlobalTagVec
         // _2 == the iterator to the child TagString
         using type = typename mpl::fold<
            ChildTagHierarchy,
            GlobalTagVec,
            mpl::eval_if<
               is_parent<mpl::_2>,
               stat_creator_helper<mpl::_1, ThisLineage, mpl::_2>,
               repr_inserter<mpl::_1, ThisLineage, mpl::_2>
            >>::type;
      };

   template <typename TagHierarchy>
      struct stat_creator
      {
         using type = typename stat_creator_helper<
               mpl::vector<>, //Global Tag Vector
               mpl::vector<>, //Parent vector
               TagHierarchy>::type;
      };

   template <typename TagVec, typename Tag>
      struct contains_tag
      {
         using type = typename mpl::contains<TagVec, Tag>::type;
      };

   template <typename Tag>
      struct matches_tag
      {
         template <typename Repr>
            struct apply
            : contains_tag<typename Repr::MatchingTags, Tag>::type
            {};
      };
}
#endif

template <typename T, typename P, typename C = void>
struct TagString
{
   NAME = T::name;
   using tag = T;
   using parent = P;
   using child_list = C;
};

template <typename T, typename Parent, typename Dummy = void>
struct GenTagHierarchy
{
   using type = TagString<T, Parent>;
};

template <typename T, typename Parent>
struct GenTagHierarchy<T, Parent, typename AlwaysVoid<typename T::ChildTypes>::type>
{
   using ChildTypes = typename T::ChildTypes;
   using child_type_vec = typename boost::mpl::transform<
      ChildTypes, GenTagHierarchy<boost::mpl::_, T>>::type;
   using type = TagString<T, Parent, child_type_vec>;
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

   using ChildTypes = vector<IpDwnTag, IpUpTag>;
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

      using ChildTypes = vector<RxCountTag, CorrScoreTag, LinkIndTag>;
   };

   using ChildTypes = vector<BlahTag, NodeStatsTag>;
};

struct etdrs_stats
{
   NAME = "TOP_LEVEL";
   using ChildTypes = vector<sndcf, sis_adapt>;
};


int level = 0;

#if 1
template<typename Tag, typename T>
auto getStatHandleView(T& stats)
{
   return filter_view<T, my_detail::matches_tag<Tag>>(stats);
}

template <typename Tag, typename T>
auto& getValue(T& stats)
{
   auto statHdlView = getStatHandleView<Tag>(stats);
   static_assert(result_of::size<decltype(statHdlView)>::value == 1,
         "getValues requires a Leaf Tag!");
   return deref(begin(statHdlView)).storage;
}
#endif


struct TopName
{
   NAME = "TOP";
};

using Top = TagString<TopName, void>;

using TagHierarchy = GenTagHierarchy<etdrs_stats, Top>::type;

using TheStats = result_of::as_vector<
   my_detail::stat_creator<TagHierarchy>::type>::type;
TheStats theStats;

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
      print(t, typename traits::is_sequence<typename T::child_list>::type{});
   }

   template <typename T>
   static void print(T type, boost::mpl::true_)
   {
      indent();
      using Parent = typename T::parent;
      std::cout << "(PARENT) Type is = " << T::name
         << ", parent is " << Parent::name << std::endl;
      level++;
      using ChildList = typename T::child_list;
      for_each(ChildList{}, PrintStrings{});
      level--;
   }

   template <typename T>
   static void print(T type, boost::mpl::false_)
   {
      indent();
      using Parent = typename T::parent;
      using Tag = typename T::tag;
      std::cout << "Type is = " << T::name
            << ", parent is " << Parent::name
            << ", value is " << getValue<Tag>(theStats)
            << ", addr is " << std::hex << &getValue<Tag>(theStats)
            << std::endl;
   }
};

int main(void)
{
   std::cout << "The Type = " << TypeId<TagHierarchy>{} << std::endl;

#if 1
   std::cout << "\n\n, all strings\n";
   PrintStrings::print(TagHierarchy{}, boost::mpl::true_{});
#endif

   std::cout << "Tags: = " << TypeId<TheStats>{} << std::endl;
   getValue<sis_adapt::BlahTag>(theStats) = 42;
   std::cout <<  getValue<sis_adapt::BlahTag>(theStats) << std::endl;

   PrintStrings::print(TagHierarchy{}, boost::mpl::true_{});

/* Will print:
(PARENT) Type is = TOP_LEVEL, parent is TOP
        (PARENT) Type is = SNDCF, parent is TOP_LEVEL
                Type is = IP_DOWN, parent is SNDCF, value is 0, addr is 0x607280
                Type is = IP_UP, parent is SNDCF, value is 0, addr is 0x607284
        (PARENT) Type is = SIS_ADAPT, parent is TOP_LEVEL
                Type is = BLAH, parent is SIS_ADAPT, value is 2a, addr is 0x607288
                (PARENT) Type is = NODE_STATS, parent is SIS_ADAPT
                        Type is = RX_COUNTERS, parent is NODE_STATS, value is 0, addr is 0x60728c
                        Type is = CORR_SCORE, parent is NODE_STATS, value is 0, addr is 0x607290
                        Type is = LINK_IND, parent is NODE_STATS, value is 0, addr is 0x607294

*/

}

