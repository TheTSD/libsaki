#include "girls_eisui.h"
#include "../table/table.h"
#include "../table/princess.h"
#include "../util/misc.h"



namespace saki
{



bool Hatsumi::checkInit(const Table &table, const Who who, const Hand &init, const Princess &princess, int iter)
{
    if (who == mSelf || iter > 100)
        return true;

    if (princess.getTable().getSelfWind(mSelf) == 1) { // 东起，不给他家东
        using namespace tiles34;
        int ctE = init.closed().ct(1_f);
        int dont4 = table.getHand(mSelf).closed().ct(1_f);
        return ctE <= 0 && dont4 == 3;
    }
    if (princess.getTable().getSelfWind(mSelf) == 2) { // 南起，不给他家南
        using namespace tiles34;
        int ctS = init.closed().ct(2_f);
        int dont4 = table.getHand(mSelf).closed().ct(2_f);
        return ctS <= 0 && dont4 == 3;
    }
    if (princess.getTable().getSelfWind(mSelf) == 3) { // 西起，不给他家西
        using namespace tiles34;
        int ctW = init.closed().ct(3_f);
        int dont4 = table.getHand(mSelf).closed().ct(3_f);
        return ctW <= 0 && dont4 == 3;
    }
    if (princess.getTable().getSelfWind(mSelf) == 4) { // 北起，不给他家北
        using namespace tiles34;
        int ctN = init.closed().ct(4_f);
        int dont4 = table.getHand(mSelf).closed().ct(4_f);
        return ctN <= 0 && dont4 == 3;
    }
    if (table.getRound() >= 4) { // 南风圈不给他家起手南对子
        using namespace tiles34;
        int ctN = init.closed().ct(2_f);
        return ctN <= 1;
    }

    return true;
}

void Hatsumi::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (rinshan)
        return;

    if (who != mSelf && table.getRound() >= 4){
        const Hand &hand = table.getHand(who);
             T34 t(Suit::F, 2);
            mount.lightA(t, -40);
                                // 南场他家难摸场风
    }
    if (who == mSelf && table.getRound() >= 4){
        T34 t(Suit::F, 2);
        const TileCount &closed = table.getHand(mSelf).closed();
        if (closed.ct(t) <= 3)
            mount.lightA(t, 325);


       // 南场自己易摸场风,3张为止，无法彻底杜绝摸4张的可能性（极低概率）
     }


        if (who == mSelf){
            const Hand &hand = table.getHand(who);
                 T34 off(Suit::F, table.getSelfWind(mSelf));
                mount.lightA(off, -5000);
                for (T34 t : tiles34::Z7)
                mount.lightA(t, -20);
        }
        // 靠自摸无法摸到第四张自风，常驻的字牌率低下


        const TileCount &closed = table.getHand(mSelf).closed();

        for (T34 t : tiles34::ALL34) {
            int ct = closed.ct(t);
            const std::array<int, 5> deltas { 30, 30, 30, 30, 30 };
            mount.lightA(t, deltas[ct]);
        }// 稳定的不明显直向
}

void Hatsumi::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                        std::bitset<Girl::NUM_NM_SKILL> &presence,
                        const Princess &princess)
{
    (void) rand;
    (void) presence;

    if (princess.getTable().getSelfWind(mSelf) == 1) { // 东家
        using namespace tiles37;
        // 起手3东
        if (mount.remainA(1_f) >= 3) {
            init.inc(mount.initPopExact(1_f), 1);
            init.inc(mount.initPopExact(1_f), 1);
            init.inc(mount.initPopExact(1_f), 1);
        }
        }
     if (princess.getTable().getSelfWind(mSelf) == 2) { // 南家
        using namespace tiles37;
        // 起手3南
         if (mount.remainA(2_f) >= 3) {
             init.inc(mount.initPopExact(2_f), 1);
             init.inc(mount.initPopExact(2_f), 1);
             init.inc(mount.initPopExact(2_f), 1);
        }
        }
     if (princess.getTable().getSelfWind(mSelf) == 3) { // 西家
        using namespace tiles37;
        // 起手3西
         if (mount.remainA(3_f) >= 3) {
             init.inc(mount.initPopExact(3_f), 1);
             init.inc(mount.initPopExact(3_f), 1);
             init.inc(mount.initPopExact(3_f), 1);
        }
        }
     if (princess.getTable().getSelfWind(mSelf) == 4) { // 北家
        using namespace tiles37;
        // 起手3北
         if (mount.remainA(4_f) >= 3) {
             init.inc(mount.initPopExact(4_f), 1);
             init.inc(mount.initPopExact(4_f), 1);
             init.inc(mount.initPopExact(4_f), 1);
         }
        }
    }



void Kasumi::onDice(util::Rand &rand, const Table &table, Choices &choices)
{
    (void) rand;
    (void) table;

    if (mZim.able) {
        mChoicesBackup = choices;
        choices.setCut();
    }
}

bool Kasumi::checkInit(Who who, const Hand &init, const Princess &princess, int iter)
{
    (void) princess;

    if (who != mSelf || iter > 100)
        return true;

    // prevent both too low step and too many Z
    return init.step() >= 2 && (iter > 50 || init.closed().ctZ() <= 5);
}

void Kasumi::onMonkey(std::array<Exist, 4> &exists, const Princess &princess)
{
    (void) princess;

    if (!mZim.on)
        return;

    for (int w = 0; w < 4; w++) {
        for (T34 t : tiles34::ALL34) {
            // isNum && (rivals' 1-lack || self's 2-lack)
            if (t.isNum()
                    && ((t.suit() == mZimSuit && Who(w) != mSelf)
                        || (t.suit() != mZimSuit && Who(w) == mSelf)))
                exists[w].inc(t, -1000);
            if (t.isZ() && Who(w) == mSelf)
                exists[w].inc(t, -20);
        }
    }
}

void Kasumi::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (rinshan || !mZim.on)
        return;

    int turn = table.getRiver(mSelf).size();
    if (turn > 12)
        return;

    if (who == mSelf) {
        Suit s1 = static_cast<Suit>((static_cast<int>(mZimSuit) + 1) % 3);
        Suit s2 = static_cast<Suit>((static_cast<int>(mZimSuit) + 2) % 3);
        const Hand &hand = table.getHand(mSelf);
        int currStep = hand.step();

        for (int v = 1; v <= 9; v++) {
            mount.lightA(T34(s1, v), -1100);
            mount.lightA(T34(s2, v), -1100);

            if (currStep <= 1) {
                T34 t(mZimSuit, v);
                if (!hand.hasEffA(t))
                    mount.lightA(t, 270);
            }
        }
    } else { // rival's draw
        for (int v = 1; v <= 9; v++)
            mount.lightA(T34(mZimSuit, v), -700);
    }
}

Choices Kasumi::forwardAction(const Table &table, Mount &mount, const Action &action)
{
    (void) table;
    (void) mount;
    assert(action.act() == ActCode::IRS_CHECK);

    mZim.on = action.mask() & 0b1;
    if (mZim.on)
        mZim.able = false; // once on, never off

    return mChoicesBackup;
}

const IrsCheckRow &Kasumi::irsCheckRow(int index) const
{
    assert(index == 0);
    return mZim;
}

int Kasumi::irsCheckCount() const
{
    return 1;
}

void Kasumi::nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                       std::bitset<Girl::NUM_NM_SKILL> &presence,
                       const Princess &princess)
{
    (void) init;
    (void) princess;

    if (!mZim.on)
        return;

    std::vector<int> ofss = { 0, 9, 18 };
    if (presence[WHITE_CLOUD])
        ofss.pop_back(); // exclude suit-S

    int door = rand.gen(ofss.size());
    mZimSuit = static_cast<Suit>(door); // ok since popped S is the last
    presence.set(ZIM_M + door); // assuming order of enum
    int begin = ofss[door];

    // break wallability of zim-tiles
    for (int i = 0; i < 9; i++)
        mount.loadB(T37(begin + i), 1);
}



} // namespace saki


