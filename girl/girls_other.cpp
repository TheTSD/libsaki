#include "girls_other.h"
#include "../table/table.h"
#include "../util/misc.h"
#include "../unit/tile.h"
#include "../table/table_view_hand.h"
#include "../table/princess.h"
#include<time.h>


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



void Rio::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    (void) rinshan;

    if (who != mSelf)
        return;

    const TableEnv &env = table.getEnv();
    int hour24 = env.hour24();

    if (5 <= hour24 && hour24 <= 9)
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 130);
}



void Kaori::onDice(util::Rand &rand, const Table &table, Choices &choices)
{
    (void) choices;
    using namespace std;
    int i =  rand.gen()%(105-1+1)+1;;
    yakuman = i;


}



bool Kaori::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{

    (void) princess;


    //四暗刻配牌挂，配3组以上对子
    if (yakuman==7){
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
    if ( yakuman==1) {
        using namespace tiles34;
        int white = init.closed().ct(1_y);
        int green = init.closed().ct(2_y);
        int red = init.closed().ct(3_y);
        return white < 1 && green < 1 && red < 1;
    }

     //四喜和，禁止他家拥有太多三元牌
    if ( yakuman==2) {
        using namespace tiles34;
        int east = init.closed().ct(1_f);
        int south = init.closed().ct(2_f);
        int west = init.closed().ct(3_f);
        int north = init.closed().ct(4_f);
        return east < 1 && south < 1 && west < 1 && north < 1;
    }

    //国士无双
    if ( yakuman==3 ) {
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
    if ( yakuman==4 ) {
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
    if ( yakuman==5 ) {
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
    if ( yakuman==6 ) {
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


void Kaori::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;
    using namespace tiles34;


    if (yakuman==1){
             using namespace tiles34;
        for (T34 t : { 1_y, 2_y, 3_y })
            exists[mSelf.index()].inc(T34(t), 360);
        }

    if (yakuman==2){
             using namespace tiles34;
        for (T34 i : { 1_f, 2_f, 3_f, 4_f })
            exists[mSelf.index()].inc(T34(i), 280);
        }


    if (yakuman==3){
             using namespace tiles34;
        for (T34 t : { 1_s, 9_s, 1_p, 9_p, 1_m, 9_m, 1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y })
            exists[mSelf.index()].inc(T34(t), 360);
        }


    if (yakuman==4){
             using namespace tiles34;
        for (T34 t : { 2_s, 3_s, 4_s, 6_s, 8_s, 2_y })
            exists[mSelf.index()].inc(T34(t), 360);
        }


    if (yakuman==5){
             using namespace tiles34;
        for (T34 t : { 1_s, 9_s, 1_p, 9_p, 1_m, 9_m })
            exists[mSelf.index()].inc(T34(t), 360);
        }


    if (yakuman==6){
             using namespace tiles34;
        for (T34 t : { 1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y })
            exists[mSelf.index()].inc(T34(t), 360);
        }



}



void Kaori::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
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
    if (who != mSelf & 1 <= yakuman & yakuman <=7){
        accelerate(mount, table.getHand(who), table.getRiver(who), -20);
    }

    //开挂时候自己加雀力，测试用参数，实装请调整
    if (who == mSelf & 1 <= yakuman & yakuman <=7)
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 10);
    else
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), -10);




    //大三元
    if (who == mSelf & yakuman==1) {
             using namespace tiles34;
        for (T34 t : { 1_y, 2_y, 3_y }){
            const TileCount &closed = hand.closed();
            if (closed.ct(t) < 3)
                mount.lightA(t, 500);
    accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 80);

    }
    }

    //四喜和
    if (who == mSelf & yakuman==2) {
             using namespace tiles34;
        for (T34 t : { 1_f, 2_f, 3_f, 4_f }){
            const TileCount &closed = hand.closed();
            if (closed.ct(t) < 3)
                mount.lightA(t, 650);
     accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 50);

    }
    }
    if (who != mSelf & yakuman==2) {
             using namespace tiles34;
        for (T34 t : { 1_f, 2_f, 3_f, 4_f }){
            const TileCount &closed = hand.closed();
                mount.lightA(t, -50);


    }
    }

    //国士
    if (who == mSelf & yakuman==3) {
             using namespace tiles34;
        for (T34 t : { 1_s, 9_s, 1_p, 9_p, 1_m, 9_m, 1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y }){
            const TileCount &closed = hand.closed();
            if (closed.ct(t) < 1)
                mount.lightA(t, 750);


    }
    }

    if (who != mSelf & yakuman==3) {
             using namespace tiles34;
        for (T34 t : { 1_s, 9_s, 1_p, 9_p, 1_m, 9_m, 1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y }){
            const TileCount &closed = hand.closed();
                mount.lightA(t, -50);


    }
    }

    //绿一色
    if (who == mSelf & yakuman==4) {
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
    if (who == mSelf & yakuman==5) {
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
    if (who == mSelf & yakuman==6) {
             using namespace tiles34;
        for (T34 t : { 1_f, 2_f, 3_f, 4_f, 1_y, 2_y, 3_y }){
            const TileCount &closed = hand.closed();
            if (closed.ct(t) < 2)
                mount.lightA(t, 700);

    }
    }

    //四暗刻
    if (who == mSelf & yakuman==7) {
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




void Kei::onDice(util::Rand &rand, const Table &table, Choices &choices)
{
    (void) choices;
    using namespace std;

        int t = rand.gen()%(2-0+1)+0;
        type = 2;
        int c = rand.gen()%(2-0+1)+0;
          itcolour = c;
        int z = rand.gen()%(9-1+1)+1;
          midp = z;


}

bool Kei::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{


    (void) princess;

    if (challenged > 10)
            return true;

    if(fortune == 0)
        return true;


        if(4<= fortune <= 5 && type == 0){
            if(who == mSelf)
                for (T34 t : tiles34::ALL34)
                    if (init.closed().ct(t) > 3)
                        return false;

                int PairCt = std::count_if(tiles37::ORDER37.begin(), tiles37::ORDER37.end(),
                                              [&init](T34 t) { return init.closed().ct(t) == 2; });
                return PairCt >= 4;

}
                else if(who == mSelf || iter > 100)
                return true;

        //三色同刻保险
        if(fortune == 6){
            using namespace tiles34;
            int t = midp;
            using namespace std;
            int ppCt = init.closed().ct(T34(t,Suit::P));
            int spCt = init.closed().ct(T34(t,Suit::P));
            int mpCt = init.closed().ct(T34(t,Suit::P));
            return ppCt == 0 && spCt == 0 && mpCt == 0;
            }


}


void Kei::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;
    using namespace tiles37;
    using namespace tiles34;



    //一气向配牌
    if (4<= fortune && fortune <= 5 && type == 1  && itcolour == 0)
        for (Suit s : { Suit::P })
            for (int i = 1; i < 9; i++)
                exists[mSelf.index()].inc(T34(s, i), 50);

    if (4<= fortune && fortune <= 5 && type == 2  && itcolour == 1)
        for (Suit s : { Suit::S })
            for (int i = 1; i < 9; i++)
                exists[mSelf.index()].inc(T34(s, i), 50);

    if (4<= fortune && fortune <= 5 && type == 2  && itcolour == 2)
        for (Suit s : { Suit::M })
            for (int i = 1; i < 9; i++)
                exists[mSelf.index()].inc(T34(s, i), 50);

    //断幺向配牌
    if(fortune >= 1 && fortune <= 3 && type == 1){
        using namespace tiles34;
   for (T34 t : YAO13)
       exists[mSelf.index()].inc(T34(t), -50);

        }

    //红宝向配牌
    if(fortune >= 1 && fortune <= 3 && type == 2){
        using namespace tiles37;
        for (Suit s : { Suit::M,Suit::P,Suit::P })
       exists[mSelf.index()].inc(T37(s,0), 50);
        for (Suit s : { Suit::M,Suit::P,Suit::S })
        exists[mSelf.index()].inc(T37(s,0), 50);
        for (Suit s : { Suit::M,Suit::P,Suit::M })
        exists[mSelf.index()].inc(T37(s,0), 50);

        }

    //三色同刻向配牌
    if(fortune >= 6 && fortune <= 7 && type == 0){
        int t =midp;
        for (Suit s : { Suit::M,Suit::P,Suit::P })
         exists[mSelf.index()].inc(T34(s, t), 100);
        for (Suit s : { Suit::P })
         exists[mSelf.index()].inc(T34(s, t), 100);
        for (Suit s : { Suit::S })
         exists[mSelf.index()].inc(T34(s, t), 100);


        }

    //混老头向配牌
    if(fortune >= 6 && fortune <= 7 && type == 1){
        for (T34 t : YAO13)
            exists[mSelf.index()].inc(T34(t), 150);


        }

    //混一色向配牌
    if (5<= fortune && fortune <= 7 && type == 2  && itcolour == 0)
        for (Suit s : { Suit::P })
            for (int i = 1; i < 9; i++)
                exists[mSelf.index()].inc(T34(s, i), 80);

    if (5<= fortune && fortune <= 7 && type == 2  && itcolour == 1)
        for (Suit s : { Suit::S })
            for (int i = 1; i < 9; i++)
                exists[mSelf.index()].inc(T34(s, i), 80);

    if (5<= fortune && fortune <= 7 && type == 2  && itcolour == 2)
        for (Suit s : { Suit::M })
            for (int i = 1; i < 9; i++)
                exists[mSelf.index()].inc(T34(s, i), 80);

    //清一色向配牌
    if (5<= fortune && fortune <= 7 && type == 2  && itcolour == 0)
        for (Suit s : { Suit::P })
            for (int i = 1; i < 9; i++)
                exists[mSelf.index()].inc(T34(s, i), 90);

    if (5<= fortune && fortune <= 7 && type == 2  && itcolour == 1)
        for (Suit s : { Suit::S })
            for (int i = 1; i < 9; i++)
                exists[mSelf.index()].inc(T34(s, i), 90);

    if (5<= fortune && fortune <= 7 && type == 2  && itcolour == 2)
        for (Suit s : { Suit::M })
            for (int i = 1; i < 9; i++)
                exists[mSelf.index()].inc(T34(s, i), 90);

    }


void Kei::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{


    (void) rinshan;
    const Hand &myHand = table.getHand(mSelf);
    const Hand &uhand = table.getHand(luckdown);
    const auto &barks = myHand.barks();
    const FormCtx &ctx = table.getFormCtx(mSelf);
    const Rule &rule = table.getRule();
    const auto &drids = mount.getDrids();


       //多处染挂，提前判断颜色
        const Hand &hand = table.getHand(mSelf);
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

        Suit full = mps[it - sums.begin()];





    if ((who != mSelf  & challenged == 0) || rinshan)
        return;


    if(who == mSelf){
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 25);
        if(challenged == 1)
        accelerate(mount, table.getHand(mSelf), table.getRiver(mSelf), 30*fortune);
        }


    if(who == luckdown && challenged == 1){
        accelerate(mount, table.getHand(luckdown), table.getRiver(luckdown), (-10)*fortune);
    }

    if (challenged == 1 && myHand.ready() & uhand.step() <= 1 & who == luckdown) { //
                    for (T34 t : myHand.effA()) {
                    mount.lightA(t, 1000);
                    mount.lightB(t, 200);
                }


    }

    //三暗刻强烈倾向
    {
    const Hand &hand = table.getHand(mSelf);
    const TileCount &closed = hand.closed();

    static const auto isAnko = [](const M37 &m) {
        return m.type() == M37::Type::DAIMINKAN
                || m.type() == M37::Type::KAKAN;
    };

    if(who == mSelf && 4<= fortune && fortune <= 5 && type == 0 && !myHand.ready()){
    int AnkoCt = std::count_if(barks.begin(), barks.end(), isAnko);
        int needAnko = 3 - AnkoCt;
            using namespace tiles34;
            if (needAnko > 0)
            for (T34 t : ALL34)
                if (closed.ct(t) == 1)
                    mount.lightA(t, 300);
            for (T34 t : ALL34)
                if (closed.ct(t) == 2)
                    mount.lightA(t, 800);
            if (needAnko = 0)
                using namespace tiles34;
                for (T34 t : ALL34)
                    if (closed.ct(t) > 2)
                        mount.lightA(t, -800);
            }

}


   //一气强烈倾向

    if(who == mSelf && 4<= fortune && fortune <= 5 && type == 1  && !myHand.ready()){
        const Hand &hand = table.getHand(mSelf);
        const TileCount &closed = hand.closed();
                for (int i = 1; i < 9; i++) {
                    T34 t(full, i);
                    const TileCount &closed = myHand.closed();
                    if (closed.ct(t) < 1)
                        mount.lightA(t, 800);
                    if (closed.ct(t) > 1)
                        mount.lightA(t, -800);
    }
    }




    //七对倾向

     if(who == mSelf && 4<= fortune && fortune <= 5 && type == 2  && !myHand.ready()){
         const Hand &hand = table.getHand(mSelf);
         const TileCount &closed = hand.closed();
         using namespace tiles34;
         for (T34 t : ALL34)
             if (closed.ct(t) == 1)
                 mount.lightA(t, 150*fortune);

     }

     //断幺倾向

      if(who == mSelf && 1<= fortune && fortune <= 3 && type == 1  && !myHand.ready()){
          const Hand &hand = table.getHand(mSelf);
          const TileCount &closed = hand.closed();
          using namespace tiles34;
          for (T34 t : YAO13)
                  mount.lightA(t, fortune*-30);

      }


    //三色同刻版进章
    if(who == mSelf && fortune >= 6 && fortune <= 7 && type == 0){
        const Hand &hand = table.getHand(mSelf);
        const TileCount &closed = hand.closed();
        for (Suit s : { Suit::M, Suit::P, Suit::S }) {
           T34 t(s, midp);
           if (closed.ct(t) == 0)
              mount.lightA(t, 200*fortune);
           if (closed.ct(t) == 1)
              mount.lightA(t, 300*fortune);
           if (closed.ct(t) == 0)
              mount.lightA(t, 400*fortune);
            }
    }
        if(who != mSelf && fortune >= 6 && fortune <= 7){
            int v = midp;
            for (Suit s : { Suit::M, Suit::P, Suit::S }) {
               T34 t(s, v);
                  mount.lightA(t, (-50)*fortune);
            }
    }

        //混老头向进章
        if(who == mSelf && fortune >= 6 && fortune <= 7 && type == 1 && !myHand.ready()){
            const Hand &hand = table.getHand(mSelf);
            const TileCount &closed = hand.closed();
            using namespace tiles34;
            for (T34 t : YAO13){
               if (closed.ct(t) == 0)
                  mount.lightA(t, 200*fortune);
               if (closed.ct(t) == 1)
                  mount.lightA(t, 300*fortune);
               if (closed.ct(t) == 2)
                  mount.lightA(t, 200*fortune);
                }

                }

        //混一色倾向

         if(who == mSelf && 6<= fortune && fortune <= 7 && type == 2 && !myHand.ready()){
                     for (int i = 1; i < 9; i++) {
                         T34 t(full, i);
                         const TileCount &closed = myHand.closed();
                         if (closed.ct(t) < 1)
                             mount.lightA(t, 200*fortune);
                         if (closed.ct(t) > 1)
                             mount.lightA(t, 100*fortune);


            }

}
         //清一色倾向

          if(who == mSelf && fortune >= 8 && !myHand.ready()){
                      for (int i = 1; i < 9; i++) {
                          T34 t(full, i);
                              mount.lightA(t, 200*fortune);
                              if(type == 0 && closed.ct(t) == 0)
                              mount.lightA(t, 250*fortune);
                              if(type == 1 && closed.ct(t) == 1)
                              mount.lightA(t, 250*fortune);
                              if(type == 2 && closed.ct(t) == 2)
                              mount.lightA(t, 250*fortune);
                      }





             }


}

void Kei::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                        std::bitset<Girl::NUM_NM_SKILL> &presence,
                        const Princess &princess)
{
    (void) rand;
    //起手役牌
    if (1<= fortune && fortune <= 3 && type == 0) {
        int wh = princess.getTable().getSelfWind(mSelf);
        int yh = rand.gen()%(3-1+1)+1;
        using namespace tiles37;
        for (Suit s : { Suit::F }) {
                T34 t(s, wh);
        if (mount.remainA(T37(wh, s)) >= 2) {
            init.inc(mount.initPopExact(T37(wh, s)), 1);
            init.inc(mount.initPopExact(T37(wh, s)), 1);
        }
        }
        using namespace tiles37;
        for (Suit s : { Suit::Y }) {
                T34 t(s, yh);
        if (mount.remainA(T37(yh, s)) >= 2) {
            init.inc(mount.initPopExact(T37(yh, s)), 1);
            init.inc(mount.initPopExact(T37(yh, s)), 1);
        }
    }
    }


    //三色同刻双保险
    if (fortune >= 6 && fortune <= 7 && type == 0) {
        using namespace tiles37;
        int v = midp;
        if (mount.remainA(T37(v, Suit::P)) >= 2) {
            for (Suit s : { Suit::P }) {
                    T34 t(s, v);
            init.inc(mount.initPopExact(T37(v, s)), 1);
            init.inc(mount.initPopExact(T37(v, s)), 1);
        }
        }
        if (mount.remainA(T37(v, Suit::S)) >= 2) {
            if (mount.remainA(T37(v, Suit::S)) >= 2) {
                for (Suit s : { Suit::S }) {
                        T37 t(s, v);
            init.inc(mount.initPopExact(T37(v, s)), 1);
            init.inc(mount.initPopExact(T37(v, s)), 1);
        }
            }
        }
        if (mount.remainA(T37(v, Suit::M)) >= 2) {
            if (mount.remainA(T37(v, Suit::M)) >= 2) {
                for (Suit s : { Suit::M }) {
                        T37 t(s, v);
            init.inc(mount.initPopExact(T37(v, s)), 1);
            init.inc(mount.initPopExact(T37(v, s)), 1);
        }
    }
}
        }

    //混一色起手役牌
    if (6<= fortune && fortune <= 7 && type == 2) {
        int zh = rand.gen()%(33-27+1)+27;
        using namespace tiles37;
                T37 t(zh);
        if (mount.remainA(T37(zh)) >= 3) {
            init.inc(mount.initPopExact(T37(zh)), 1);
            init.inc(mount.initPopExact(T37(zh)), 1);
            init.inc(mount.initPopExact(T37(zh)), 1);
        }


    }
}


void Kei::onRoundEnded(const Table &table, RoundResult result,
                        const std::vector<Who> &openers, Who gunner,
                        const std::vector<Form> &fs)
{
    (void) table;
    (void) gunner;


    if (result == RoundResult::TSUMO && openers[0] == mSelf){
      challenged = 0;
      fortune = 0;
      luckdown = mSelf;
    } else if (result == RoundResult::RON && openers[0] == mSelf) {
        challenged = 0;
        fortune = 0;
        luckdown = mSelf;
    }


      if (result == RoundResult::TSUMO && openers[0] != mSelf) {
          challenged = 1;
          if(fortune < fs[0].han()+fs[0].dora())
          fortune = fs[0].han()+fs[0].dora();
          if(fortune > fs[0].han()+fs[0].dora())
          fortune = fortune + 1;
          luckdown = openers[0];
      } else if (result == RoundResult::RON && openers[0] != mSelf) {
          challenged = 1;
          if(fortune < fs[0].han()+fs[0].dora())
          fortune = fs[0].han()+fs[0].dora();
          if(fortune > fs[0].han()+fs[0].dora())
          fortune = fortune + 1;
          luckdown = openers[0];
}

}


} // namespace saki



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


