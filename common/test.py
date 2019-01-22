# from node import Node

if __name__ == "__main__":

    # with open("../data/run.bvh") as f:
    #     for line in f:
    #         print(line)
    Root = Node.createStructureFromBVH("C:\\Users\\ensimag\\Source\\Repos\\\soutyoan\\Image_Synthesis_Animation\\viewer\\models\\walk.bvh")
    a = 1
    Root.create_jointsAnimation_MAYA(a)
	