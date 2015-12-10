///@file
///@brief Source: http://stackoverflow.com/a/11744832/2725810
#ifndef REFLECT_STRUCT
#define REFLECT_STRUCT

#include <iostream>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/bind.hpp>

#define REM(...) __VA_ARGS__
#define EAT(...)

// Retrieve the type
#define TYPEOF(x) DETAIL_TYPEOF(DETAIL_TYPEOF_PROBE x, )
#define DETAIL_TYPEOF(...) DETAIL_TYPEOF_HEAD(__VA_ARGS__)
#define DETAIL_TYPEOF_HEAD(x, ...) REM x
#define DETAIL_TYPEOF_PROBE(...) (__VA_ARGS__),
// Strip off the type
#define STRIP(x) EAT x
// Show the type without parenthesis
#define PAIR(x) REM x

// Next, we define a REFLECTABLE macro to generate the data about each
// field(plus the field itself). This macro will be called like this:
// REFLECTABLE
// (
//     (const char *) name,
//     (int) age
// )

// So using Boost.PP we iterate over each argument and generate the data like
// this:

// A helper metafunction for adding const to a type
template <class M, class T> struct make_const {
    typedef T type;
};

template <class M, class T> struct make_const<const M, T> {
    typedef typename boost::add_const<T>::type type;
};

#define REFLECTABLE(...)                                                       \
    static const int fields_n = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__);           \
    friend struct reflector;                                                   \
    template <int N, class Self> struct field_data {};                         \
    BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH, data,                                \
                            BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define REFLECT_EACH(r, data, i, x)                                            \
    PAIR(x);                                                                   \
    template <class Self> struct field_data<i, Self> {                         \
        Self &self;                                                            \
        field_data(Self &self) : self(self) {}                                 \
                                                                               \
        typename make_const<Self, TYPEOF(x)>::type &get() {                    \
            return self.STRIP(x);                                              \
        }                                                                      \
        typename boost::add_const<TYPEOF(x)>::type &get() const {              \
            return self.STRIP(x);                                              \
        }                                                                      \
        const char *name() const { return BOOST_PP_STRINGIZE(STRIP(x)); }      \
    }; // What this does is generate a constant fields_n that is number of
       // reflectable
// fields in the class. Then it specializes the field_data for each field. It
// also friends the reflector class, this is so it can access the fields even
// when they are private:

struct reflector {
    // Get field_data at index N
    template <int N, class T>
    static typename T::template field_data<N, T> get_field_data(T &x) {
        return typename T::template field_data<N, T>(x);
    }

    // Get the number of fields
    template <class T> struct fields {
        static const int n = T::fields_n;
    };
};
// Now to iterate over the fields we use the visitor pattern. We create an MPL
// range from 0 to the number of fields, and access the field data at that
// index. Then it passes the field data on to the user-provided visitor:

struct field_visitor {
    template <class C, class Visitor, class T>
    void operator()(C &c, Visitor v, T) {
        v(reflector::get_field_data<T::value>(c));
    }
};

template <class C, class Visitor> void visit_each(C &c, Visitor v) {
    typedef boost::mpl::range_c<int, 0, reflector::fields<C>::n> range;
    boost::mpl::for_each<range>(
        boost::bind<void>(field_visitor(), boost::ref(c), v, _1));
}

/*
template <class C1, class C2, class Visitor>
void visit_each_2(C1 &c1, C2 &c2, Visitor v) {
    typedef boost::mpl::range_c<int, 0, reflector::fields<C1>::n> range;
    boost::mpl::for_each<range>(boost::bind<void>(field_visitor_2(), v, _1, _2)(
        boost::ref(c1), boost::ref(c2)));
}
*/

#endif
