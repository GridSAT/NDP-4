
// ClauseSetPool.hpp    
#ifndef CLAUSE_SET_POOL_HPP
#define CLAUSE_SET_POOL_HPP

#include <vector>
#include <cstdint>



// Using default allocation for ClauseSet.

struct Clause3 {
    int l[3];
};

using ClauseSet = std::vector<Clause3>;

struct ClauseSetPool {
        std::vector<ClauseSet*> freeList;
        
        ClauseSet* obtain(std::size_t reserveSize = 0) {
                if (!freeList.empty()) {
                    ClauseSet* cs = freeList.back();
                    freeList.pop_back();
                    cs->clear();
                    if(reserveSize)
                            cs->reserve(reserveSize);
                    return cs;
                }
                ClauseSet* cs = new ClauseSet();
                if(reserveSize)
                        cs->reserve(reserveSize);
                return cs;
        }
        
        void release(ClauseSet* cs) {
                freeList.push_back(cs);
        }
        
        ~ClauseSetPool() {
                for(auto cs : freeList)
                        delete cs;
        }
};

#endif // CLAUSE_SET_POOL_HPP