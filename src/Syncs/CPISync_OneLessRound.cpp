#include <GenSync/Syncs/CPISync_OneLessRound.h>
#include <GenSync/Benchmarks/BenchParams.h>

std::shared_ptr<Params> CPISync_OneLessRound::getParams() const {
    return std::make_shared<CPISyncParams>(getMaxDiff(), getBits(),
                                           getProbEps(), getHashes());
}

std::shared_ptr<Params>
CPISync_OneLessRoundProtocol::readParams(std::istream &is) const {
    auto par = make_shared<CPISyncParams>();
    is >> *par;
    return par;
}

std::shared_ptr<SyncMethod> CPISync_OneLessRoundProtocol::makeSyncMethod(
    const SyncParameters &syncParams) const {
    if (syncParams.mbar.isNullQ())
        throw std::invalid_argument(
            "Must define <mbar> explicitly for CPISync_OneLessRound.");
    return std::make_shared<CPISync_OneLessRound>(
        syncParams.mbar, syncParams.bits, syncParams.errorProb);
}
