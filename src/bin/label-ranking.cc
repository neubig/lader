
#include <kyldr/util.h>
#include <kyldr/ranks.h>
#include <kyldr/combined-alignment.h>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace kyldr;
using namespace std;
using namespace boost;

int main(int argc, char** argv) {
    if(argc != 3) THROW_ERROR("Usage: label-ranking SOURCE ALIGNMENTS");
    ifstream src_in(argv[1]);
    if(!src_in) THROW_ERROR("Could not find source file " << argv[1]);
    ifstream align_in(argv[2]);
    if(!align_in) THROW_ERROR("Could not find alignment file " << argv[2]);
    string src_line, align_line;
    vector<string> srcs;
    while(getline(src_in, src_line) && getline(align_in, align_line)) {
        srcs.clear();
        algorithm::split(srcs, src_line, is_any_of(" "));
        Ranks ranks(CombinedAlign(srcs, Alignment::FromString(align_line),
            CombinedAlign::ATTACH_NULL_LEFT,
            CombinedAlign::COMBINE_BLOCKS,
            CombinedAlign::ALIGN_BRACKET_SPANS));
        if(ranks.GetSrcLen()) cout << "X";
        for(int i = 1; i < (int)ranks.GetSrcLen(); i++) {
            if(ranks[i-1] == ranks[i]) cout << " X";
            else if(ranks[i-1] < ranks[i]) cout << " S";
            else cout << " I";
        }
        cout << endl;
    }
}
