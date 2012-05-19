#include <lader/feature-data-parse.h>

using namespace std;
using namespace lader;
using namespace boost;

void FeatureDataParse::FromString(const std::string & str) {
    int pos = 0;
    sequence_.clear();
    vector<pair<int,string> > stack;
    while(pos < (int)str.length()) {
        // Delete white space
        if(str[pos] == ' ') {
            pos++;
        // Open a constituent
        } else if(str[pos] == '(') {
            int start_pos = ++pos;
            // Get the symbol
            while(pos < (int)str.length() && str[pos] != ' ')
                pos++;
            if(start_pos == pos)
                THROW_ERROR("Empty symbol in parse: " << str);
            // Push onto the stack
            stack.push_back(MakePair(sequence_.size(),
                                     str.substr(start_pos,pos-start_pos)));
        // Close a constituent
        } else if(str[pos] == ')') {
            if(stack.size() == 0)
                THROW_ERROR("Too many close brackets in parse: " << str);
            pair<int,string> start = *stack.rbegin();
            stack.pop_back();
            if(start.first == (int)sequence_.size())
                THROW_ERROR("Empty constituent in parse: " << str);
            spans_[MakePair(start.first, (int)sequence_.size()-1)] = start.second;
            pos++;
        } else {
            int start_pos = pos;
            while(pos < (int)str.length() 
                                && str[pos] != ' ' && str[pos] != ')')
                pos++;
            if(start_pos == pos)
                THROW_ERROR("Empty symbol in parse: " << str);
            // Push onto the stack
            sequence_.push_back(str.substr(start_pos,pos-start_pos)); 
        }
    }
    if(stack.size() != 0)
        THROW_ERROR("Too many open brackets in parse: " << str);
}
