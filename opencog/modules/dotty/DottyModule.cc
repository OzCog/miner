#include <string>
#include <queue>

#include <opencog/util/platform.h>
#include <opencog/atomspace/Node.h>
#include <opencog/atomspace/Link.h>
#include <opencog/server/CogServer.h>

using namespace opencog;

class DottyGrapher
{
public:
    DottyGrapher(AtomSpace *space) : space(space), withIncoming(false) { answer = ""; }
    AtomSpace *space;
    std::string answer;
    bool withIncoming;

    /**
     * Outputs a dotty node for an atom.
     */
    bool do_nodes(const Atom *a)
    {
        std::ostringstream ost;
        ost << TLB::getHandle(a) << " [";
        if (!space->isNode(a->getType()))
            ost << "shape=\"diamond\" ";
        ost << "label=\"[" << ClassServer::getTypeName(a->getType()) << "]";

        const Node *n = dynamic_cast<const Node *>(a);
        if (n) {
            ost << " " << n->getName();
        } else {
            const Link *l = dynamic_cast<const Link *>(a);
            l = l; // TODO: anything to output for links?
        }
        ost << "\"];\n";
        answer += ost.str();
        return false;
    }

    /**
     * Outputs dotty links for an atom's outgoing connections.
     */
    bool do_links(const Atom *a)
    {
        Handle h = TLB::getHandle(a);
        std::ostringstream ost;
        const std::vector<Handle> &out = a->getOutgoingSet();
        for (size_t i = 0; i < out.size(); i++) {
            ost << h << "->" << out[i] << " [label=\"" << i << "\"];\n";
        }

        if (withIncoming) {
            HandleEntry *he = a->getIncomingSet();
            int i = 0;
            while (he) {
                ost << h << "->" << he->handle << " [style=\"dotted\" label=\"" << i << "\"];\n";
                he = he->next;
                i++;
            }
        }

        answer += ost.str();
        return false;
    }

    void graph()
    {
        answer += "\ndigraph OpenCog {\n";
        space->getAtomTable().foreach_atom(&DottyGrapher::do_nodes, this);
        space->getAtomTable().foreach_atom(&DottyGrapher::do_links, this);
        answer += "}\n";
    }

};

extern "C" std::string cmd_dotty(std::queue<std::string> &args)
{
    AtomSpace *space = CogServer::getAtomSpace();
    DottyGrapher g(space);
    while (!args.empty()) {
        if (args.front() == "with-incoming")
            g.withIncoming = true;
        args.pop();
    }
    g.graph();
    return g.answer;
}
