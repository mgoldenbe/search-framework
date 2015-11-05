#ifndef UTILITIES
#define UTILITIES

#define BUCKETED(bucketSize) Bucketed<decltype((bucketSize)), bucketSize>

template <typename T, T bucketSize>
struct Bucketed;

// E.g. if bucketSizeD is 0.2:
//   2.29 will be stored as 2.2
//   2.30 will be stored as 2.4
template <const double &bucketSizeD>
struct Bucketed<const double &, bucketSizeD> {
    Bucketed(double myF) : f(round(myF / bucketSizeD) * bucketSizeD) {};
    double f;
    bool operator==(const Bucketed &rhs) const {return f==rhs.f;}
    bool operator<(const Bucketed &rhs) const {return f<rhs.f;}
};
template <const double &bucketSizeD>
std::ostream &operator<<(std::ostream &os,
                         const Bucketed<const double &, bucketSizeD> &f) {
    return os << f.f;
}

// E.g. if bucketSizeI is 5:
//   7 will be stored as 5
//   8 will be stored as 10
template <int bucketSizeI>
struct Bucketed<int, bucketSizeI> {
    Bucketed(int myF) : f(round((double)myF / bucketSizeI) * bucketSizeI) {};
    double f;
    bool operator==(const Bucketed &rhs) const {return f==rhs.f;}
    bool operator<(const Bucketed &rhs) const {return f<rhs.f;}
};
template <int bucketSizeI>
std::ostream &operator<<(std::ostream &os,
                         const Bucketed<int, bucketSizeI> &f) {
    return os << f.f;
}

#endif
