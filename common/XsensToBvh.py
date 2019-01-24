

"""
Python script to convert .ascii files from Xsens sensors to .bvh files
"""

def ascii_to_bvh(elbow_data, wrist_data, output_name="output"):
    """
    Writes the Hierarchy from the list of ascii_files list.
    """
    # Simplest version : two sensors, [0] : elbow (root) + [1] arm
    # Hierarchy part
    with open(output_name+".bvh", "w") as output_file:
        output_file.write("HIERARCHY\n")
        output_file.write("ROOT elbow\n")
        output_file.write("{\n")
        output_file.write("\tOFFSET 0.00000 0.00000 0.00000\n")
        output_file.write("\tCHANNELS 6 Xposition Yposition Zposition Zrotation Yrotation Xrotation\n")
        output_file.write("\tJOIN wrist\n")
        output_file.write("\t{\n")
        output_file.write("\t\tOFFSET -2.0 -2.0 0.0\n")
        output_file.write("\t\tCHANNELS 3 Zrotation Yrotation Xrotation\n")
        output_file.write("\t\tEnd Site\n")
        output_file.write("\t\t{\n")
        output_file.write("\t\tOFFSET -2.0 -2.0 0.0\n")
        output_file.write("\t\t}\n")
        output_file.write("\t}\n")
        output_file.write("}\n")

	# Motion part


def fill_data(elbow_data, wrist_data):
	"""
	Extraction of values (positions and angles)

	Returns two tables with data, frame rate, numbers of frames
	"""


l="zsdc  dsxcw sd \n zdsqzsd\t"
print(l.rstrip())
ascii_to_bvh(0,0)
