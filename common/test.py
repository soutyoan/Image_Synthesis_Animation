from node import Node

if __name__ == "__main__":

    # with open("../data/run.bvh") as f:
    #     for line in f:
    #         print(line)

    Root = Node.createStructureFromBVH("../viewer/models/walk.bvh")
    print(Root)
