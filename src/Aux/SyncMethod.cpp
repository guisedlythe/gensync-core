/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

#include <Gensync/Communicants/Communicant.h>
#include <Gensync/Aux/SyncMethod.h>
#include <Gensync/Aux/Exceptions.h>

SyncMethod::SyncMethod() {
    SyncID = SYNC_TYPE::GenericSync; // synchronization type

    sketches = make_shared<Sketches>(Sketches{Sketches::Types::CARDINALITY,
                                              Sketches::Types::UNIQUE_ELEM,
                                              Sketches::Types::HEAVY_HITTERS}); // initiate sketches
}

SyncMethod::~SyncMethod() = default;

void SyncMethod::SendSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay /* = false */) {
 if (!commSync->establishModSend(oneWay)) // establish ZZ_p modulus - must be first
     throw SyncFailureException("Sync parameters do not match between communicants.");
}

void SyncMethod::RecvSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay /* = false */) {
if (!commSync->establishModRecv(oneWay)) // establish ZZ_p modulus - must be first
      throw SyncFailureException("Sync parameters do not match between communicants.");   
}



