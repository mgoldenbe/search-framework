#include <curses.h>
#include <menu.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
using namespace ::boost;
using namespace ::boost::multi_index;

struct value_t {
    std::string s;
    int i;
};
struct string_tag {};
typedef multi_index_container<
    value_t,
    indexed_by<
        random_access<>, // this index represents insertion order
        hashed_unique< tag<string_tag>, member<value_t, std::string, &value_t::s> >
    >
> values_t;

int main() {
    values_t v;
    v["a"] = 5;
}
