/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on July, 2020.
 */

#include <cstdio>
#include <regex>
#include <stdexcept>
#include <GenSync/Benchmarks/BenchParams.h>

const char BenchParams::KEYVAL_SEP = ':';
const string BenchParams::FILEPATH_SEP = "/"; // TODO: we currently don't compile for _WIN32!

template<char delimiter>
class DelimitedString : public string {};

/**
 * Extracts data from the next line in input stream. The line is of the form "text: data"
 */
template<typename T>
void getVal(istream& is, T& container) {
    string line;
    getline(is, line);

    stringstream ss(line);
    string segment;
    vector<string> segments;
    while (getline(ss, segment, BenchParams::KEYVAL_SEP))
        segments.push_back(segment);

    istringstream(segments.at(1)) >> std::boolalpha >> container;
}

/**
 * Extract SyncProtocol from the next line in the input stream
 */
inline std::string getProtocolName(const SyncProtocolRegistry& syncRegistry, istream& is) {
    std::string protoStr;
    getVal(is, protoStr);
    return protoStr;
}

ostream& CPISyncParams::serialize(ostream& os) const {
    os << "m_bar: " << m_bar << "\n"
       << "bits: " << bits << "\n"
       << "epsilon: " << epsilon << "\n"
       << "partitions/pFactor(for InterCPISync): " << partitions << "\n"
       << "hashes: " << std::boolalpha << hashes;

    return os;
}

istream& CPISyncParams::unserialize(istream& is) {
    getVal<decltype(m_bar)>(is, m_bar);
    getVal<decltype(bits)>(is, bits);
    getVal<decltype(epsilon)>(is, epsilon);
    getVal<decltype(partitions)>(is, partitions);
    getVal<decltype(hashes)>(is, hashes);

    return is;
}

void CPISyncParams::apply(GenSync::Builder& gsb) const {
    gsb.setBits(bits);
    gsb.setMbar(m_bar);
    gsb.setErr(epsilon);
    gsb.setNumPartitions(partitions);
    gsb.setHashes(hashes);
}

ostream& IBLTParams::serialize(ostream& os) const {
    os << "expected: " << expected << "\n"
       << "eltSize: " << eltSize << "\n"
       << "numElemChild: " << numElemChild;

    return os;
}

istream& IBLTParams::unserialize(istream& is) {
    getVal<decltype(expected)>(is, expected);
    getVal<decltype(eltSize)>(is, eltSize);
    getVal<decltype(numElemChild)>(is, numElemChild);

    return is;
}

void IBLTParams::apply(GenSync::Builder& gsb) const {
    gsb.setExpNumElems(expected);
    gsb.setBits(eltSize);
    gsb.setExpNumElemChild(numElemChild);
}

ostream& CuckooParams::serialize(ostream& os) const {
    os << "fngprtSize: " << fngprtSize << "\n"
       << "bucketSize: " << bucketSize << "\n"
       << "filterSize: " << filterSize << "\n"
       << "maxKicks: " << maxKicks;

    return os;
}

istream& CuckooParams::unserialize(istream& is) {
    getVal<decltype(fngprtSize)>(is, fngprtSize);
    getVal<decltype(bucketSize)>(is, bucketSize);
    getVal<decltype(filterSize)>(is, filterSize);
    getVal<decltype(maxKicks)>(is, maxKicks);

    return is;
}

void CuckooParams::apply(GenSync::Builder& gsb) const {
    gsb.setFngprtSize(fngprtSize);
    gsb.setBucketSize(bucketSize);
    gsb.setFilterSize(filterSize);
    gsb.setMaxKicks(maxKicks);
}

/**
 * Makes the correct Params specialization using the data in the input stream.
 */
inline shared_ptr<Params> decideBenchParams(const std::shared_ptr<SyncProtocol>& syncProtocol, ifstream& is) {
    if(!syncProtocol) return nullptr;
    return syncProtocol->readParams(is);
}

/**
 * Figures out whether the file contains real data or a reference to a
 * data file. If there is reference, it returns the path to the
 * referenced file. Otherwise it returns an empty string.
 *
 * Referenced file path is considered relative to the file we are
 * currently reading.
 */
inline string getReference(istream& is, const string& origFile) {
    string line;
    getline(is, line);
    if (line.compare(FromFileGen::DELIM_LINE) != 0) {
        string msg = "File is malformed.\n";
        Logger::error_and_quit(msg);
    }

    getline(is, line);
    auto pos = line.find(FromFileGen::REFERENCE);
    if (pos != string::npos) {
        stringstream ss(line);
        string segment;
        vector<string> segments;
        while (getline(ss, segment, BenchParams::KEYVAL_SEP))
            segments.push_back(segment);

        string refFName = segments.at(1);

        // trim leading and trailing spaces
        refFName = std::regex_replace(refFName, std::regex("^ +| +$"), "");

        // build the relative path to the referenced file
        auto origFNameLen = origFile.substr(origFile.find_last_of(BenchParams::FILEPATH_SEP)).length();
        string pref = origFile.substr(0, origFile.length() - origFNameLen);
        stringstream prefs;
        prefs << pref << BenchParams::FILEPATH_SEP << refFName;
        return prefs.str();
    }

    return "";
}

BenchParams::BenchParams(const string& fName, const SyncProtocolRegistry& syncRegistry) {
    ifstream is(fName);
    if (!is.is_open()) {
        stringstream ss;
        ss << "File " << fName << " not found";
        throw runtime_error(ss.str());
    }

    const auto protocolName = getProtocolName(syncRegistry, is);
    syncProtocol = syncRegistry.getSyncProtocol(protocolName);
    if(!syncProtocol) {
        throw runtime_error("There is no sync protocol for " + protocolName);
    }
    syncParams = decideBenchParams(syncProtocol, is);
    if(!syncParams) {
        throw runtime_error("Could not get parameters for " + syncProtocol->getName());
    }

    // Expect Sketches here in file. When there, just skip them.
    string line;
    getline(is, line);
    if (line.find(Sketches::PRINT_KEY) == string::npos) {
        stringstream ss;
        ss << "\"" << Sketches::PRINT_KEY << "\" expected here\n";
        Logger::error_and_quit(ss.str());
    }

    string refFile = getReference(is, fName);
    string fToUse = refFile.empty() ? fName : refFile;

    AElems = make_shared<FromFileGen>(fToUse, FromFileGen::FIRST);
    BElems = make_shared<FromFileGen>(fToUse, FromFileGen::SECOND);
}

BenchParams::~BenchParams() {}
