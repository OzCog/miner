/*
 * opencog/server/CommandRequestProcessor.h
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * All Rights Reserved
 *
 * Written by Andre Senna <senna@vettalabs.com>
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

#ifndef _OPENCOG_COMMAND_REQUEST_PROCESSOR_H
#define _OPENCOG_COMMAND_REQUEST_PROCESSOR_H

#include <map>
#include <queue>
#include <string>

#include <opencog/atomspace/types.h>
#include <opencog/server/RequestProcessor.h>
#include <opencog/xml/XMLBufferReader.h>

#ifdef HAVE_SQL_STORAGE
#include <opencog/persist/AtomStorage.h>
#endif /* HAVE_SQL_STORAGE */

#ifdef HAVE_LIBMEMCACHED
#include <opencog/memcache/AtomCache.h>
#endif /* HAVE_LIBMEMCACHED */

#ifdef HAVE_GUILE
#include <opencog/guile/SchemeShell.h>
#endif /* HAVE_GUILE */

namespace opencog
{

class CommandRequestProcessor : public RequestProcessor
{
public:
    ~CommandRequestProcessor();
    CommandRequestProcessor(void);
    virtual void processRequest(CogServerRequest *);

private:
    std::string prompt;
    std::string loadXML(XMLBufferReader *);
    std::string data(std::string);
    std::string help(std::string);
    std::string load(std::string);
#ifdef HAVE_MODULES
    std::string dlopen(std::string);
    std::string dlclose(std::string);
    bool externalCommand(std::string, std::queue<std::string>&, std::string&);
#endif /* HAVE_MODULES */
    std::string ls(std::string, std::string);
    std::string ls(std::string);
    std::string ls(Handle);
    std::string ls(void);
#ifdef HAVE_SQL_STORAGE
    std::string sql_open(std::string, std::string, std::string);
    std::string sql_close(void);
    std::string sql_load(void);
    std::string sql_store(void);

    AtomStorage *store;
#endif /* HAVE_SQL_STORAGE */
#ifdef HAVE_LIBMEMCACHED
    std::string cache_open(std::string, std::string);
    std::string cache_close(void);
    std::string cache_load(void);
    std::string cache_store(void);

    AtomCache *cache;
#endif /* HAVE_LIBMEMCACHED */
#ifdef HAVE_GUILE
    bool shell_mode;
    SchemeShell *ss;
#endif /* HAVE_GUILE */

    int load_count;
    std::map<std::string, void*> dlmodules;

}; // class
}  // namespace

#endif // _OPENCOG_COMMAND_REQUEST_PROCESSOR_H
