#ifndef TEST_BASE__
#define TEST_BASE__

#include <vector>
#include <iostream>
#include <tr1/unordered_map>

using namespace std;

namespace kyldr {

class TestBase {

public:

    // RunTest must be implemented by any test, and returns true if all
    // tests were passed
    virtual bool RunTest() = 0;

protected:

    TestBase() : passed_(false) { }

    bool passed_;

    template<class T>
    int CheckVector(const std::vector<T> & exp, const std::vector<T> & act) {
        int ok = 1;
        for(int i = 0; i < (int)max(exp.size(), act.size()); i++) {
            if(i >= (int)exp.size() || 
               i >= (int)act.size() || 
               exp[i] != act[i]) {
               
                ok = 0;
                std::cout << "exp["<<i<<"] != act["<<i<<"] (";
                if(i >= (int)exp.size()) std::cout << "NULL";
                else std::cout << exp[i];
                std::cout <<" != ";
                if(i >= (int)act.size()) std::cout << "NULL"; 
                else std::cout << act[i];
                std::cout << ")" << std::endl;
            }
        }
        return ok;
    }

    // Map equality checking function
    template <class Key, class Val>
    bool CheckMap(const std::tr1::unordered_map<Key, Val> & a, 
                  const std::tr1::unordered_map<Key, Val> & b) {
        if(a.size() != b.size()) {
            std::cerr << "CheckMap: a.size() != b.size() ("
                      << a.size() <<", " <<b.size() << std::endl;
            return false;
        }
        for(typename std::tr1::unordered_map<Key, Val>::const_iterator ait = a.begin();
            ait != a.end();
            ait++) {
            typename std::tr1::unordered_map<Key, Val>::const_iterator bit = b.find(ait->first);
            if(bit == b.end() || ait->second != bit->second) {
                std::cerr << "CheckMap: values don't match at " 
                          << ait->first << endl;
                return false;
            }
        }
        return true;
    }

};

}

#endif
