//
// TryMe.cpp - a really simple example of how to use this library.
// Created by eliez on 8/17/2018.
//


#include <iostream>
#include <GenSync/Syncs/SyncProtocol.h>
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Syncs/CPISync.h>
#include <GenSync/Syncs/CPISync_HalfRound.h>
#include <GenSync/Syncs/FullSync.h>
#include <GenSync/Syncs/IBLTSync.h>
#include <GenSync/Syncs/IBLTSync_HalfRound.h>
#include <GenSync/Syncs/InterCPISync.h>

using std::cout;
using std::endl;
using std::string;

static constexpr int PORT = 8001; // port on which to connect
static constexpr int ERR = 8; // inverse log of error chance
static constexpr int M_BAR = 1; // max differences between server and client
static constexpr int BITS = CHAR_BIT; // bits per entry
static constexpr int PARTS = 3; // partitions per level for partition-syncs
static constexpr int EXP_ELTS = 4; // expected number of elements per set

int main(int argc, char *argv[]) {
    if(strcmp(argv[1], "client")!=0 && strcmp(argv[1], "server")!=0) {
        cout << "usage: 'TryMe client <sync type>' for client mode, 'TryMe server <sync type>' for server mode." << endl;
        cout << "run the client in one terminal instance and the server in another." << endl;
        exit(0);
    }

    std::shared_ptr<SyncProtocol> prot;
    string type = string(argv[2]);

    long bits = BITS * CHAR_BIT;
    // no string switch statements :(
    if(type == "CPISync") {
        prot = std::make_shared<CPISyncProtocol>();
    } else if (type == "InterCPISync") {
        prot = std::make_shared<InterCPISyncProtocol>();
    } else if (type == "OneWayCPISync") {
        prot = std::make_shared<CPISync_HalfRoundProtocol>();
    } else if (type == "FullSync") {
        prot = std::make_shared<FullSyncProtocol>();
    } else if (type == "IBLTSync") {
        prot = std::make_shared<IBLTSyncProtocol>();
        bits = BITS;
    } else if (type == "OneWayIBLTSync") {
        prot = std::make_shared<IBLTSync_HalfRoundProtocol>();
        bits = BITS;
    } else {
        cout << "invalid sync type!" << endl;
        return 1;
    }

    GenSync genSync = GenSync::Builder().
			setProtocol(prot).
			setComm(GenSync::SyncComm::socket).
			setPort(PORT).
			setErr(ERR).
			setMbar(M_BAR).
			setBits(bits).
			setNumPartitions(PARTS).
			setExpNumElems(EXP_ELTS).
            build();

    genSync.addElem(make_shared<DataObject>('a'));
    genSync.addElem(make_shared<DataObject>('b'));
    genSync.addElem(make_shared<DataObject>('c'));

    if(strcmp(argv[1], "client")==0) {
        genSync.addElem(make_shared<DataObject>('d'));

        cout << "listening on port " << PORT << "..." << endl;
		genSync.clientSyncBegin(0);
        cout << "sync succeeded." << endl;

    } else {
        genSync.addElem(make_shared<DataObject>('e'));

        cout << "connecting on port " << PORT << "..." << endl;
		genSync.serverSyncBegin(0);
        cout << "sync succeeded." << endl;
    }
}
