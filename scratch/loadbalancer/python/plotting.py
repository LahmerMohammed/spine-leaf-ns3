import numpy as np
import pandas as pd

from matplotlib import pyplot as plt

pd_data = pd.read_csv("file.csv")
first_tx = np.sort(pd_data['first_tx'].to_numpy(dtype=float))
last_rx = np.sort(pd_data['last_rx'].to_numpy(dtype=float))


def plot_server2server_flowz_distribution(data):
    a_group = data.groupby(["srcaddr", "destaddr"])
    for name, group in a_group:
        title = "flow-{}".format(name)
        nw_data = group.sort_values(['tx_packets'])
        size = nw_data.tx_bytes.size
        x1 = []
        for i in range(1, size + 1):
            x1.append(i)
        plt.bar(x1, nw_data.tx_packets, label='stream size', color='r')
        plt.xlim((0, size + 1))
        # plt.bar(x2,nw_data.packet_count,label='packets count',color='c')
        plt.xlabel("stream")
        plt.ylabel("size (Packets)")
        plt.title(title)
        plt.legend()
        plt.savefig("z{}.png".format(title))
        plt.clf()


def active_streams(first_tx_sorted, last_rx_sorted):
    print(first_tx_sorted)
    print(last_rx_sorted)
    i = j = k = 0
    x = []
    y = []
    active = 0
    while i < len(first_tx_sorted) and j < len(last_rx_sorted):
        if first_tx_sorted[i] < last_rx_sorted[j]:
            active += 1
            x.append(first_tx_sorted[i])
            i += 1
        else:
            active -= 1
            x.append(last_rx_sorted[j])
            j += 1
        y.append(active)

    while i < len(first_tx_sorted):
        active += 1
        x.append(first_tx_sorted[i])
        i += 1
        y.append(active)
    while j < len(last_rx_sorted):
        active -= 1
        x.append(last_rx_sorted[j])
        j += 1
        y.append(active)

    plt.plot(x, y, label='active flow size', color='r')
    plt.xlabel("time")
    plt.ylabel("#active flow")
    plt.legend()
    plt.show()

def display_active_flowz(tx_sorted, rx_sorted):
    arr = []
    x = []
    tx_i = rx_i = 0
    current = 0
    while tx_i < len(tx_sorted) or rx_i < len(rx_sorted):

        if tx_i < len(tx_sorted) and tx_sorted[tx_i] <= rx_sorted[rx_i]:
            current += 1
            x_tmp = tx_sorted[tx_i]
            tx_i += 1
        else:
            current -= 1
            x_tmp = rx_sorted[rx_i]
            rx_i += 1
        print(current)
        x.append(x_tmp)
        arr.append(current)

    plt.plot(np.sort(np.array(x)), arr, label='active flow size', color='r')
    plt.xlabel("time")
    plt.ylabel("#active flow")
    plt.legend()
    plt.show()


active_streams(first_tx, last_rx)
