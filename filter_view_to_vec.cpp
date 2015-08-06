#include "fusion_includes"

using namespace boost::fusion;

struct print
{
   template <typename T>
   void operator()(const T& t) const
   {
      std::cout << std::boolalpha << t << '\n';
   }
};

template <int N>
struct TupleMaker
{
   using tuple_type = tuple<int, tuple<typename TupleMaker<N-1>::tuple_type>>;
};

template <>
struct TupleMaker<1>
{
   using tuple_type = int;
};

void demonstrateTupleMaker()
{
   using myTupleType = TupleMaker<4>::tuple_type;
   myTupleType foo;//{1,2,3,4};
   std::cout << "Is foo a seq = " << traits::is_sequence<myTupleType>::value << std::endl;
   auto flat_foo = flatten_view<myTupleType>{foo};
   // flatten_view<myTupleType> flat_foo(foo);
   using myFlatTupleType = result_of::as_vector<decltype(flat_foo)>::type;
   myFlatTupleType flat_foo_vec{1,2,3,4};
   // auto flat_foo_vec = as_vector(v){1,2,3,4};
#if 1
   std::cout << "Size (orig, my) = " << sizeof(foo) << std::endl;
   std::cout << "Size (orig, my) = " << sizeof(flat_foo_vec) << std::endl;
   for_each(foo, print{});
   for_each(flat_foo_vec, print{});
#endif
}

void demonstrateAsVectorNewMem()
{
   std::cout << __func__ << std::endl;
   using myTupleType = TupleMaker<4>::tuple_type;
   myTupleType foo;
   auto flat_foo = flatten_view<myTupleType>{foo};
   // at_c<0>(flat_foo) = 1;
   deref(begin(flat_foo)) = 1;
   //NOTE: unfortunately you cannot pass in constructor args to as_vector
   auto flat_foo_vec = as_vector(flat_foo);//{1,2,3,4};
   at_c<0>(flat_foo_vec) = 34;

#if 1
   //Shows that views point to the same thing, but
   // doing as_vector on a view stamps down a new fusion vector
   std::cout << "Size (orig, my) = " << sizeof(foo) << std::endl;
   for_each(foo, print{});
   std::cout << "Size (orig, flat_vec) = " << sizeof(flat_foo_vec) << std::endl;
   for_each(flat_foo, print{});
   std::cout << "Size (orig, flat_vec new) = " << sizeof(flat_foo_vec) << std::endl;
   for_each(flat_foo_vec, print{});
#endif
}

int main(void)
{
   demonstrateTupleMaker();
   demonstrateAsVectorNewMem();

   using tuple_type = tuple<int, int, int, int, int, short, bool, double>;
   tuple_type t{10, 10, 10, 10, 10, 1, true, 3.14};

   std::cout << "Size (orig) = " << sizeof(t) << std::endl;
   filter_view<tuple_type, std::is_integral<boost::mpl::arg<1>>> v{t};

   std::cout << "Size (view) = " << sizeof(v) << std::endl;

   auto v_vec = as_vector(v);
   std::cout << "Size (vec) = " << sizeof(v_vec) << std::endl;
}
