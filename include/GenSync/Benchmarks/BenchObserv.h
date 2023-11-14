/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on July, 2020.
 */

#ifndef BENCHOBSERV_H
#define BENCHOBSERV_H

#include <vector>
#include <algorithm>
#include <GenSync/Aux/Auxiliary.h>
#include <GenSync/Benchmarks/BenchParams.h>
#include <GenSync/Aux/SyncMethod.h>

using namespace std;

/**
 * Represents the performance observation of a sync run.
 */
struct BenchObserv {
    BenchObserv(const std::shared_ptr<SyncMethod>& syncMethod) : syncMethod(syncMethod) {}

    BenchObserv(const std::shared_ptr<SyncMethod>& syncMethod, const string& stats, bool success, const string& exception) :
        syncMethod(syncMethod), benchResult({stats, success, exception}) {}

    friend ostream& operator<<(ostream& os, const BenchObserv& bo) {
        if(!bo.syncMethod) return os;
        // Some getName() implementations also write its parameters, which could
        // lead to the parameters being written twice.
        // Potential fix would be to make getName() only return the name.
        os << "Sync protocol: " << bo.syncMethod->getName() << "\n";
        if(const auto syncParams = bo.syncMethod->getParams()) {
            os << *syncParams << "\n";
        } else {
            os << "could not recover sync params\n";
        }
        if(const auto sketches = bo.syncMethod->getSketches()) {
           os << *sketches << "\n";
        } else {
            os << "could not recover sketches\n";
        }
        os << FromFileGen::DELIM_LINE << "\n";
        if(!bo.benchResult.isNullQ()) {
            os << "Success: " << (*bo.benchResult).success << " ["
               << (*bo.benchResult).exception << "]"
               << "\n"
               << (*bo.benchResult).stats << "\n";
        }
        return os;
    }

    struct BenchResult {
        string stats;
        bool success;
        string exception;
    };

    const std::shared_ptr<SyncMethod> syncMethod = nullptr;
    const Nullable<BenchResult> benchResult;
};

#endif // BENCHOBSERV_H
