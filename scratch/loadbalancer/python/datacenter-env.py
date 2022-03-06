#!/usr/bin/env python3
import subprocess

import numpy as np
from ns3gym import ns3env

startSim = False
iterationNum = 1
simTime = 5  # seconds
stepTime = 0.5  # seconds
seed = 0
simArgs = {"--simTime": simTime,
           "--stepTime": stepTime,
           "--testArg": 123}
debug = True


class DataCenterEnv(object):
    def __init__(self, **kwargs):
        self.ns_filename = kwargs.pop("filename", "/home/slahmer/CLionProjects/spine-leaf-ns3/build/scratch"
                                                  "/loadbalancer/ns3-dev-main-loadbalancer-debug")
        self.ns_port = kwargs.pop("port", 5555)
        self.ns_simTime = kwargs.pop("simtime", 10)
        self.ns_simTime = kwargs.pop("steptime", 1)
        self.ns_seed = kwargs.pop("seed", 1)
        self.ns_serverCount = kwargs.pop("serverCount", 1)
        self.ns_leafCount = kwargs.pop("leafCount", 4)
        self.ns_spineCount = kwargs.pop("spineCount", 2)
        self.proc = None
        self.env = None

    def reset(self):
        self.stop()
        self.proc = subprocess.Popen([self.ns_filename])
        self.env = ns3env.Ns3Env(port=self.ns_port, stepTime=self.ns_simTime, startSim=startSim, simSeed=self.ns_seed)

        return self.env.reset()

    def step(self, action):
        return self.env.step(action)

    def action_space(self):
        return self.env.action_space

    def observation_space(self):
        return self.env.observation_space

    def sample_action(self):
        return self.env.action_space.sample()

    def stop(self):
        if self.proc is not None:
            self.proc.kill()
        if self.env is not None:
            self.env.close()


env = DataCenterEnv()

a = env.reset()

for i in range(10):
    action = env.sample_action()
    for i in range(len(action)):
        if i < 2:
            if i % 2 == 0:
                action[i] = 1.0
            else:
                action[i] = 0.0
        else:
            if i % 2 == 0:
                action[i] = 1.0
            else:
                action[i] = 0.0
    print("Action: {}".format(action))

    ret = env.step(action)
    for elm in np.array(ret[0]).reshape(7, 8):
        for val in elm:
            print("{:8.3f}".format(val), end="\t")
        print("\n")
