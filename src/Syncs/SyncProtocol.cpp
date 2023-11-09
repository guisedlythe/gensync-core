#include <GenSync/Syncs/SyncProtocol.h>

void SyncProtocolRegistry::registerDefault() {
    // registerSyncProtocol(std::make_shared<FullSyncProtocol>(), "FullSync", "7");
}

void SyncProtocolRegistry::registerSyncProtocol(
    const std::shared_ptr<SyncProtocol> &syncProtocol,
    const std::string &name) {
    syncProtocols.emplace(name, syncProtocol);
}
