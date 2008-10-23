#include "TulipWriter.h"
#include <CogServer.h>
#include <AtomSpace.h>

#include <boost/foreach.hpp>

#include <time.h>
#include <sstream>

#define foreach BOOST_FOREACH

namespace opencog {


void TulipWriter::writeNodes(HandleSeq nodes)
{
    // Output Node numbers/ids
    myfile << "(nodes ";
    foreach (Handle h, nodes) {
        myfile << h << " ";
    }
    myfile << ")" << endl;
}

bool TulipWriter::write(Handle seed, int depth, Handle setLink)
{
    //CogServer& cogserver = static_cast<CogServer&>(server());
    AtomSpace* a = BaseServer::getAtomSpace();
    myfile.open( filename.c_str() );
    int notLinkOffsetIndex = 1000000;
    // Write header
    //
    myfile << "(tlp \"2.0\"" << endl;
    myfile << "(date \"" << getDateString() << "\")" << endl;
    myfile << "(comments \"This file was generated by OpenCog "
        "( http://opencog.org/ ).\")" << endl;

    HandleSeq nodeHandles;
    a->getHandleSet(back_inserter(nodeHandles), (Type) NODE, true );
    HandleSeq linkHandles;
    a->getHandleSet(back_inserter(linkHandles), (Type) LINK, true );
    HandleSeq notLinks;
    a->getHandleSet(back_inserter(notLinks), (Type) NOT_LINK, true );
    std::set<Handle> inSet;
    // Handles in setLink will be made a separate cluster in tulip
    if (setLink) {
        HandleSeq setLinks = a->getOutgoing(setLink);
        foreach (Handle h, setLinks) {
            inSet.insert(h);
        }
    }

    HandleSeq nodesToWrite;
    copy(nodeHandles.begin(), nodeHandles.end(), back_inserter(nodesToWrite));
    copy(notLinks.begin(), notLinks.end(), back_inserter(nodesToWrite));
    writeNodes(nodesToWrite);

    // Output Edge numbers/ids, source, and target
    foreach (Handle h, linkHandles) {
        if (a->getType(h) == NOT_LINK) {
            myfile << "(edge " << h + notLinkOffsetIndex << " " << h << " ";
                foreach (Handle t, a->getOutgoing(h)) {
                    myfile << t << " ";
                }
            myfile << ")" << endl;
        } else if (a->getType(h) != SET_LINK) {
            myfile << "(edge " << h << " ";
                foreach (Handle t, a->getOutgoing(h)) {
                    myfile << t << " ";
                }
            myfile << ")" << endl;
        }
    }

    // Output setLink as a cluster
    myfile << "(cluster 1 \"Forward chaining results\"" << endl;
    myfile << " (edges ";
    foreach (Handle h, inSet) {
        myfile << h << " ";
    }
    myfile << ")\n)" << endl;

    // Output everything not in setLink as a cluster
    myfile << "(cluster 2 \"Original word pairs\"" << endl;
    myfile << " (nodes ";
    foreach (Handle h, nodeHandles) {
        myfile << h << " ";
    }
    foreach (Handle h, linkHandles) {
        if (a->getType(h) == NOT_LINK) {
            myfile << h << " ";
        }
    }
    myfile << ")" << endl;
    myfile << " (edges ";
    foreach (Handle h, linkHandles) {
        std::set<Handle>::iterator si = inSet.find(h);
        if (si == inSet.end()) {
            if (a->getType(h) == NOT_LINK) {
                myfile << h + notLinkOffsetIndex << " ";
            } else if (a->getType(h) != SET_LINK) {
                myfile << h << " ";
            }
        }
    }
    myfile << ")\n)" << endl;

    // Output node names
    myfile << "(property  0 string \"viewLabel\" " << endl;
    myfile << "  (default \"\" \"\" )" << endl;
    foreach (Handle h, nodeHandles) {
        myfile << "  (node " << h << " \"" << a->getName(h) << "\")" << endl;
    }
    // give not nodes the name NOT
    foreach (Handle h, notLinks) {
        myfile << "(node " << h << " \"NOT\" )" << endl;
    }
    myfile << ")" << endl;

    // Define default colouring
    myfile << "(property  0 color \"viewColor\"" << endl;
    myfile << "(default \"(35,0,235,255)\" \"(0,0,0,128)\" )" << endl;
    foreach (Handle h, notLinks) {
        myfile << "(node " << h << " \"(235,0,35,255)\" )" << endl;
        myfile << "(edge " << h + notLinkOffsetIndex << " \"(235,35,35,255)\" )" << endl;
    }
    myfile << ")" << endl;

    // Output strength component of truth value
    myfile << "(property  0 double \"strength\"" << endl;
    myfile << "(default \"0.0\" \"0.0\" )" << endl;
    foreach (Handle h, nodeHandles) {
        myfile << "  (node " << h << " \"" << a->getTV(h).getMean() << "\")" << endl;
    }
    foreach (Handle h, linkHandles) {
        if (a->getType(h) != NOT_LINK && a->getType(h) != SET_LINK) {
            myfile << "  (edge " << h << " \"" << a->getTV(h).getMean() << "\")" << endl;
        }
    }
    myfile << ")" << endl;

    // Output distance metric as 1/strength 
    myfile << "(property  0 double \"distance\"" << endl;
    myfile << "(default \"0.0\" \"0.0\" )" << endl;
    foreach (Handle h, linkHandles) {
        if (a->getType(h) != NOT_LINK && a->getType(h) != SET_LINK) {
            myfile << "  (edge " << h << " \"" << 1.0 / (a->getTV(h).getMean()+0.0000001) << "\")" << endl;
        }
    }
    foreach (Handle h, notLinks) {
        myfile << "  (edge " << h + notLinkOffsetIndex << " \"0.9\")" << endl;
    }
    myfile << ")" << endl;

    // Output count component of truth value
    myfile << "(property  0 double \"count\"" << endl;
    myfile << "(default \"0.0\" \"0.0\" )" << endl;
    foreach (Handle h, nodeHandles) {
        myfile << "  (node " << h << " \"" << a->getTV(h).getConfidence() << "\")" << endl;
    }
    foreach (Handle h, linkHandles) {
        if (a->getType(h) == NOT_LINK)
            myfile << "  (node " << h << " \"" << a->getTV(h).getConfidence() << "\")" << endl;
        else if (a->getType(h) != SET_LINK)
            myfile << "  (edge " << h << " \"" << a->getTV(h).getConfidence() << "\")" << endl;
    }
    myfile << ")" << endl;
   
    // Close header
    myfile << ")" << endl;
    myfile.close();

    return true;
}

std::string TulipWriter::getDateString()
{
    time_t rawtime;
    struct tm * timeinfo;
    ostringstream datestr;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    datestr << timeinfo->tm_mday << "-";
    datestr << timeinfo->tm_mon+1 << "-";
    datestr << timeinfo->tm_year + 1900;
    return datestr.str();

}

} // namespace opencog
