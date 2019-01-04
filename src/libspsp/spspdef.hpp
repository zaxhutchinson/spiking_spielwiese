/*
SPSPDEF
by Zachary Hutchinson

Templates and other meta stuff used by SPSP.
*/

#include<vector>
#include<memory>
#include<list>

namespace spsp {

    template<typename T>
    using vec = std::vector<T>;

    template<typename T>
    using lst = std::list<T>;

    template<typename T>
    using uptr = std::unique_ptr<T>;

    template<typename T>
    using sptr = std::shared_ptr<T>;

    template<typename T>
    using wptr = std::shared_ptr<T>;

    template<typename T>
    using vuptr = vec<uptr<T>>;

    template<typename T>
    using vsptr = vec<sptr<T>>;

    template<typename T>
    using vwptr = vec<wptr<T>>;

    template<typename T>
    using luptr = lst<uptr<T>>;

    template<typename T>
    using lsptr = lst<sptr<T>>;

    template<typename T>
    using lwptr = lst<wptr<T>>;
    
}