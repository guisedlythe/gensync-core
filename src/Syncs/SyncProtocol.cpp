#include <GenSync/Syncs/SyncProtocol.h>
#include <GenSync/Syncs/CPISync.h>
#include <GenSync/Syncs/CPISync_HalfRound.h>
#include <GenSync/Syncs/CPISync_OneLessRound.h>
#include <GenSync/Syncs/CuckooSync.h>
#include <GenSync/Syncs/FullSync.h>
#include <GenSync/Syncs/IBLTSetOfSets.h>
#include <GenSync/Syncs/IBLTSync.h>
#include <GenSync/Syncs/IBLTSync_HalfRound.h>
#include <GenSync/Syncs/IBLTSync_Multiset.h>
#include <GenSync/Syncs/InterCPISync.h>
#include <GenSync/Syncs/ProbCPISync.h>
#include <string>

void SyncProtocolRegistry::registerDefault() {
    const auto defaultSyncProtocols = GetDefaultSyncProtocols();
    for (auto i = 0; i < defaultSyncProtocols.size(); i++) {
        const auto &protocol = defaultSyncProtocols.at(i);
        // for compatability also add the old enum index as name
        registerSyncProtocol(protocol.second, protocol.first,
                             std::to_string(i + 1));
    }
}

void SyncProtocolRegistry::registerSyncProtocol(
    const std::shared_ptr<SyncProtocol> &syncProtocol,
    const std::string &name) {
    syncProtocols.emplace(name, syncProtocol);
}

std::vector<std::pair<std::string, std::shared_ptr<SyncProtocol>>>
SyncProtocolRegistry::GetDefaultSyncProtocols() {
    // TODO: CPISync_HalfRound and OneWayCPISync are the same, one should be
    // removed
    return {
        std::make_pair("CPISync", std::make_shared<CPISyncProtocol>()),
        std::make_pair("CPISync_OneLessRound",
                       std::make_shared<CPISync_OneLessRoundProtocol>()),
        std::make_pair("CPISync_HalfRound",
                       std::make_shared<CPISync_HalfRoundProtocol>()),
        std::make_pair("ProbCPISync", std::make_shared<ProbCPISyncProtocol>()),
        std::make_pair("InteractiveCPISync",
                       std::make_shared<InterCPISyncProtocol>()),
        std::make_pair("OneWayCPISync",
                       std::make_shared<CPISync_HalfRoundProtocol>()),
        std::make_pair("FullSync", std::make_shared<FullSyncProtocol>()),
        std::make_pair("IBLTSync", std::make_shared<IBLTSyncProtocol>()),
        std::make_pair("OneWayIBLTSync",
                       std::make_shared<IBLTSync_HalfRoundProtocol>()),
        std::make_pair("IBLTSetOfSets",
                       std::make_shared<IBLTSetOfSetsProtocol>()),
        std::make_pair("IBLTSync_Multiset",
                       std::make_shared<IBLTSyncMultisetProtocol>()),
        std::make_pair("CuckooSync", std::make_shared<CuckooSyncProtocol>()),
    };
}
