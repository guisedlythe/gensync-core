#ifndef SYNC_PROTOCOL_H
#define SYNC_PROTOCOL_H

#include <istream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct Params;
class SyncMethod;
struct SyncParameters;

class SyncProtocol {
  public:
    virtual std::string getName() const = 0;

    virtual std::shared_ptr<Params> readParams(std::istream &is) const = 0;

    virtual std::shared_ptr<SyncMethod>
    makeSyncMethod(const SyncParameters &syncParams) const = 0;
};

class SyncProtocolRegistry {
  public:
    void registerDefault();

    void registerSyncProtocol(const std::shared_ptr<SyncProtocol> &syncProtocol,
                              const std::string &name);

    template <typename T, typename... Args>
    void registerSyncProtocol(const std::shared_ptr<SyncProtocol> &syncProtocol,
                              const std::string &name, T &&arg,
                              Args &&...args) {
        registerSyncProtocol(syncProtocol, name);
        registerSyncProtocol(syncProtocol, std::forward<T>(arg),
                             std::forward<Args>(args)...);
    }

    std::shared_ptr<SyncProtocol>
    getSyncProtocol(const std::string &name) const {
        const auto it = syncProtocols.find(name);
        if (it != syncProtocols.end())
            return it->second;
        return nullptr;
    }

    static std::vector<std::pair<std::string, std::shared_ptr<SyncProtocol>>>
    GetDefaultSyncProtocols();

  private:
    std::unordered_map<std::string, std::shared_ptr<SyncProtocol>>
        syncProtocols;
};

#endif
