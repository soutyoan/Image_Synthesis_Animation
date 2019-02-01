#!usr/bin/env python3
import math as m
import numpy as np

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
    """
        Extraction of motion informations from the sensors data.

        Parameters
        ----------
        elbow_data          string
                            path to elbow sensor .txt file
        wrist_data          string
                            path to wrist sensor .txt file

        Returns
        -------
        frames, frame_rate, elbow_values, wrist_values

        frames              int
                            number of frames
        frame rate          float
                            frame rate
        elbow_values        list of np.ndarray
                            list of Euler angles at each frame for elbow
        wrist_values        np.ndarray
                            list of Euler angles at each frame for wrist
    """
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
            matA = quat_to_rotMat(float(eq_values[0]), float(eq_values[1]), float(eq_values[2]), float(eq_values[3]))
            matB = quat_to_rotMat(float(wq_values[0]), float(wq_values[1]), float(wq_values[2]), float(wq_values[3]))
            elbow_angles, wrist_angles = globRot_to_locRot(matA, matB)
            elbow_values.append(elbow_angles)
            wrist_values.append(wrist_angles)
            frames += 1
    return frames, frame_rate, elbow_values, wrist_values

def quat_to_rotMat(q0, q1, q2, q3):
    """
        Computes the rotation matrix associated to input quaternion

        cf. https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
    """
    return np.array([[1-2*(q2**2+q3**2), 2*(q1*q2-q0*q3), 2*(q1*q3+q0*q2)],
                     [2*(q1*q2+q0*q3), 1-2*(q1**2+q3**2), 2*(q2*q3-q1*q0)],
                     [2*(q1*q3-q2*q0), 2*(q2*q3+q1*q0), 1-2*(q1**2+q2**2)]])

def rotMat_to_euler(mat):
    """
        Gives the Euler angles (in degrees) from a rotation matrix in R3
        cf. http://nghiaho.com/?page_id=846

        Parameters
        ----------
        mat         np.ndarray
                    input rotation matrix

        Returns
        -------
        np.ndarray
                    [yaw, pitch, roll]
    """
    yaw = m.atan2(mat[2,1], mat[2,2])
    pitch = m.atan2(-mat[2,0], np.linalg.norm(np.array([mat[2,1], mat[2,2]])))
    roll = m.atan2(mat[1,0], mat[0,0])
    return np.array([m.degrees(yaw), m.degrees(pitch), m.degrees(roll)])

def globRot_to_locRot(matA, matB):
    """
        Computation of Euler angles from the elbow and wrist matrixes

        Parameters
        ----------
        matA        np.ndarray
                    rotation matrix of elbow sensor in global grid
        matB        np.ndarray
                    rotation matrix of wrist sensor in global grid

        Returns
        -------
        np.ndarray  [[elbow_x, elbow_y, elbow_z], [wrist_x, wrist_y, wrist_z]]
                    returns Euler angles for elbow and wrist
    """
    # computation for elbow
    elbow_angles = rotMat_to_euler(matA)
    # computation for wrist
    matM = matB @ np.linalg.inv(matA)
    wrist_angles = rotMat_to_euler(matM)
    return elbow_angles, wrist_angles



ascii_to_bvh("elbow_data.txt", "wrist_data.txt")
