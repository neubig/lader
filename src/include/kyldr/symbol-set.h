#ifndef SYMBOL_SET_H__
#define SYMBOL_SET_H__

#include <tr1/unordered_map>
#include <vector>
#include <stdexcept>
#include <kyldr/util.h>

namespace kyldr {

template < class Key, class T, class Hash = std::tr1::hash<Key> >
class SymbolSet {

public:

    typedef std::tr1::unordered_map< Key, T, Hash > Map;
    typedef std::vector< Key* > Vocab;
    typedef std::vector< T > Ids;

protected:
    
    Map map_;
    Vocab vocab_;
    Ids reuse_;

public:
    SymbolSet() : map_(), vocab_(), reuse_() { }
    SymbolSet(const SymbolSet & ss) : 
                map_(ss.map_), vocab_(ss.vocab_), reuse_(ss.reuse_) {
        for(typename Vocab::iterator it = vocab_.begin(); 
                it != vocab_.end(); it++) 
            if(*it)
                *it = new Key(**it);
    }
    ~SymbolSet() {
        for(typename Vocab::iterator it = vocab_.begin(); 
                                            it != vocab_.end(); it++)
            if(*it)
                delete *it;
    }

    const std::vector<Key*> & GetSymbols() const { return vocab_; }
    const Key & GetSymbol(T id) const {
        return *SafeAccess(vocab_, id);
    }
    T GetId(const Key & sym, bool add = false) {
        typename Map::const_iterator it = map_.find(sym);
        if(it != map_.end())
            return it->second;
        else if(add) {
            T id;
            if(reuse_.size() != 0) {
                id = reuse_.back(); reuse_.pop_back();
                vocab_[id] = new Key(sym);
            } else {
                id = vocab_.size();
                vocab_.push_back(new Key(sym));
            }
            map_.insert(std::pair<Key,T>(sym,id));
            return id;
        }
        return -1;
    }
    T GetId(const Key & sym) const {
        return const_cast< SymbolSet<Key,T,Hash>* >(this)->GetId(sym,false);
    }
    size_t size() const { return vocab_.size() - reuse_.size(); }
    size_t capacity() const { return vocab_.size(); }
    size_t hashCapacity() const { return map_.size(); }
    void removeId(const T id) {
        map_.erase(*vocab_[id]);
        delete vocab_[id];
        vocab_[id] = 0;
        reuse_.push_back(id);
    }
    
    void ToStream(std::ostream & out) {
        out << vocab_.size() << std::endl;
        for(int i = 0; i < (int)vocab_.size(); i++)
            out << *vocab_[i] << std::endl;
        out << std::endl;
    }
    static SymbolSet<Key,T,Hash>* FromStream(std::istream & in) {
        std::string line;
        int size;
        SymbolSet<Key,T,Hash> * ret = new SymbolSet<Key,T,Hash>;
        getline(in, line); std::istringstream iss(line);
        for(iss >> size; size > 0 && getline(in, line); size--)
            ret->GetId(line, true);
        GetlineEquals(in, "");
        return ret;
    }

    Map & GetMap() { return map_; }

};

}

#endif
