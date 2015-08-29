#ifndef _LADER_UTIL__
#define _LADER_UTIL__

#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "lader/config.h"

#define LADER_SAFE

#define THROW_ERROR(msg) do {                   \
    std::ostringstream oss;                     \
    oss << msg;                                 \
    throw std::runtime_error(oss.str()); }       \
  while (0);

#ifdef HAVE_UNORDERED_MAP
#   include <unordered_map>
    template <class T>
    class StringMap : public std::unordered_map<std::string,T> { };
    template <class T>
    class IntMap : public std::unordered_map<int,T> { };
#elif HAVE_TR1_UNORDERED_MAP
#   include <tr1/unordered_map>
    template <class T>
    class StringMap : public std::tr1::unordered_map<std::string,T> { };
    template <class T>
    class IntMap : public std::tr1::unordered_map<int,T> { };
#endif

namespace std {

// Unordered map equality function
template < class T >
inline bool operator==(const StringMap<T> & lhs, 
                       const StringMap<T> & rhs) {
    if(lhs.size() != rhs.size())
        return false;
    for(typename StringMap<T>::const_iterator it = lhs.begin();
        it != lhs.end();
        it++) {
        typename StringMap<T>::const_iterator it2 = rhs.find(it->first);
        if(it2 == rhs.end() || it2->second != it->second)
            return false;
    }
    return true;
}

// Output function for pairs
template <class X, class Y>
inline std::ostream& operator << ( std::ostream& out, 
                                   const std::pair< X, Y >& rhs )
{
    out << "< " << rhs.first << " " << rhs.second << " >";
    return out;
}

// Input function for pairs
template <class X, class Y>
inline std::istream & operator>> (std::istream & in, std::pair<X,Y>& s) {
    string open, close;
    in >> open >> s.first >> s.second >> close;
    if(open != "<")
        THROW_ERROR("Bad start of pair " << open);
    if(close != "<")
        THROW_ERROR("Bad end of pair " << close);
    return in;
}

// // Less than function for pairs
// template <class X, class Y>
// inline bool operator< ( const std::pair< X, Y >& lhs, const std::pair< X, Y >& rhs ) {
//     return lhs.first < rhs.first || 
//         (lhs.first == rhs.first && lhs.second < rhs.second);
// }

}

namespace lader {

inline bool IsDigit(char c) {
    return c >= '0' && c <= '9';
}

// Make a pair without all the difficult coding
template < class X, class Y >
inline std::pair<X,Y> MakePair(const X & x, const Y & y) {
    return std::pair<X,Y>(x,y);
}

// A hash for pairs
template < class T >
class PairHash {
public:
    size_t operator()(const std::pair<T,T> & x) const {
        size_t hash = 5381;
        hash = ((hash << 5) + hash) + x.first;
        hash = ((hash << 5) + hash) + x.second;
        return hash;
    }
};

inline std::vector<std::string> Tokenize(const char *str, char c = ' ') {
    std::vector<std::string> result;
    while(1) {
        const char *begin = str;
        while(*str != c && *str)
            str++;
        result.push_back(std::string(begin, str));
        if(0 == *str++)
            break;
    }
    return result;
}
inline std::vector<std::string> Tokenize(const std::string &str, char c = ' ') {
    return Tokenize(str.c_str(), c);
}

inline void GetlineEquals(std::istream & in, const std::string & str) {
    std::string line;
    std::getline(in, line);
    if(line != str)
        THROW_ERROR("Expected and received inputs differ." << std::endl
                    << "Expect:  " << str << std::endl
                    << "Receive: " << line << std::endl);
}

template <class X>
inline void GetConfigLine(std::istream & in, const std::string & name, X& val) {
    std::string line;
    std::getline(in, line);
    std::istringstream iss(line);
    std::string myName;
    iss >> myName >> val;
    if(name != myName)
        THROW_ERROR("Expected and received inputs differ." << std::endl
                    << "Expect:  " << name << std::endl
                    << "Receive: " << myName << std::endl);
}


inline bool ApproximateDoubleEquals(double a, double b) {
    return (std::abs(a-b) <= std::abs(a)/1000.0);
}

// Check to make sure that two arrays of doubles are approximately equal
inline bool ApproximateDoubleEquals(const std::vector<double> & a, 
                                    const std::vector<double> & b) {
    if(a.size() != b.size())
        return false;
    for(int i = 0; i < (int)a.size(); i++)
        if (!ApproximateDoubleEquals(a[i],b[i]))
            return false;
    return true;
}

inline bool ApproximateDoubleEquals(
    const StringMap<double> & a,
    const StringMap<double> & b) {
    if(a.size() != b.size())
        return false;
    for(StringMap<double>::const_iterator it = a.begin(); it != a.end(); it++) {
        StringMap<double>::const_iterator it2 = b.find(it->first);
        if(it2 == b.end() || !ApproximateDoubleEquals(it->second, it2->second))
            return false;
    }
    return true;
    
}

// Perform safe access to a vector
template < class T >
inline const T & SafeAccess(const std::vector<T> & vec, int idx) {
#ifdef LADER_SAFE
    if(idx < 0 || idx >= (int)vec.size())
        THROW_ERROR("Out of bound access size="<<vec.size()<<", idx="<<idx);
#endif
    return vec[idx];
}

// Perform safe access to a vector
template < class T >
inline T & SafeAccess(std::vector<T> & vec, int idx) {
#ifdef LADER_SAFE
    if(idx < 0 || idx >= (int)vec.size())
        THROW_ERROR("Out of bound access size="<<vec.size()<<", idx="<<idx);
#endif
    return vec[idx];
}

template < class T >
inline const T & SafeReference(const T * ptr) {
#ifdef LADER_SAFE
    if(!ptr)
        THROW_ERROR("Null pointer access");
#endif
    return *ptr;
}

}  // end namespace

#ifdef __MACH__
#define CLOCK_MONOTONIC 0
#include <mach/clock.h>
#include <mach/mach.h>
#include <time.h>
#include <sys/time.h>
//clock_gettime is not implemented on OSX
inline int clock_gettime(int /*clk_id*/, struct timespec* t) {
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}
#endif

#endif
