/*
 * opencog/server/CogServer.cc
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

#include "CogServer.h"

#include <memory>

#include <time.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include <opencog/server/SimpleNetworkServer.h>
#include <opencog/util/Config.h>
#include <opencog/util/Logger.h>
#include <opencog/util/exceptions.h>

using namespace opencog;

CogServer::CogServer() : cycleCount(1), networkServer(NULL)
{
    if (atomSpace != NULL) delete atomSpace;
    atomSpace = new AtomSpace();

    pthread_mutex_init(&messageQueueLock, NULL);
}

CogServer::~CogServer()
{
    disableNetworkServer();
}

CogServer* CogServer::createInstance() {
    return new CogServer();
}

void CogServer::enableNetworkServer()
{
    if (networkServer == NULL)
        networkServer = new SimpleNetworkServer(this, config().get_int("SERVER_PORT"));
}

void CogServer::disableNetworkServer()
{
    if (networkServer != NULL) {
        delete networkServer;
        networkServer = NULL;
    }
}

void CogServer::serverLoop()
{
    struct timeval timer_start, timer_end;
    time_t elapsed_time;
    time_t cycle_duration = config().get_int("SERVER_CYCLE_DURATION") * 1000;

    if (networkServer != NULL) networkServer->start();
    logger().info("opencog server ready.");

    gettimeofday(&timer_start, NULL);
    for (running = true; running;) {

        bool had_input = false;
        if (getRequestQueueSize() != 0) {
            processRequests();
            had_input = true;
        }

        // Only run the input handlers if there was actual input.
        if (had_input) {
            processInput();
        }
        processMindAgents();

        cycleCount++;
        if (cycleCount < 0) cycleCount = 0;

        // sleep long enough so that the next cycle will only start
        // after config["SERVER_CYCLE_DURATION"] milliseconds
        gettimeofday(&timer_end, NULL);
        elapsed_time = ((timer_end.tv_sec - timer_start.tv_sec) * 1000000) +
                       (timer_end.tv_usec - timer_start.tv_usec);
        if ((cycle_duration - elapsed_time) > 0)
            usleep((unsigned int) (cycle_duration - elapsed_time));
        timer_start = timer_end;
    }
}

void CogServer::processRequests(void)
{
    int countDown = getRequestQueueSize();
    while (countDown != 0) {
        CogServerRequest *request = popRequest();
        request->processRequest();
        delete request;
        countDown--;
    }
}

void CogServer::processMindAgents(void)
{
    for (unsigned int i = 0; i < mindAgents.size(); i++) {
        if ((cycleCount % mindAgents[i].frequency) == 0) {
            (mindAgents[i].agent)->run(this);
        }
    }
}

void CogServer::processInput(void)
{
    for (unsigned int i = 0; i < inputHandlers.size(); i++) {
        inputHandlers[i]->run(this);
    }
}

void CogServer::plugInMindAgent(MindAgent *agent, int frequency)
{
    ScheduledMindAgent newScheduledAgent;
    newScheduledAgent.agent = agent;
    newScheduledAgent.frequency = frequency;
    mindAgents.push_back(newScheduledAgent);
}

void CogServer::plugInInputHandler(MindAgent *handler)
{
    inputHandlers.push_back(handler);
}

long CogServer::getCycleCount()
{
    return cycleCount;
}

void CogServer::stop()
{
    running = false;
}

CogServerRequest *CogServer::popRequest()
{

    CogServerRequest *request;

    pthread_mutex_lock(&messageQueueLock);
    if (requestQueue.empty()) {
        request = NULL;
    } else {
        request = requestQueue.front();
        requestQueue.pop();
    }
    pthread_mutex_unlock(&messageQueueLock);

    return request;
}

void CogServer::pushRequest(CogServerRequest *request)
{
    pthread_mutex_lock(&messageQueueLock);
    requestQueue.push(request);
    pthread_mutex_unlock(&messageQueueLock);
}

int CogServer::getRequestQueueSize()
{
    pthread_mutex_lock(&messageQueueLock);
    int size = requestQueue.size();
    pthread_mutex_unlock(&messageQueueLock);
    return size;
}

// Used for debug purposes on unit tests
void CogServer::unitTestServerLoop(int nCycles)
{
    for (int i = 0; (nCycles == 0) || (i < nCycles); ++i) {
        processRequests();
        processMindAgents();
        cycleCount++;
        if (cycleCount < 0) cycleCount = 0;
    }
}
