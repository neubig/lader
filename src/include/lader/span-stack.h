#ifndef SPAN_STACK_H__ 
#define SPAN_STACK_H__

namespace lader {

class SpanStack {
public:

    ~SpanStack() {
        BOOST_FOREACH(TargetSpan *span, spans_)
            delete span;
    }

    TargetSpan* GetSpanOfRank(int rank) {
        if(rank < 0 || rank >= (int)spans_.size())
            return NULL;
        return spans_[rank];
    }
    const TargetSpan* GetSpanOfRank(int rank) const {
        if(rank < 0 || rank >= (int)spans_.size())
            return NULL;
        return spans_[rank];
    }

    void AddSpan(TargetSpan* span) {
        spans_.push_back(span);
    }
    void push_back(TargetSpan* span) {
        spans_.push_back(span);
    }

    size_t size() const { return spans_.size(); }
    const TargetSpan* operator[] (size_t val) const { return spans_[val]; }
    TargetSpan* operator[] (size_t val) { return spans_[val]; }
    const std::vector<TargetSpan*> & GetSpans() const { return spans_; }
    std::vector<TargetSpan*> & GetSpans() { return spans_; }

private:

    std::vector<TargetSpan*> spans_;

};

}

#endif

