#ifndef SAKI_GIRLS_OTHER_H
#define SAKI_GIRLS_OTHER_H

#include "girl.h"



namespace saki
{



class Kazue : public Girl
{
public:
    GIRL_CTORS(Kazue)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
};



class Uta : public Girl
{
public:
    GIRL_CTORS(Uta)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

private:
    void power3sk(const Hand &hand, const River &river, Mount &mount);
    bool tryPowerDye(const Hand &hand, Mount &mount);
};



class Rio : public Girl
{
public:
    GIRL_CTORS(Rio)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

};



class Kaori : public Girl
{
public:
    GIRL_CTORS(Kaori)
    void onDice(util::Rand &rand, const Table &table, Choices &choices) override;
    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    int yakuman;

};



class Yui : public Girl
{
public:
    GIRL_CTORS(Yui)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

};


class Kei : public Girl
{
public:
    GIRL_CTORS(Kei)
    void onDice(util::Rand &rand, const Table &table, Choices &choices) override;
    bool checkInit(Who who, const Hand &init, const Princess &princess, int iter) override;
    void onMonkey(std::array<Exist, 4> &exists, const Princess &princess) override;
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;
    void onRoundEnded(const Table &table, RoundResult result,
                      const std::vector<Who> &openers, Who gunner,
                      const std::vector<Form> &fs) override;
    void nonMonkey(util::Rand &rand, TileCount &init, Mount &mount,
                            std::bitset<Girl::NUM_NM_SKILL> &presence,
                            const Princess &princess) override;
    int challenged = 0;
    int fortune = 0;
    int itcolour;
    int midp;
    int type;
    private:
    Who luckdown = mSelf;
};



} // namespace saki



#endif // SAKI_GIRLS_OTHER_H



public:
    GIRL_CTORS(Yui)
    void onDraw(const Table &table, Mount &mount, Who who, bool rinshan) override;

};



} // namespace saki



#endif // SAKI_GIRLS_OTHER_H


