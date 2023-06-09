# param_of_bottom_aruco.put(25f, List.of(-0.1535f,0.134f));
# param_of_bottom_aruco.put(26f, List.of(0.1535f,0.134f));
# param_of_bottom_aruco.put(27f, List.of(0.1535f,-0.134f));
# param_of_bottom_aruco.put(28f, List.of(-0.1535f,-0.134f));

# param_of_bottom_aruco.put(25f, List.of(0.15f,0.118f));
# param_of_bottom_aruco.put(26f, List.of(0.143f,-0.16f));
# param_of_bottom_aruco.put(27f, List.of(-0.099f,-0.093f));
# param_of_bottom_aruco.put(28f, List.of(-0.165f,0.12f));
def finalCoor(data):
    x_offset = -0.08 #-0.12
    y_offset = -0.02 #-0.07
    z_offset = 0
    aruco_coordinate = {
        # NO CALIBRATION
        # 25: [-0.1535,0.134],1
        # 26: [0.1535,0.134],
        # 27: [0.1535,-0.134],
        # 28: [-0.1535,-0.134]

        #WITH CALIBRATION
        25: [-0.1535,0.134],
        26: [0.1535,0.134],
        27: [0.1535,-0.134],
        28: [-0.1205 ,-0.121]



        # 25: [0.15,0.118],
        # 26: [0.143,-0.16],
        # 27: [-0.099,-0.093],
        # 28: [-0.165,0.12]
    }
    id = data['id']
    x = float(data['x'])
    y = float(data['y'])
    z = float(data['z'])
    final_x = x + x_offset - aruco_coordinate[id][0]
    final_y = -y + y_offset - aruco_coordinate[id][1]
    final_z = -z + z_offset
    print(id,final_x,final_y,final_z)
    return final_x,final_y,final_z