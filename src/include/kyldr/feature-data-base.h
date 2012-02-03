#ifndef FEATURE_DATA_BASE_H__ 
#define FEATURE_DATA_BASE_H__

#include <vector>
#include <string>

namespace kyldr {

// A virtual class for the data from a single sentence that is used to calculate
// features. This stores the number of words in a standard format to make it
// easier to check to make sure that all sizes are equal
class FeatureDataBase {
public:
    FeatureDataBase() : num_words_(-1) { }
    virtual ~FeatureDataBase() { }
 
    // Reorder the data according to the input vector
    virtual void Reorder(const std::vector<int> & order) = 0;

    // Convert this data into a string for output
    virtual std::string ToString() const = 0;

    // Accessors
    int GetNumWords() { return num_words_; }

protected:
    // The number of words in the sentence
    int num_words_;

private:

};

typedef std::vector<FeatureDataBase*> Sentence;

}

#endif

