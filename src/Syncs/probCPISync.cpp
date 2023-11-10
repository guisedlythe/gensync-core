/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
#include "GenSync/Benchmarks/BenchParams.h"
#include <NTL/RR.h>

// project libraries
#include <GenSync/Aux/SyncMethod.h>
#include <GenSync/Syncs/CPISync.h>
#include <GenSync/Syncs/ProbCPISync.h>

ProbCPISync::ProbCPISync(long m_bar, long bits, int epsilon,bool hashes) :
CPISync(m_bar, bits, epsilon + (int) ceil(log(bits) / log(2)), 0, hashes) // adding lg(b) gives an equivalent probability of error for GenSync
{

  // tweak parameters of GenSync for probabilistic implementation
   probEps = epsilon; // this was the designed probability of error
   probCPI = true; // switches some code in the GenSync superclass
   currDiff = 1;

}

std::shared_ptr<Params> ProbCPISync::getParams() const {
   return std::make_shared<CPISyncParams>(getMaxDiff(), getBits(), getProbEps(),
                                          getHashes());
}

std::shared_ptr<Params>
ProbCPISyncProtocol::readParams(std::istream &is) const {
   auto par = make_shared<CPISyncParams>();
   is >> *par;
   return par;
}

std::shared_ptr<SyncMethod>
ProbCPISyncProtocol::makeSyncMethod(const SyncParameters &syncParams) const {
   if (syncParams.mbar.isNullQ())
       throw std::invalid_argument(
           "Must define <mbar> explicitly for ProbCPISync.");
   return std::make_shared<ProbCPISync>(syncParams.mbar, syncParams.bits,
                                        syncParams.errorProb,
                                        syncParams.hashes);
}
