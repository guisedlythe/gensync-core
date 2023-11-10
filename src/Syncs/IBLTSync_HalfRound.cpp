#include <GenSync/Syncs/IBLTSync_HalfRound.h>
#include <GenSync/Benchmarks/BenchParams.h>

std::shared_ptr<Params> IBLTSync_HalfRound::getParams() const {
    return std::make_shared<IBLTParams>(getExpNumElems(), getElementSize());
}

std::shared_ptr<Params>
IBLTSync_HalfRoundProtocol::readParams(std::istream &is) const {
    auto par = make_shared<IBLTParams>();
    is >> *par;
    return par;
}

std::shared_ptr<SyncMethod> IBLTSync_HalfRoundProtocol::makeSyncMethod(
    const SyncParameters &syncParams) const {
    return std::make_shared<IBLTSync_HalfRound>(syncParams.numExpElem,
                                                syncParams.bits);
}
