#include "fusion_includes.h"
using namespace boost::fusion;
int main(void)
{
    char const* s = "Ruby";
    typedef vector<int, char, double, char const*> vector_type;
    vector_type vec(1, 'x', 3.3, s);

    typedef result_of::begin<vector_type>::type A;
    typedef result_of::end<vector_type>::type B;
    typedef result_of::next<A>::type C;
    typedef result_of::prior<B>::type D;

    C c(vec);
    D d(vec);

    iterator_range<C, D> range(c, d);
    std::cout << range << std::endl;

    return 0;
}
