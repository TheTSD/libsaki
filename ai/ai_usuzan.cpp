#include "ai_usuzan.h"
#include "../girl/girls_usuzan.h"

#include <algorithm>



namespace saki
{



Action AiSawaya::forward(const TableView &view)
{
    if (view.myChoices().can(ActCode::IRS_CHECK)) {
        unsigned mask = 0;
        if (view.getDealer() == mSelf) {
            const Sawaya &girl = static_cast<const Sawaya&>(view.me());
            assert(girl.getId() == Girl::Id::SHISHIHARA_SAWAYA);

            // confusing code. change it someday...
            girl.canUseRedCloud(mask)
                    || girl.canUseWhiteCloud(mask);
        }
        return Action(ActCode::IRS_CHECK, mask);
    } else {
        return Action();
    }
}

Action AiSawaya::think(const TableView &view, Limits &limits)
{
    const Sawaya &girl = static_cast<const Sawaya&>(view.me());
    assert(girl.getId() == Girl::Id::SHISHIHARA_SAWAYA);

    if (!girl.usingRedCloud() || view.getRiver(mSelf).size() > 3)
        return Ai::think(view, limits);

    // reserve Z when using red cloud
    for (T34 z : tiles34::Z7)
        limits.addNoOut(z);

    return Ai::think(view, limits);
}



} // namespace saki


