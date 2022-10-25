import os
import pickle
import graphviz

import matplotlib.pyplot as plt
import networkx as nx
import numpy as np
from matplotlib import cm

from utils import read_depths, color_from_array, read_csv
from pathlib import Path

cmaps = {}

gradient = np.linspace(0, 1, 256)
gradient = np.vstack((gradient, gradient))


def tukey_depth(db, graph, pos, graph_num, cmap):
    x = "_4"
    if os.path.isfile(f"../out/{db}.tukey{x}"):
        depths = read_depths(f"../out/{db}.tukey{x}")
        depth = depths[graph_num]
        nx.draw(graph, pos=pos, node_color=color_from_array(cmap, depth, True))
        plt.savefig(f"Images/{db}_depth{x}.png")
        plt.clf()


def draw_depth(path: str, graph_num, cmap: str = "viridis"):
    graph = nx.read_edgelist(path)
    db = Path(path).stem
    if os.path.isfile(f"Data/{db}_pos.pickle"):
        pos = pickle.load(open(f'Data/{db}_pos.pickle', "rb"))
    else:
        pos = nx.nx_pydot.graphviz_layout(graph)

    tukey_depth(db, graph, pos, graph_num, cmap)




def main():
    cmap = "cividis"
    draw_depth("../../../Documents/PythonProjects/OSMGraphs/data/Endenich.edges", 0, cmap=cmap)


if __name__ == '__main__':
    main()
