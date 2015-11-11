#ifndef UTILITIES
#define UTILITIES

// E.g. if bucketSize is 0.2:
//   2.29 will be stored as 2.2
//   2.30 will be stored as 2.4
template <const double &bucketSize>
struct BucketedDouble {
    BucketedDouble(double myF) : f(round(myF / bucketSize) * bucketSize) {};
    double f;
    bool operator==(const BucketedDouble &rhs) const {return f==rhs.f;}
    bool operator<(const BucketedDouble &rhs) const {return f<rhs.f;}
};
template <const double &bucketSize>
std::ostream &operator<<(std::ostream &os, const BucketedDouble<bucketSize> &f) {return os << f.f;}

// E.g. if bucketSize is 5:
//   7 will be stored as 5
//   8 will be stored as 10
template <int bucketSize>
struct BucketedInt {
    BucketedInt(int myF) : f(round((double)myF / bucketSize) * bucketSize) {};
    double f;
    bool operator==(const BucketedInt &rhs) const {return f==rhs.f;}
    bool operator<(const BucketedInt &rhs) const {return f<rhs.f;}
};
template <int bucketSize>
std::ostream &operator<<(std::ostream &os, const BucketedInt<bucketSize> &f) {return os << f.f;}

#endif
