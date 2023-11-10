#include <GenSync/Syncs/CPISync_HalfRound.h>
#include <GenSync/Benchmarks/BenchParams.h>

std::shared_ptr<Params> CPISync_HalfRound::getParams() const {
    return std::make_shared<CPISyncParams>(getMaxDiff(), getBits(),
                                           getProbEps(), getHashes());
}

std::shared_ptr<Params>
CPISync_HalfRoundProtocol::readParams(std::istream &is) const {
    auto par = make_shared<CPISyncParams>();
    is >> *par;
    return par;
}

std::shared_ptr<SyncMethod> CPISync_HalfRoundProtocol::makeSyncMethod(
    const SyncParameters &syncParams) const {
    if (syncParams.mbar.isNullQ())
        throw std::invalid_argument(
            "Must define <mbar> explicitly for CPISync_HalfRound.");
    return std::make_shared<CPISync_HalfRound>(syncParams.mbar, syncParams.bits,
                                               syncParams.errorProb);
}
