#!/usr/bin/env python3
from ns3gym import ns3env
import numpy as np
startSim = False
iterationNum = 1
port = 5555
simTime = 5 # seconds
stepTime = 0.5  # seconds
seed = 0
simArgs = {"--simTime": simTime,
           "--stepTime": stepTime,
           "--testArg": 123}
debug = False

env = ns3env.Ns3Env(port=port, stepTime=stepTime, startSim=startSim, simSeed=seed, simArgs=simArgs, debug=debug)
env.reset()

ob_space = env.observation_space
ac_space = env.action_space
print("Observation space: ", ob_space,  ob_space.dtype)
print("Action space: ", ac_space, ac_space.dtype)

stepIdx = 0
currIt = 0

obs = env.reset()

while True:
    stepIdx += 1
    action = env.action_space.sample()
    for i in range(2):
        if i % 2 == 0:
            action[i] = 0.9
        else:
            action[i] = 0.1

    print(action)

    re = env.step(action)
    print(np.array(re[0]).reshape(6,8))

