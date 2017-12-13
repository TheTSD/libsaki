#include "girls_other.h"
#include "../table/table.h"
#include "../util/misc.h"
#include "../unit/tile.h"
#include "../table/table_view_hand.h"
#include "../table/princess.h"


namespace saki
{


void Kazue::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{

    (void) table;
    const Hand &hand = table.getHand(mSelf);
    const TileCount &closed = hand.closed();
    const auto &barks = hand.barks();


    if (rinshan)
        return;

    if (who != mSelf & table.getRound() >= 4){
        accelerate(mount, table.getHand(who), table.getRiver(who), -12);
    }

    if (who == mSelf & table.getRound() >= 4)
        accelerate(mount, table.getHand(who), table.getRiver(who), 80);

    else
        accelerate(mount, table.getHand(who), table.getRiver(who), -10);

    if (who == mSelf & table.getRound() >= 4 & table.riichiEstablished(mSelf)){
        accelerate(mount, table.getHand(who), table.getRiver(who), 70);
    }

    if (table.riichiEstablished(mSelf) && table.getRound() >= 0){
        for (int pos = 0; pos < 5; pos++) {
            const TileCount &closed = hand.closed();
                  for (T34 t : tiles34::ALL34) {
                      if (closed.ct(t) >= 1){
                      T34 i = t.indicator();
                mount.power(Mount::URADORA, pos, i, 5000, false);
            }
          }
        }
}

}

void Uta::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(mSelf);
    if (!hand.isMenzen())
        return;

    if (table.getRiver(mSelf).size() < 5 && hand.step() <= 1) {
        for (T34 t : hand.effA())
            mount.lightA(t, -40); // slow down
    } else {
        if (hand.ready()) {
            const FormCtx &ctx = table.getFormCtx(mSelf);
            const Rule &rule = table.getRule();
            const auto &drids = mount.getDrids();

            for (T34 t : hand.effA()) {
                Form f(hand, T37(t.id34()), ctx, rule, drids);
                int ronHan = f.han();
                int tsumoHan = ronHan + 1;
                bool strong = tsumoHan >= 6;
                bool greedy = ronHan >= 7 && table.riichiEstablished(mSelf);
                mount.lightA(t, strong && !greedy ? 400 : -50);
            }
        } else {
            const River &river = table.getRiver(mSelf);
            if (!tryPowerDye(hand, mount))
                power3sk(hand, river, mount);
        }
    }
}

void Uta::power3sk(const Hand &hand, const River &river, Mount &mount)
{
    const auto parseds = hand.parse4();

    auto inRiver = [&river](T34 t) {
        return util::any(river, [t](const T37 &r) { return t == r; });
    };

    auto comp = [&inRiver](const Parsed &a, const Parsed &b) {
        auto needA = a.claim3sk();
        auto needB = b.claim3sk();

        bool hateA = util::any(needA, inRiver);
        bool hateB = util::any(needB, inRiver);
        if (hateA != hateB)
            return hateB;

        if (needA.size() < needB.size())
            return true;

        if (needA.size() == needB.size()) {
            auto isYao = [](T34 t) { return t.isYao(); };
            return util::any(needA, isYao) && util::none(needB, isYao);
        }

        return false;
    };

    auto needs = std::min_element(parseds.begin(), parseds.end(), comp)->claim3sk();
    for (T34 t : needs)
        if (util::none(river, [t](const T37 &r) { return t == r; }))
            mount.lightA(t, 500);

    if (needs.empty() && river.size() >= 9)
        accelerate(mount, hand, river, 100);

    const auto &drids = mount.getDrids();
    if (hand.ctAka5() + drids % hand < 3)
        for (const T37 &ind : drids)
            if (util::none(river, [&ind](const T37 &r) { return ind % r; }))
                mount.lightA(ind.dora(), 100);
}

bool Uta::tryPowerDye(const Hand &hand, Mount &mount)
{
    const TileCount &closed = hand.closed();

    auto sum = [&closed](Suit s) {
        int sum = 0;
        for (int v = 1; v <= 9; v++)
            sum += closed.ct(T34(s, v));
        return sum;
    };

    std::array<Suit, 3> mps { Suit::M, Suit::P, Suit::S };
    std::array<int, 3> sums;
    std::transform(mps.begin(), mps.end(), sums.begin(), sum);

    auto it = std::max_element(sums.begin(), sums.end());
    if (*it + closed.ctZ() < 9)
        return false;

    Suit s = mps[it - sums.begin()];
    for (int v = 1; v <= 9; v++) {
        T34 t(s, v);
        int doraVal = mount.getDrids() % t;
        mount.lightA(t, 300 + doraVal * 100);
    }

    return true;
}


void Rio::onDice(util::Rand &rand, const Table &table, Choices &choices)
{
    (void) choices;
    using namespace std;
    int i =  rand.gen(105);
    yakuman = i;


}



bool Rio::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{

    (void) princess;


    //四暗刻配牌挂，配3组以上对子
    if (91 <= yakuman & yakuman <= 105){
        if(who == mSelf)
            for (T34 t : tiles34::ALL34)
                if (init.closed().ct(t) > 3)
                    return false;

            int PairCt = std::count_if(tiles37::ORDER37.begin(), tiles37::ORDER37.end(),
                                          [&init](T34 t) { return init.closed().ct(t) == 2; });
            return PairCt >= 3;
             }
            else if(who == mSelf || iter > 100)
            return true;


    //以下参数测试用，实装请选择=1、=2等等
    //大三元，禁止他家拥有太多三元牌
    if ( 1 <= yakuman && yakuman <= 15) {
        using namespace tiles34;
        int white = init.closed().ct(1_y);
        int green = init.closed().ct(2_y);
        int red = init.closed().ct(3_y);
        return white < 1 && green < 1 && red < 1;
    }

     //四喜和，禁止他家拥有太多三元牌
    if ( 16 <= yakuman & yakuman <= 30) {
        using namespace tiles34;
        int east = init.closed().ct(1_f);
        int south = init.closed().ct(2_f);
        int west = init.closed().ct(3_f);
        int north = init.closed().ct(4_f);
        return east < 1 && south < 1 && west < 1 && north < 1;
    }

    //国士无双
    if ( 31 <= yakuman & yakuman <= 45) {
        using namespace tiles34;
        int east = init.closed().ct(1_f);
        int south = init.closed().ct(2_f);
        int west = init.closed().ct(3_f);
        int north = init.closed().ct(4_f);
        int white = init.closed().ct(1_y);
        int green = init.closed().ct(2_y);
        int red = init.closed().ct(3_y);
        int p1 = init.closed().ct(1_p);
        int p9 = init.closed().ct(9_p);
        int s1 = init.closed().ct(1_s);
        int s9 = init.closed().ct(9_s);
        int m1 = init.closed().ct(1_m);
        int m9 = init.closed().ct(9_m);
        return east <= 2 && south <= 2 && west <= 2 && north <= 2 && white <= 2 && green <= 2 && red <= 2 && p1 <= 2 && p9 <= 2 && s1 <= 2 && s9 <= 2 && m1 <= 2 && m9 <= 2;
    }

    //绿一色
    if ( 46 <= yakuman & yakuman <= 60) {
        using namespace tiles34;
        int s2 = init.closed().ct(2_s);
        int s3 = init.closed().ct(3_s);
        int s4 = init.closed().ct(4_s);
        int s6 = init.closed().ct(6_s);
        int s8 = init.closed().ct(8_s);
        int green = init.closed().ct(2_y);
        return s2 <= 1 && s3 <= 1 && s4 <= 1 && s6 <= 1 && s8 <= 2 && green <= 1;
    }

    //清老头
    if ( 61 <= yakuman & yakuman <= 75) {
        using namespace tiles34;
        int p1 = init.closed().ct(1_p);
        int p9 = init.closed().ct(9_p);
        int s1 = init.closed().ct(1_s);
        int s9 = init.closed().ct(9_s);
        int m1 = init.closed().ct(1_m);
        int m9 = init.closed().ct(9_m);
        return p1 < 1 && p9 <= 1 && s1 <= 1 && s9 < 1 && m1 < 1 && m9 < 1;
    }

    //字一色（七对）
    if ( 76 <= yakuman & yakuman <= 90) {
        using namespace tiles34;
        int east = init.closed().ct(1_f);
        int south = init.closed().ct(2_f);
        int west = init.closed().ct(3_f);
        int north = init.closed().ct(4_f);
        int white = init.closed().ct(1_y);
        int green = init.closed().ct(2_y);
        int red = init.closed().ct(3_y);
        return east <= 2 && south <= 2 && west <= 2 && north <= 2 && white <= 2 && green <= 2 && red <= 2;
    }






    return true;
}


void Rio::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;
    using namespace tiles34;


    if (1 <= yakuman & yakuman <= 15){
             using namespace tiles34;
        for (T34 t : { 1_y, 2_y, 3_y })
            exists[mSelf.index()].inc(T34(t), 360);
        }

    if (yakuman >= 16 & yakuman <= 30){
             using namespace tiles34;
        for (T34 i : { 1_f, 2_f, 3_f, 4_f })
            exists[mSelf.index()].inc(T34(i), 280);
        }


    if (31 <= yakuman & yakuman <= 45){
             using namespace tiles34;
        for (T34 t : { 1_s, 9_s, 1_p, 9_p, 1_m, 9_m, 1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y })
            exists[mSelf.index()].inc(T34(t), 360);
        }


    if (46 <= yakuman & yakuman <= 60){
             using namespace tiles34;
        for (T34 t : { 2_s, 3_s, 4_s, 6_s, 8_s, 2_y })
            exists[mSelf.index()].inc(T34(t), 360);
        }


    if (61 <= yakuman & yakuman <= 75){
             using namespace tiles34;
        for (T34 t : { 1_s, 9_s, 1_p, 9_p, 1_m, 9_m })
            exists[mSelf.index()].inc(T34(t), 360);
        }


    if (76 <= yakuman & yakuman <= 90){
             using namespace tiles34;
        for (T34 t : { 1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y })
            exists[mSelf.index()].inc(T34(t), 360);
        }



}



void Rio::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) rinshan;
    const Hand &hand = table.getHand(mSelf);
    const auto &barks = hand.barks();
    const FormCtx &ctx = table.getFormCtx(mSelf);
    const Rule &rule = table.getRule();
    const auto &drids = mount.getDrids();
    if (hand.ready() & who == mSelf) {
        for (T34 t : hand.effA()) {
            Form f(hand, T37(t.id34()), ctx, rule, drids);
            int ronHan = f.han();
            bool ok = ronHan >= 11;
            mount.lightA(t, ok ?  1000 :  -100);
        }

}
    //开挂时候全场降雀力，测试用参数，实装请调整
    if (who != mSelf & 1 <= yakuman & yakuman <=105){
        accelerate(mount, table.getHand(who), table.getRiver(who), -20);
    }

    //开挂时候自己加雀力，测试用参数，实装请调整
    if (who == mSelf & 1 <= yakuman & yakuman <=105)
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 10);
    else
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), -10);




    //大三元
    if (who == mSelf & 1 <= yakuman & yakuman <= 15) {
             using namespace tiles34;
        for (T34 t : { 1_y, 2_y, 3_y }){
            const TileCount &closed = hand.closed();
            if (closed.ct(t) < 3)
                mount.lightA(t, 500);
    accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 80);

    }
    }

    //四喜和
    if (who == mSelf & yakuman >= 16 & yakuman <= 30) {
             using namespace tiles34;
        for (T34 t : { 1_f, 2_f, 3_f, 4_f }){
            const TileCount &closed = hand.closed();
            if (closed.ct(t) < 3)
                mount.lightA(t, 650);
     accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 50);

    }
    }
    if (who != mSelf & 16 <= yakuman & yakuman <= 30) {
             using namespace tiles34;
        for (T34 t : { 1_f, 2_f, 3_f, 4_f }){
            const TileCount &closed = hand.closed();
                mount.lightA(t, -50);


    }
    }

    //国士
    if (who == mSelf & 31 <= yakuman & yakuman <= 45) {
             using namespace tiles34;
        for (T34 t : { 1_s, 9_s, 1_p, 9_p, 1_m, 9_m, 1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y }){
            const TileCount &closed = hand.closed();
            if (closed.ct(t) < 1)
                mount.lightA(t, 750);


    }
    }

    if (who != mSelf & 31 <= yakuman & yakuman <= 45) {
             using namespace tiles34;
        for (T34 t : { 1_s, 9_s, 1_p, 9_p, 1_m, 9_m, 1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y }){
            const TileCount &closed = hand.closed();
                mount.lightA(t, -50);


    }
    }

    //绿一色
    if (who == mSelf & 46 <= yakuman & yakuman <= 60) {
        const TileCount &closed = hand.closed();
             using namespace tiles34;
        for (T34 t : { 2_s, 3_s, 4_s, 6_s, 8_s, 2_y }){
            if (closed.ct(t) < 3)
                mount.lightA(t, 500);
    }
         using namespace tiles34;
        for (T34 t : { 2_y }){
            if (closed.ct(t) < 2)
                mount.lightA(t, 800);


    }
    }

    //清老头
    if (who == mSelf & 61 <= yakuman & yakuman <= 75) {
             using namespace tiles34;
        for (T34 t : { 1_s, 9_s, 1_p, 9_p, 1_m, 9_m }){
            const TileCount &closed = hand.closed();
            if (closed.ct(t) < 2)
                mount.lightA(t, 800);
            if (closed.ct(t) < 3)
                mount.lightA(t, 400);

        }

    }


    //字一色
    if (who == mSelf & 76 <= yakuman & yakuman <= 90) {
             using namespace tiles34;
        for (T34 t : { 1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y }){
            const TileCount &closed = hand.closed();
            if (closed.ct(t) < 2)
                mount.lightA(t, 700);

    }
    }

    //四暗刻
    if (who == mSelf & 91 <= yakuman & yakuman <= 105) {
        for (T34 t : tiles34::ALL34) {
            const TileCount &closed = hand.closed();
            if (closed.ct(t) == 1)
                mount.lightA(t, 400);
            if (closed.ct(t) == 2)
                mount.lightA(t, 750);

            }
        }
    }



void Yui::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan)
        return;

    const Hand &hand = table.getHand(who);
    if (hand.barks().size() > 0)
        return;

    const int turn = table.getRiver(mSelf).size();
    const int s7 = hand.step7();

    auto morePairs = [&hand, &mount](int mk) {
        for (T34 t : tiles34::ALL34)
            if (hand.closed().ct(t) == 1)
                mount.lightA(t, mk);
    };

    if (turn < 6 && s7 >= 3)
        morePairs(70);

    if (turn >= 6 && s7 <= 2) {
        accelerate(mount, hand, table.getRiver(mSelf), 100);
        morePairs(100);
    }
}



} // namespace saki


