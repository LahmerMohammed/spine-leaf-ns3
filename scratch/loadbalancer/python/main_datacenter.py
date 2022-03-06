import numpy as np
import torch
from collections import deque
from ddpg import Agent

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
print("using", device)

num_agents = 1
action_size = 2
state_size = 33
actions = np.random.randn(num_agents, action_size)

agent = Agent(state_size=state_size,
              action_size=action_size,
              num_agents=num_agents, random_seed=0)

model_save_path = 'trained_models/ddpg/'


def ddpg_train(n_epochs=500, max_t=1000, target_score=30, PRINT_EVERY=50):
    scores_deque = deque(maxlen=100)
    scores = []
    max_score = -np.Inf

    for i_epoch in range(1, n_epochs + 1):
        states = np.random.random((num_agents, state_size))
        agent.reset()
        score = np.zeros(num_agents)
        for t in range(max_t):
            actions = agent.act(states)
            next_states = np.random.random((num_agents, state_size))
            rewards = np.random.randint(-10, 10, num_agents)  # get reward (for each agent)
            dones = np.zeros(num_agents)  # see if episode finished

            agent.step(states, actions, rewards, next_states, dones)
            states = next_states
            score += rewards
            if np.any(dones):
                break
        avg_agent_score = np.mean(score)
        scores_deque.append(avg_agent_score)
        scores.append(avg_agent_score)
        if max_score < avg_agent_score:
            max_score = avg_agent_score

        # print progress
        print('\rEpoch: {}\t Average Score: {:.2f}\tCurrent Score: {:.2f}'.format(i_epoch, np.mean(scores_deque),
                                                                                  avg_agent_score), end="")

        if i_epoch % PRINT_EVERY == 0:
            print('\rEpoch: {}\t Average reward: {:.2f}\t Max reward: {:.2f}'.format(i_epoch,
                                                                                     np.mean(scores_deque), max_score))
        if np.mean(scores_deque) > target_score:
            print("\nEnvironment solved in {} episodes! ... Avg Reward: {:.2f}".format(i_epoch - 100,
                                                                                       np.mean(scores_deque)))
            break

    return scores

scores = ddpg_train()

"""
    ob_space = env.observation_space
    ac_space = env.action_space
    print("Observation space: ", ob_space,  ob_space.dtype)
    print("Action space: ", ac_space, ac_space.dtype)

    stepIdx = 0
    currIt = 0

    try:
        while True:
            print("Start iteration: ", currIt)
            obs = env.reset()
            print("Step: ", stepIdx)
            print("---obs: ", obs)

            while True:
                stepIdx += 1
                action = env.action_space.sample()
                print("---action: ", action)

                print("Step: ", stepIdx)
                obs, reward, done, info = env.step(action)
                print("---obs, reward, done, info: ", obs, reward, done, info)
                myVector = obs
                print("---myVector: ", myVector)

                if done:
                    stepIdx = 0
                    if currIt + 1 < iterationNum:
                        env.reset()
                    break

            currIt += 1
            if currIt == iterationNum:
                break

    except KeyboardInterrupt:
        print("Ctrl-C -> Exit")
    finally:
        env.close()
        print("Done")
"""