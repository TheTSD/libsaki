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



} // namespace saki



#endif // SAKI_GIRLS_OTHER_H


