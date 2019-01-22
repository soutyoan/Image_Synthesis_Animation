# from node import Node

if __name__ == "__main__":

    # with open("../data/run.bvh") as f:
    #     for line in f:
    #         print(line)
    Root = Node.createStructureFromBVH("C:\\Users\\ensimag\\Source\\Repos\\Image_Synthesis_Animation\\viewer\\models\\walk.bvh")
    L = [[0, 0, 0] for _ in range(1000)]
    a = 1
    Root.createjointDEBUG(a, L)
	