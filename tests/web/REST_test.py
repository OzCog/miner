#!/usr/bin/env python
# Test REST interface by spawning server and sending a bunch of commands.

import sys
import os
import time
import subprocess
import unittest
import json

import urllib2
import urllib

server_exe = sys.argv[1]
opencog_conf = sys.argv[2]
del sys.argv[1:3]
server_process=None

base_url = 'http://localhost:17034/'
rest_url = base_url + 'rest/0.2/'

def spawn_server():
    global server_process
    print "Spawning server"
    server_process = subprocess.Popen([server_exe, '-c',
            '../../../lib/opencog.conf'], stdout=sys.stdout)
#stderr=subprocess.PIPE, stdout=subprocess.PIPE) #, '-DLOG_TO_STDOUT=TRUE'])
    time.sleep(1) # Allow modules time to load
    print "Server spawned with pid %d" % (server_process.pid,)

def kill_server():
    print "Killing server"
    server_process.terminate()
    #req = urllib2.Request(base_url + '/opencog/request/shutdown')
    #response = urllib2.urlopen(req).read()
    #stdout,stderr = server_process.communicate()
    print "Server killed"

class TestPostAtom(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass
        
    def testPostSuccess(self):
        """ Just test a very basic add """
        data = '{ "type":"ConceptNode", "name":"testPostSuccess", "truthvalue": {"simple": {"str":0.5, "count":10}}}'
        req = urllib2.Request(rest_url + 'atom/',data)
        response = urllib2.urlopen(req).read()
        result = json.loads(response)
        self.assertTrue("result" in result)
        self.assertEqual(result["result"], "created")

    def testPostMerge(self):
        """ Test that merging happens if the atom already exists """
        data = '{ "type":"ConceptNode", "name":"testPostMerge", "truthvalue": {"simple": {"str":0.5, "count":10}}}'
        req = urllib2.Request(rest_url + 'atom/',data)
        response = urllib2.urlopen(req).read()
        req = urllib2.Request(rest_url + 'atom/',data)
        response = urllib2.urlopen(req).read()
        result = json.loads(response)
        self.assertTrue("result" in result)
        self.assertEqual(result["result"], "merged")

    def testPostTVTypes(self):
        """ Test that all the different TV types parse correctly """
        data = { "type":"ConceptNode",
            "name":"SimpleTV",
            "truthvalue":
            {"simple": {"str":0.5, "count":10}}
        }
        req = urllib2.Request(rest_url + 'atom/',json.dumps(data))
        response = urllib2.urlopen(req).read()
        result = json.loads(response)
        self.assertTrue("result" in result)
        self.assertEqual(result["result"], "created")

        data = { "type":"ConceptNode",
            "name":"CountTV",
            "truthvalue":
            {"count": {"str":0.5, "count":10, "conf":0.5}}
        }
        req = urllib2.Request(rest_url + 'atom/',json.dumps(data))
        response = urllib2.urlopen(req).read()
        result = json.loads(response)
        self.assertTrue("result" in result)
        self.assertEqual(result["result"], "created")

        data = { "type":"ConceptNode",
            "name":"IndefiniteTV",
            "truthvalue":
            {"indefinite": {"l":0.5, "u":0.7, "conf":0.2}}
        }
        req = urllib2.Request(rest_url + 'atom/',json.dumps(data))
        response = urllib2.urlopen(req).read()
        result = json.loads(response)
        self.assertTrue("result" in result)
        self.assertEqual(result["result"], "created")

        data = { "type":"ConceptNode",
            "name":"CompositeTV",
            "truthvalue":
            {"composite": {"primary": { "simple": {"str":0.5, "count":10}},
                "CONTEXTUAL":[1, {"simple": {"str":0.5, "count":10}}] }
            }
        }
        req = urllib2.Request(rest_url + 'atom/',json.dumps(data))
        response = urllib2.urlopen(req).read()
        result = json.loads(response)
        self.assertTrue("result" in result)
        self.assertEqual(result["result"], "created")

    def testPostTVRobustness(self):
        """ Test that bad json TV syntax fails gracefully, i.e. doesn't kill the
            server!
        """
        data = """{ "type":"ConceptNode",
            "name":"SimpleTVMutations",
            "truthvalue":
            {"simple": {"str":0.5, "count":10}}
        }"""
        import random
        r = random.Random()
        for i in range(0,40):
            data_copy = list(data)
            data_copy[r.randint(0,len(data)-1)] = ' '
            data_copy = ''.join(data_copy)
            req = urllib2.Request(rest_url + 'atom/',data_copy)
            response = urllib2.urlopen(req).read()
            try:
                result = json.loads(response)
            except ValueError:
                print response
            self.assertEqual(server_process.returncode, None)

        data =""" { "type":"ConceptNode",
            "name":"CompositeTVMutations",
            "truthvalue":
            {"composite": {"primary": { "simple": {"str":0.5, "count":10}},
                "CONTEXTUAL":[1, {"simple": {"str":0.5, "count":10}}] }
                "HYPOTHETICAL":[2, {"simple": {"str":0.5, "count":10}}] }
            }
        }"""
        for i in range(0,40):
            data_copy = list(data)
            data_copy[r.randint(0,len(data)-1)] = ' '
            data_copy = ''.join(data_copy)
            req = urllib2.Request(rest_url + 'atom/',data_copy)
            response = urllib2.urlopen(req).read()
            print response
            try:
                result = json.loads(response)
            except ValueError:
                print response
            self.assertEqual(server_process.returncode, None)


if __name__ == "__main__":
    print "Starting REST interface test"
    spawn_server()
    # unittest.main is stupid and has sys.exit hardcoded. python 2.7 has an
    # option to disable it, but most people still use 2.6 or earlier.
    try:
        unittest.main()
    except SystemExit, e:
        print('caught exit')
        kill_server()
        raise e

