#ifndef TEST_BASE__
#define TEST_BASE__

#include <vector>
#include <iostream>

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

};

}

#endif
