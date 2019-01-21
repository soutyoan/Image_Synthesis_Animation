# from node import Node

if __name__ == "__main__":

    # with open("../data/run.bvh") as f:
    #     for line in f:
    #         print(line)
    Root = Node.createStructureFromBVH("C:\\Users\\ensimag\\Source\\Repos\\Image_Synthesis_Animation\\viewer\\models\\walk1.bvh")

    Root.create_jointsAnimation_MAYA(2)
	