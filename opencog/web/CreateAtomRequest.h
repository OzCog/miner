/*
 * opencog/rest/CreateAtomRequest.h
 *
 * Copyright (C) 2010 by Singularity Institute for Artificial Intelligence
 * All Rights Reserved
 *
 * Written by Joel Pitt <joel@fruitionnz.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOG_CREATE_ATOM_REQUEST_H
#define _OPENCOG_CREATE_ATOM_REQUEST_H

#include <sstream>
#include <string>
#include <vector>

#include <opencog/atomspace/types.h>
#include <opencog/atomspace/TruthValue.h>
#include <opencog/server/Request.h>

#include <opencog/web/json_spirit/json_spirit.h>

namespace opencog
{

class CreateAtomRequest : public Request
{

protected:

    std::ostringstream  _output;

public:

    static inline const RequestClassInfo& info() {
        static const RequestClassInfo _cci(
            "create-atom-json",
            "Create a new atom from JSON",
            "Usage: create-atom<CRLF>JSON<CRLF><Ctrl-D><CRLF>\n\n"
            "   Create an atom based on the JSON format:\n"
            "   { \"type\": TYPENAME, \"name\": NAME, \n"
            "     \"outgoing\": [ UUID1, UUID2 ... ], \n"
            "     \"truthvalue\": {\"simple|composite|count|indefinite\":\n"
            "          [truthvalue details] } \n",
            true, false // not shell, is hidden
        );
        return _cci;
    }

    CreateAtomRequest();
    virtual ~CreateAtomRequest();
    virtual bool execute(void);
    virtual bool isShell(void) {return info().is_shell;}
    void json_makeOutput(Handle h, bool exists);
    TruthValue* JSONToTV(const json_spirit::Value&);
    void generateProcessingGraph(Handle h);
    void setRequestResult(RequestResult* rr);
    bool assertJSONTVCorrect(std::string expected, std::string actual);
    bool assertJsonMapContains(const json_spirit::Object& o, std::vector<std::string> keys);
};

} // namespace 

#endif // _OPENCOG_CREATE_ATOM_REQUEST_H
