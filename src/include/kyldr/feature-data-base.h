#ifndef FEATURE_DATA_BASE_H__ 
#define FEATURE_DATA_BASE_H__

namespace kyldr {

// A virtual class for the data from a single sentence that is used to calculate
// features. This stores the number of words in a standard format to make it
// easier to check to make sure that all sizes are equal
class FeatureDataBase {
public:
    FeatureDataBase() : num_words_(-1) { }
    virtual ~FeatureDataBase() { }
 
    // Accessors
    int GetNumWords() { return num_words_; }

protected:
    // The number of words in the sentence
    int num_words_;

private:

};

}

#endif

