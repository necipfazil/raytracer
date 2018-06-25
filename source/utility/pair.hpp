#ifndef __PAIR_HPP__
#define __PAIR_HPP__

template<class P1, class P2>
class Pair
{
    public:
        const P1 p1;
        const P2 p2;
        
        Pair(const P1 & p1, const P2 & p2)
            : p1(p1), p2(p2) { }
};

template<class P1, class P2, class P3>
class Triple
{
    public:
        const P1 p1;
        const P2 p2;
        const P3 p3;
        
        Triple(const P1 & p1, const P2 & p2, const P3 & p3)
            : p1(p1), p2(p2), p3(p3) { }
};

#endif