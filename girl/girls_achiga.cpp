#include "girls_achiga.h"
#include "../table/table.h"
#include "../table/princess.h"
#include "../util/misc.h"

#include <cstdlib>



namespace saki
{



void Kuro::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    T34 dora = princess.getImageIndic(Princess::Indic::DORA).dora();

    using namespace tiles37;

    int self = mSelf.index();
    if (mCd) {
        exists[self].inc(dora, -EJECT_MK);
        exists[self].inc(0_m, -EJECT_MK);
        exists[self].inc(0_p, -EJECT_MK);
        exists[self].inc(0_s, -EJECT_MK);
    } else {
        for (int w = 0; w < 4; w++) {
            int doraDelta = w == self ? 150 : -EJECT_MK;
            int akadoraDelta = w == self ? 60 : -EJECT_MK;
            exists[w].inc(dora, doraDelta);
            exists[w].inc(0_m, akadoraDelta);
            exists[w].inc(0_p, akadoraDelta);
            exists[w].inc(0_s, akadoraDelta);
        }
    }
}

bool Kuro::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    if (who != mSelf || iter > 100)
        return true;


    // drop extreme cases
    int doraCt = princess.getImageIndic(Princess::Indic::DORA) % init;
    int akaCt = init.ctAka5();
    return (1 <= doraCt && doraCt <= 2) && (1 <= akaCt && akaCt <= 3);
}

void Kuro::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table;

    std::vector<T34> doras;
    for (const T37 &indic : mount.getDrids())
        doras.push_back(indic.dora());

    using namespace tiles37;

    if (mCd) {
        if (who != mSelf)
            return;
        for (T34 d : doras)
            mount.lightA(d, -EJECT_MK, rinshan);

        mount.lightA(0_m, -EJECT_MK, rinshan);
        mount.lightA(0_p, -EJECT_MK, rinshan);
        mount.lightA(0_s, -EJECT_MK, rinshan);
    } else {
        if (who == mSelf) {
            int turn = table.getRiver(mSelf).size();
            const Hand &hand = table.getHand(mSelf);

            for (T34 d : doras) {
                int expect = turn < 6 ? 2 : 3;
                if (hand.ct(d) < expect)
                    mount.lightA(d, d == doras.front() ? 300 : 600, rinshan);
            }

            int akaExpect = turn < 5 ? 2 : (turn < 10 ? 3 : 4);
            int akaMk = hand.ctAka5() < akaExpect ? 200 : 60;
            mount.lightA(0_m, akaMk, rinshan);
            mount.lightA(0_p, akaMk, rinshan);
            mount.lightA(0_s, akaMk, rinshan);
        } else {
            for (T34 d : doras)
                mount.lightA(d, -EJECT_MK, rinshan);
            mount.lightA(0_m, -EJECT_MK, rinshan);
            mount.lightA(0_p, -EJECT_MK, rinshan);
            mount.lightA(0_s, -EJECT_MK, rinshan);
        }
    }
}

void Kuro::onDiscarded(const Table &table, Who who)
{
    if (who != mSelf)
        return;

    // for equalty, 34/37 not important
    const T37 &last = table.getFocusTile();
    T37 lastIndicator(last.indicator().id34());
    const auto &indics = table.getMount().getDrids();
    if (util::has(indics, lastIndicator) || last.isAka5())
        mCd = true;
}



using namespace tiles34;
const std::vector<T34> Yuu::LV5_TARS { 1_m, 2_m, 3_m, 4_m, 5_m, 6_m, 7_m, 8_m, 9_m, 3_y };
const std::vector<T34> Yuu::LV4_TARS { 6_p, 7_p, 9_p, 9_s };
const std::vector<T34> Yuu::LV3_TARS { 1_p, 3_p, 5_p, 1_s, 5_s, 7_s };

void Yuu::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;

    for (T34 t : LV5_TARS)
        exists[mSelf.index()].inc(t, LV5_VAL);

    for (T34 t : LV4_TARS)
        exists[mSelf.index()].inc(t, LV4_VAL);

    for (T34 t : LV3_TARS)
        exists[mSelf.index()].inc(t, LV3_VAL);
}

void Yuu::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table;

    if (who != mSelf || rinshan)
        return;

    for (T34 t : LV5_TARS)
        mount.lightA(t, LV5_VAL);

    for (T34 t : LV4_TARS)
        mount.lightA(t, LV4_VAL);

    for (T34 t : LV3_TARS)
        mount.lightA(t, LV3_VAL);
}



void Ako::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(mSelf);
    std::vector<T34> seqHeads;
    for (const M37 &m : hand.barks())
        if (m.type() == M37::Type::CHII)
            seqHeads.push_back(m[0]);

    if (seqHeads.size() == 1) {
        oneDragTwo(mount, hand.closed(), seqHeads[0]);
    } else if (seqHeads.size() == 2) {
        bool sameSuit = seqHeads[0].suit() == seqHeads[1].suit();
        int absValDiff = std::abs(seqHeads[0].val() - seqHeads[1].val());
        bool like3 = !sameSuit && seqHeads[0].val() == seqHeads[1].val();
        bool like1 = sameSuit && (absValDiff == 3 || absValDiff == 6);
        if (like3) {
            int s0 = static_cast<int>(seqHeads[0].suit());
            int s1 = static_cast<int>(seqHeads[1].suit());
            Suit s = static_cast<Suit>(3 - (s0 + s1));
            thinFill(mount, hand.closed(), T34(s, seqHeads[0].val()));
        } else if (like1) {
            int v = 12 - (seqHeads[0].val() + seqHeads[1].val());
            thinFill(mount, hand.closed(), T34(seqHeads[0].suit(), v));
        } else {
            int dist3A = sskDist(hand.closed(), seqHeads[0]);
            int dist1A = ittDist(hand.closed(), seqHeads[0]);
            int dist3B = sskDist(hand.closed(), seqHeads[1]);
            int dist1B = ittDist(hand.closed(), seqHeads[1]);
            int minA = std::min(dist3A, dist1A);
            int minB = std::min(dist3B, dist1B);
            if (minA < minB) {
                oneDragTwo(mount, hand.closed(), seqHeads[0]);
            } else {
                oneDragTwo(mount, hand.closed(), seqHeads[1]);
            }
        }
    }

    const auto &drids = mount.getDrids();
    accelerate(mount, hand, table.getRiver(mSelf), 15);
    if (hand.ctAka5() + drids % hand < 1) {
        for (const T37 &t : drids)
            mount.lightA(t.dora(), 80);
        mount.lightA(T37(Suit::M, 0), 30);
        mount.lightA(T37(Suit::P, 0), 30);
        mount.lightA(T37(Suit::S, 0), 30);
    }
}

int Ako::sskDist(const TileCount &c, T34 head)
{
    assert(head.isNum());
    Suit a = static_cast<Suit>((static_cast<int>(head.suit()) + 1) % 3);
    Suit b = static_cast<Suit>((static_cast<int>(head.suit()) + 2) % 3);
    T34 ah(a, head.val());
    T34 bh(b, head.val());
    int aaa = (c.ct(ah) > 0) + (c.ct(ah.next()) > 0) + (c.ct(ah.nnext()) > 0);
    int bbb = (c.ct(bh) > 0) + (c.ct(bh.next()) > 0) + (c.ct(bh.nnext()) > 0);
    return 6 - (aaa + bbb);
}

int Ako::ittDist(const TileCount &closed, T34 head)
{
    assert(head.isNum());

    Suit s = head.suit();
    int v = head.val();
    int sum = 0;

    if (v == 1) {
        for (int i = 4; i <= 9; i++)
            sum += (closed.ct(T34(s, i)) > 0);
    } else if (v == 4) {
        for (int i = 1; i <= 3; i++)
            sum += (closed.ct(T34(s, i)) > 0);
        for (int i = 7; i <= 8; i++)
            sum += (closed.ct(T34(s, i)) > 0);
    } else if (v == 7) {
        for (int i = 1; i <= 6; i++)
            sum += (closed.ct(T34(s, i)) > 0);
    } else {
        sum = -3;
    }

    return 6 - sum;
}

void Ako::oneDragTwo(Mount &mount, const TileCount &closed, T34 head)
{
    int dist3 = sskDist(closed, head);
    int dist1 = ittDist(closed, head);
    if (dist3 < dist1) {
        if (0 < dist3 && dist3 <= 3) {
            for (Suit s : { Suit::M, Suit::P, Suit::S }) {
                if (s == head.suit())
                    continue;
                thinFill(mount, closed, T34(s, head.val()));
            }
        }
    } else {
        if (0 < dist1 && dist1 <= 3) {
            for (int v : { 1, 4, 7 }) {
                if (v == head.val())
                    continue;
                thinFill(mount, closed, T34(head.suit(), v));
            }
        }
    }
}

void Ako::thinFill(Mount &mount, const TileCount &closed, T34 head)
{
    for (T34 t : { head, head.next(), head.nnext() })
        if (closed.ct(t) == 0)
            mount.lightA(t, 600);
}


void Arata::onDice(util::Rand &rand, const Table &table, Choices &choices)
{
    (void) choices;
    using namespace std;

        int t = rand.gen()%(7-0+1)+7;
        bowling = t;

        //0=5556789
        //1=334567
        //2=7799
        //3=5556
        //4=2333456
        //5=5577
        //6=2288
        //7=3399


}


bool Arata::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{


    (void) princess;
    using namespace tiles34;
    using namespace std;
    int p2 =init.closed().ct(2_p);
    int p3 =init.closed().ct(3_p);
    int p4 =init.closed().ct(4_p);
    int p5 =init.closed().ct(5_p);
    int p6 =init.closed().ct(6_p);
    int p7 =init.closed().ct(7_p);
    int p8 =init.closed().ct(8_p);
    int p9 =init.closed().ct(9_p);


        //保险
        if(bowling==0){
            return p5<1 && p6<3 && p7<3 && p8<3 && p9<3;
            }
        if(bowling==1){
            return p3<1 && p4<3 && p5<3 && p6<3 && p7<3;
            }
        if(bowling==2){
            return p7<2 && p9<2;
            }
        if(bowling==3){
            return p5<3 && p6<1;
            }
        if(bowling==4){
            return p2<3 && p3<1 && p4<3 && p5<3 && p6<3;
            }
        if(bowling==5){
            return p5<2 && p7<2;
            }
        if(bowling==6){
            return p2<2 && p8<2;
            }
        if(bowling==7){
            return p3<2 && p9<2;
            }


}


void Arata::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
         using namespace tiles34;
    if(bowling==0)
    for (T34 t : { 5_p, 6_p, 7_p, 8_p, 9_p }){
        exists[mSelf.index()].inc(T34(t), 50);
}
    if(bowling==1)
    for (T34 t : { 3_p, 4_p, 5_p, 6_p, 7_p }){
        exists[mSelf.index()].inc(T34(t), 50);
}
    if(bowling==2)
    for (T34 t : { 7_p, 9_p }){
        exists[mSelf.index()].inc(T34(t), 50);
}
    if(bowling==3)
    for (T34 t : { 5_p, 6_p }){
        exists[mSelf.index()].inc(T34(t), 50);
}
    if(bowling==4)
    for (T34 t : { 2_p, 3_p, 4_p, 5_p, 6_p }){
        exists[mSelf.index()].inc(T34(t), 50);
}
    if(bowling==5)
    for (T34 t : { 5_p, 7_p }){
        exists[mSelf.index()].inc(T34(t), 50);
}
    if(bowling==6)
    for (T34 t : { 2_p, 8_p }){
        exists[mSelf.index()].inc(T34(t), 50);
}
    if(bowling==7)
    for (T34 t : { 3_p, 9_p }){
        exists[mSelf.index()].inc(T34(t), 50);
}
}


void Arata::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(mSelf);
    const Hand &myHand = table.getHand(mSelf);

    if(myHand.ready())
        for (T34 t : myHand.effA()) {
            for (T34 s : { 4_p, 6_p, 7_p, 9_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 2_p, 4_p, 5_p, 7_p, 8_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 7_p, 9_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 2_p, 8_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 4_p, 6_p, 7_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 1_p, 2_p, 4_p, 7_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 5_p, 7_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 3_p, 9_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 3_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 5_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 7_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 1_p, 7_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 2_p, 4_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 2_p, 7_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 3_p, 6_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 3_p, 7_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 4_p, 5_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 5_p, 6_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 1_p, 3_p, 6_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
            for (T34 s : { 1_p, 2_p, 4_p, 5_p }){
                if(t==s)
                mount.lightA(t, 900);
            }
    }
        if(!myHand.ready())
            using namespace tiles34;
           const TileCount &closed = hand.closed();
                int p2 =closed.ct(2_p);
                int p3 =closed.ct(3_p);
                int p4 =closed.ct(4_p);
                int p5 =closed.ct(5_p);
                int p6 =closed.ct(6_p);
                int p7 =closed.ct(7_p);
                int p8 =closed.ct(8_p);
                int p9 =closed.ct(9_p);
            if((p5<3||p6<1||p7<1||p8<1||p9<1) && (p3<3||p4<1||p5<1||p6<1||p7<1) &&
                (p7<2||p9<2) && (p2<2||p8<2) && (p5<3||p6<1) && (p2<1||p3<3||p4<1||p5<1||p6<1)
                    && (p5<2||p7<2) && (p3<2||p9<2))
                if(bowling==0)
                   if(p5<3){
                   mount.lightA(5_p, 200);
                   }
                   if(p6<1){
                   mount.lightA(6_p, 50);
                   }
                   if(p7<1){
                   mount.lightA(7_p, 50);
                   }
                   if(p8<1){
                   mount.lightA(8_p, 50);
                   }
                   if(p9<1){
                   mount.lightA(9_p, 50);
                   }
                if(bowling==1)
                   if(p3<3){
                   mount.lightA(3_p, 200);
                   }
                   if(p4<1){
                   mount.lightA(4_p, 50);
                   }
                   if(p5<1){
                   mount.lightA(5_p, 50);
                   }
                   if(p6<1){
                   mount.lightA(6_p, 50);
                   }
                   if(p7<1){
                   mount.lightA(7_p, 50);
                   }
                   if(p5<3){
                   mount.lightA(5_p, 200);
                   }
                   if(p6<1){
                   mount.lightA(6_p, 50);
                   }
                   if(p7<1){
                   mount.lightA(7_p, 50);
                   }
                   if(p8<1){
                   mount.lightA(8_p, 50);
                   }
                   if(p9<1){
                   mount.lightA(9_p, 50);
                   }
                if(bowling==2)
                   if(p7<2){
                   mount.lightA(7_p, 150);
                   }
                   if(p9<2){
                   mount.lightA(9_p, 150);
                   }
                if(bowling==3)
                   if(p5<3){
                   mount.lightA(5_p, 200);
                   }
                   if(p6<1){
                   mount.lightA(6_p, 50);
                   }
                if(bowling==4)
                   if(p2<1){
                   mount.lightA(2_p, 50);
                   }
                   if(p3<3){
                   mount.lightA(3_p, 200);
                   }
                   if(p4<1){
                   mount.lightA(4_p, 50);
                   }
                   if(p5<1){
                   mount.lightA(5_p, 50);
                   }
                   if(p6<1){
                   mount.lightA(6_p, 50);
                   }
                if(bowling==5)
                   if(p5<2){
                   mount.lightA(5_p, 150);
                   }
                   if(p7<2){
                   mount.lightA(7_p, 150);
                   }
                if(bowling==6)
                   if(p2<2){
                   mount.lightA(2_p, 150);
                   }
                   if(p8<2){
                   mount.lightA(8_p, 150);
                   }
                if(bowling==7)
                   if(p3<2){
                   mount.lightA(3_p, 150);
                   }
                   if(p9<2){
                   mount.lightA(4_p, 150);
                   }

                else
                accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 10);



}



} // namespace saki



void Kuro::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table;

    std::vector<T34> doras;
    for (const T37 &indic : mount.getDrids())
        doras.push_back(indic.dora());

    using namespace tiles37;

    if (mCd) {
        if (who != mSelf)
            return;
        for (T34 d : doras)
            mount.lightA(d, -EJECT_MK, rinshan);

        mount.lightA(0_m, -EJECT_MK, rinshan);
        mount.lightA(0_p, -EJECT_MK, rinshan);
        mount.lightA(0_s, -EJECT_MK, rinshan);
    } else {
        if (who == mSelf) {
            int turn = table.getRiver(mSelf).size();
            const Hand &hand = table.getHand(mSelf);

            for (T34 d : doras) {
                int expect = turn < 6 ? 2 : 3;
                if (hand.ct(d) < expect)
                    mount.lightA(d, d == doras.front() ? 300 : 600, rinshan);
            }

            int akaExpect = turn < 5 ? 2 : (turn < 10 ? 3 : 4);
            int akaMk = hand.ctAka5() < akaExpect ? 200 : 60;
            mount.lightA(0_m, akaMk, rinshan);
            mount.lightA(0_p, akaMk, rinshan);
            mount.lightA(0_s, akaMk, rinshan);
        } else {
            for (T34 d : doras)
                mount.lightA(d, -EJECT_MK, rinshan);
            mount.lightA(0_m, -EJECT_MK, rinshan);
            mount.lightA(0_p, -EJECT_MK, rinshan);
            mount.lightA(0_s, -EJECT_MK, rinshan);
        }
    }
}

void Kuro::onDiscarded(const Table &table, Who who)
{
    if (who != mSelf)
        return;

    // for equalty, 34/37 not important
    const T37 &last = table.getFocusTile();
    T37 lastIndicator(last.indicator().id34());
    const auto &indics = table.getMount().getDrids();
    if (util::has(indics, lastIndicator) || last.isAka5())
        mCd = true;
}



using namespace tiles34;
const std::vector<T34> Yuu::LV5_TARS { 1_m, 2_m, 3_m, 4_m, 5_m, 6_m, 7_m, 8_m, 9_m, 3_y };
const std::vector<T34> Yuu::LV4_TARS { 6_p, 7_p, 9_p, 9_s };
const std::vector<T34> Yuu::LV3_TARS { 1_p, 3_p, 5_p, 1_s, 5_s, 7_s };

void Yuu::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;

    for (T34 t : LV5_TARS)
        exists[mSelf.index()].inc(t, LV5_VAL);

    for (T34 t : LV4_TARS)
        exists[mSelf.index()].inc(t, LV4_VAL);

    for (T34 t : LV3_TARS)
        exists[mSelf.index()].inc(t, LV3_VAL);
}

void Yuu::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) table;

    if (who != mSelf || rinshan)
        return;

    for (T34 t : LV5_TARS)
        mount.lightA(t, LV5_VAL);

    for (T34 t : LV4_TARS)
        mount.lightA(t, LV4_VAL);

    for (T34 t : LV3_TARS)
        mount.lightA(t, LV3_VAL);
}



void Ako::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(mSelf);
    std::vector<T34> seqHeads;
    for (const M37 &m : hand.barks())
        if (m.type() == M37::Type::CHII)
            seqHeads.push_back(m[0]);

    if (seqHeads.size() == 1) {
        oneDragTwo(mount, hand.closed(), seqHeads[0]);
    } else if (seqHeads.size() == 2) {
        bool sameSuit = seqHeads[0].suit() == seqHeads[1].suit();
        int absValDiff = std::abs(seqHeads[0].val() - seqHeads[1].val());
        bool like3 = !sameSuit && seqHeads[0].val() == seqHeads[1].val();
        bool like1 = sameSuit && (absValDiff == 3 || absValDiff == 6);
        if (like3) {
            int s0 = static_cast<int>(seqHeads[0].suit());
            int s1 = static_cast<int>(seqHeads[1].suit());
            Suit s = static_cast<Suit>(3 - (s0 + s1));
            thinFill(mount, hand.closed(), T34(s, seqHeads[0].val()));
        } else if (like1) {
            int v = 12 - (seqHeads[0].val() + seqHeads[1].val());
            thinFill(mount, hand.closed(), T34(seqHeads[0].suit(), v));
        } else {
            int dist3A = sskDist(hand.closed(), seqHeads[0]);
            int dist1A = ittDist(hand.closed(), seqHeads[0]);
            int dist3B = sskDist(hand.closed(), seqHeads[1]);
            int dist1B = ittDist(hand.closed(), seqHeads[1]);
            int minA = std::min(dist3A, dist1A);
            int minB = std::min(dist3B, dist1B);
            if (minA < minB) {
                oneDragTwo(mount, hand.closed(), seqHeads[0]);
            } else {
                oneDragTwo(mount, hand.closed(), seqHeads[1]);
            }
        }
    }

    const auto &drids = mount.getDrids();
    accelerate(mount, hand, table.getRiver(mSelf), 15);
    if (hand.ctAka5() + drids % hand < 1) {
        for (const T37 &t : drids)
            mount.lightA(t.dora(), 80);
        mount.lightA(T37(Suit::M, 0), 30);
        mount.lightA(T37(Suit::P, 0), 30);
        mount.lightA(T37(Suit::S, 0), 30);
    }
}

int Ako::sskDist(const TileCount &c, T34 head)
{
    assert(head.isNum());
    Suit a = static_cast<Suit>((static_cast<int>(head.suit()) + 1) % 3);
    Suit b = static_cast<Suit>((static_cast<int>(head.suit()) + 2) % 3);
    T34 ah(a, head.val());
    T34 bh(b, head.val());
    int aaa = (c.ct(ah) > 0) + (c.ct(ah.next()) > 0) + (c.ct(ah.nnext()) > 0);
    int bbb = (c.ct(bh) > 0) + (c.ct(bh.next()) > 0) + (c.ct(bh.nnext()) > 0);
    return 6 - (aaa + bbb);
}

int Ako::ittDist(const TileCount &closed, T34 head)
{
    assert(head.isNum());

    Suit s = head.suit();
    int v = head.val();
    int sum = 0;

    if (v == 1) {
        for (int i = 4; i <= 9; i++)
            sum += (closed.ct(T34(s, i)) > 0);
    } else if (v == 4) {
        for (int i = 1; i <= 3; i++)
            sum += (closed.ct(T34(s, i)) > 0);
        for (int i = 7; i <= 8; i++)
            sum += (closed.ct(T34(s, i)) > 0);
    } else if (v == 7) {
        for (int i = 1; i <= 6; i++)
            sum += (closed.ct(T34(s, i)) > 0);
    } else {
        sum = -3;
    }

    return 6 - sum;
}

void Ako::oneDragTwo(Mount &mount, const TileCount &closed, T34 head)
{
    int dist3 = sskDist(closed, head);
    int dist1 = ittDist(closed, head);
    if (dist3 < dist1) {
        if (0 < dist3 && dist3 <= 3) {
            for (Suit s : { Suit::M, Suit::P, Suit::S }) {
                if (s == head.suit())
                    continue;
                thinFill(mount, closed, T34(s, head.val()));
            }
        }
    } else {
        if (0 < dist1 && dist1 <= 3) {
            for (int v : { 1, 4, 7 }) {
                if (v == head.val())
                    continue;
                thinFill(mount, closed, T34(head.suit(), v));
            }
        }
    }
}

void Ako::thinFill(Mount &mount, const TileCount &closed, T34 head)
{
    for (T34 t : { head, head.next(), head.nnext() })
        if (closed.ct(t) == 0)
            mount.lightA(t, 600);
}



} // namespace saki


