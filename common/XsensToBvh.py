#!usr/bin/env python3
import math as m

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
        output_file.write("\tJOINT wrist\n")
        output_file.write("\t{\n")
        output_file.write("\t\tOFFSET -5.0 -0.0 0.0\n")
        output_file.write("\t\tCHANNELS 3 Zrotation Yrotation Xrotation\n")
        output_file.write("\t\tEnd Site\n")
        output_file.write("\t\t{\n")
        output_file.write("\t\tOFFSET 0.0 5.0 0.0\n")
        output_file.write("\t\t}\n")
        output_file.write("\t}\n")
        output_file.write("}\n")

        #Motion part
        output_file.write("MOTION\n")
        frames, frame_rate, values1, values2 = fill_data(elbow_data, wrist_data)
        output_file.write("Frames: "+str(frames)+"\n")
        output_file.write("Frame rate: "+str(frame_rate)+"\n")
        for i in range(frames):
            output_file.write("0.0 0.0 0.0 ") # unchanged for root (all to 0.0)
            for val in values1[i]:
                output_file.write(str(val)+" ")
            for val in values2[i]:
                output_file.write(str(val)+" ")
            output_file.write("\n")

def fill_data(elbow_data, wrist_data):
    frame_rate = 1/40.0
    frames = 0
    elbow_values = []
    wrist_values = []
    with open(elbow_data, "r") as f1, open(wrist_data, "r") as f2:
        elbow_lines, wrist_lines = f1.readlines(), f2.readlines()
        for l1, l2 in zip(elbow_lines, wrist_lines):
            if (l1[0]=='/' or l1[0]=='P'): # reading headers
                continue
            l1_values = l1.split('\t')
            l2_values = l2.split('\t')
            eq_values = [l1_values[i] for i in range(4, 8)] # quaternion values
            wq_values = [l2_values[i] for i in range(4, 8)] # quaternion values
            r1, r2, r3 = quaternion_to_euler(float(eq_values[0]), float(eq_values[1]), float(eq_values[2]), float(eq_values[3]))
            elbow_values.append([r1, r2, r3])
            r1, r2, r3 = quaternion_to_euler(float(wq_values[0]), float(wq_values[1]), float(wq_values[2]), float(wq_values[3]))
            wrist_values.append([r1, r2, r3])
            frames += 1
    return frames, frame_rate, elbow_values, wrist_values


def quaternion_to_euler(q0, q1, q2, q3):
    """
    Conversion from quaternion to Euler angles

    Returns angle_x, angle_y, angle_z (in degree)
    """
    angle_x = m.atan2(2*(q0*q1+q2*q3), 1 - 2*(q1**2+q2**2))
    if (m.fabs(2*(q0*q2 - q3*q1)) >= 1):
        angle_y = m.pi/2
    else:
        angle_y = m.asin(2*(q0*q2 - q3*q1))
    angle_z = m.atan2(2*q0*q3+q2*q1, 1 - 2*(q2**2+q3**2))
    return m.degrees(angle_x), m.degrees(angle_y), m.degrees(angle_z)



ascii_to_bvh("elbow_data.txt", "wrist_data.txt")
