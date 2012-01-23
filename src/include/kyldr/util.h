#ifndef _KYLDR_UTIL__
#define _KYLDR_UTIL__

#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <tr1/unordered_map>

namespace std {

// Unordered map equality function
template < class Key, class T, class Hash >
inline bool operator==(const std::tr1::unordered_map< Key, T, Hash > & lhs, 
                       const std::tr1::unordered_map< Key, T, Hash > & rhs) {
    if(lhs.size() != rhs.size())
        return false;
    for(typename std::tr1::unordered_map< Key, T, Hash >::const_iterator it = lhs.begin();
        it != lhs.end();
        it++) {
        typename std::tr1::unordered_map< Key, T, Hash >::const_iterator it2 = rhs.find(it->first);
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


// Output function for pairs
template <class X, class Y>
inline std::istream & operator>> (std::istream & in, std::pair<X,Y>& s) {
    string open, close;
    in >> open >> s.first >> s.second >> close;
    CHECK_EQ("<", open);
    CHECK_EQ(">", close);
    return in;
}

}

namespace kyldr {

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
    CHECK_EQ(line, str);
}

template <class X>
inline void GetConfigLine(std::istream & in, const std::string & name, X& val) {
    std::string line;
    std::getline(in, line);
    std::istringstream iss(line);
    std::string myName;
    iss >> myName >> val;
    CHECK_EQ(name, myName);
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

template < class Key, class Hash >
inline bool ApproximateDoubleEquals(
    const std::tr1::unordered_map< Key, double, Hash > & a,
    const std::tr1::unordered_map< Key, double, Hash > & b) {
    if(a.size() != b.size())
        return false;
    for(typename std::tr1::unordered_map< Key, double, Hash >::const_iterator it = a.begin(); it != a.end(); it++) {
        typename std::tr1::unordered_map< Key, double, Hash >::const_iterator it2 = b.find(it->first);
        if(it2 == b.end() || !ApproximateDoubleEquals(it->second, it2->second))
            return false;
    }
    return true;
    
}

}  // end namespace

#endif
