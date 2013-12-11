#ifndef HYPOTHESIS_QUEUE_H__ 
#define HYPOTHESIS_QUEUE_H__

#include <queue>

namespace lader {

template< typename T >
class PointerLess : public std::binary_function< T, T, bool >{
public :
	bool operator()( T x, T y ) const { return *x < *y; }
};

class Hypothesis;

typedef std::priority_queue<Hypothesis*,
		std::vector<Hypothesis*>, PointerLess<Hypothesis*> > HypothesisQueue;

}

#endif

