import json
import os
from time import sleep
import time
import cv2 
import memcache
import numpy as np
import math 
import LocateCircle
import save_aruco2csv
import circle_finder_faster
from final_coord import finalCoor

class LimitedList:
    def __init__(self, initial_size=20):
        self.items = []
        self.initial_size = initial_size

    def add_element(self, element):
        if len(self.items) == self.initial_size:
            self.items.pop(0)  # 删除最早的元素
        self.items.append(element)
    def pop_element_and_getmedian(self):
        if len(self.items) == 0:
            return 'null'
        else:
            self.items.pop(0)
            return np.median(self.items)
    def calculate_median(self):
        return np.median(self.items)
    
class Aruco:
    mtx = np.array([[511.30105788, 0.0, 330.14455905], [0.0, 510.54688952, 247.69595943],[0.0, 0.0, 1.0]])
    dist = np.array([[0.18435012, -0.57693444, 0.00853163, 0.00982479, 0.46038521]])
    markerLength = 0.0585
    # markerLength = 0.0385
    def __init__(self,id) -> None:
        self.id = id
        self.x_list = LimitedList(5)
        self.y_list = LimitedList(5)
        self.z_list = LimitedList(5)
        self.yaw_list = LimitedList(5)
        self.pitch_list = LimitedList(5)
        self.roll_list = LimitedList(5)
    def checkInList(self,ids):
        if ids is None or self.id not in ids:
            self.x_list.pop_element_and_getmedian()
            self.y_list.pop_element_and_getmedian()
            self.z_list.pop_element_and_getmedian()
            self.yaw_list.pop_element_and_getmedian()
            self.pitch_list.pop_element_and_getmedian()
            self.roll_list.pop_element_and_getmedian()
            return False
        else:
            return True
            
    def estimatePoseSingleMarkers(self,corner):
        
        rvec, tvec, markerPoints = cv2.aruco.estimatePoseSingleMarkers(corner, self.markerLength, self.mtx, self.dist)
        self.rvec = rvec
        self.tvec = tvec
        yaw, pitch, roll = self.rvec_to_euler_angles(rvec)
        x = tvec[0][0][0]
        y = tvec[0][0][1]
        z = tvec[0][0][2]
        return x,y,z,yaw,pitch,roll
    def update(self,id,corner):
        if(self.id != id):
            return
        x,y,z,yaw,pitch,roll = self.estimatePoseSingleMarkers(corner)
        self.x_list.add_element(x)
        self.y_list.add_element(y)
        self.z_list.add_element(z)
        self.yaw_list.add_element(math.degrees(yaw))
        self.pitch_list.add_element(math.degrees(pitch))
        self.roll_list.add_element(math.degrees(roll))
        return self.getCoordinate()
    def getCoordinate(self):
        x = self.x_list.calculate_median()
        y = self.y_list.calculate_median()
        z = self.z_list.calculate_median()
        yaw = self.yaw_list.calculate_median()
        pitch = self.pitch_list.calculate_median()
        roll = self.roll_list.calculate_median()
        return x,y,z,yaw,pitch,roll
    def rvec_to_euler_angles(self,rvec):
        rvec_flipped = rvec[0][0]*-1 
        R_mat, jacobian=cv2.Rodrigues(rvec_flipped)        
        pitch = math.atan2(R_mat[2,1],R_mat[2,2])
        sy = math.sqrt((R_mat[0][0]*R_mat[0][0])+ (R_mat[1][0]*R_mat[1][0]))
        singular = sy<1e-6
        if not singular:
            pitch = math.atan2(R_mat[2,1],R_mat[2,2])
            roll = math.atan2(-R_mat[2,0],sy)
            yaw = math.atan2(R_mat[1,0],R_mat[0,0])
        else:
            pitch = math.atan2(-R_mat[1,2],R_mat[1,1])
            roll = math.atan2(-R_mat[2,0],sy)
            yaw = 0
        return yaw, pitch, roll
    def drawAruco(self, frame):
        
        frame = cv2.drawFrameAxes(frame, self.mtx,self.dist,self.rvec,self.tvec, 0.05)
        return frame

def addNewAruco(id,corner,arucoList):
    for aruco in arucoList:
        if aruco.id == id:
            return False
    aruco = Aruco(id)
    aruco.update(id,corner)
    arucoList.append(aruco)

def main():
    #-----------------setting-----------------
    outVedio = True
    drawText = False
    showImage = False
    drawAruco = False
    #-----------------init-----------------
    cap = cv2.VideoCapture(0)
    mc = memcache.Client(['127.0.0.1:11211'], debug=True)
    if(not cap.isOpened()):
        print("Cannot open camera")
        exit()

    arucoList = []
    count = 0
    start_time = cv2.getTickCount()

    #-----------------output vedio-----------------
    if(outVedio):
        outputvediofolder = time.strftime("%Y-%m-%d-%H-%M-%S", time.localtime())
        fourcc = cv2.VideoWriter_fourcc(*'MP4V')
        if(not os.path.exists('output_vedio/')):
            os.makedirs('output_vedio/')
        out = cv2.VideoWriter('output_vedio/'+outputvediofolder+'.mp4', fourcc, 20.0, (640,  480))

        # if(mc.get("find_circle") == b'1' or True):
    while True:
        ret, frame = cap.read()
        #-----------------find circle-----------------

        circle_img = circle_finder_faster.QQcircle(frame,mc)
        # LocateCircle.quickcircle(frame,mc)
            

        #-----------------find aruco-----------------
        arucoDict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_6X6_50)
        arucoParams = cv2.aruco.DetectorParameters_create()
        (corners, ids, rejected) = cv2.aruco.detectMarkers(frame, arucoDict,parameters=arucoParams)
        frame = cv2.aruco.drawDetectedMarkers(	frame, corners, ids, (0,255,0)	)

        if len(corners) > 0:
            for i in range(len(ids)):
                addNewAruco(ids[i][0],corners[i],arucoList)
        for aruco in arucoList:
            if aruco.checkInList(ids) and len(corners) > 0:
                id = aruco.id
                aruco.update(id,corners[np.where(ids == aruco.id)[0][0]])

        send_data = []
        closest_aruco = None
        for aruco in arucoList:
            id = aruco.id
            x,y,z,yaw,pitch,roll = aruco.getCoordinate()
            if(drawAruco==True):
                frame = aruco.drawAruco(frame)
            if(drawText):
                draw(x,y,z,yaw,id,frame,(id-24)*25) #draw aruco
                save_aruco2csv.savetocsv(id,x,y,z,yaw,frame, False)
            if(x == None or y == None or z == None or yaw == None or pitch == None or roll == None):
                continue
            if(math.isnan(x) or math.isnan(y) or math.isnan(z) or math.isnan(yaw) or math.isnan(pitch) or math.isnan(roll)):
                continue
            json_data = {
                'id':int(id),
                'x':format(x,'.2f'),
                'y':format(y,'.2f'),
                'z':format(z,'.2f'),
                'yaw':format(yaw,'.2f'),
                'pitch':format(pitch,'.2f'),
                'roll':format(roll,'.2f')
            }
            if(closest_aruco == None):
                closest_aruco = json_data
            else:
                aruco_distance = math.sqrt(math.pow(x,2)+math.pow(y,2))
                closest_aruco_distance = math.sqrt(math.pow(float(closest_aruco['x']),2)+math.pow(float(closest_aruco['y']),2))
                if(aruco_distance < closest_aruco_distance):
                    closest_aruco = json_data
            # send_data.append(json_data)
        if(closest_aruco != None):
            send_data.append(closest_aruco)
        if(len(send_data) == 0):
            send_data_json = json.dumps(send_data)
        else:
            send_data_json = json.dumps([send_data[0]])
        #------------send data to memcache-----------------
        mc.set('time',cv2.getTickCount())
        mc.set('aruco',send_data_json)
        
        #------------draw final coordinate-----------------
        if(drawText and len(send_data) > 0):
            final_x, final_y, final_z = finalCoor(send_data[0])
            save_aruco2csv.savetocsv(id,x,y,z,yaw,frame,True)
            draw(final_x,final_y,final_z,float(send_data[0]['yaw']),send_data[0]['id'],frame,250)
        #-------------print fps-----------------
        count += 1
        if(count%30 == 0):
            end_time = cv2.getTickCount()
            print('FPS: ', 30/((end_time - start_time)/cv2.getTickFrequency()))
            for aruco in arucoList:
                print(aruco.id,aruco.getCoordinate())
            start_time = cv2.getTickCount()
        #-------------show frame-----------------
        if(showImage):
            cv2.imshow('frame', frame)
        if cv2.waitKey(1) == ord('q'):
            break
        #-------------store vedio-----------------
        if(outVedio):
            out.write(frame)

def draw(x, y, z, yaw, id, frame, image_y):
    text = 'id: {}, x: {:.2f}, y: {:.2f}, z: {:.2f}, yaw: {:.2f}'.format(id, x, y, z, yaw)
    cv2.putText(frame, text, (20,image_y), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 255), 2)

def rvec_to_euler_angles(rvec):
    rvec_flipped = rvec[0][0]*-1 

    R_mat, jacobian=cv2.Rodrigues(rvec_flipped)

    pitch = math.atan2(R_mat[2,1],R_mat[2,2])
    sy = math.sqrt((R_mat[0][0]*R_mat[0][0])+ (R_mat[1][0]*R_mat[1][0]))
    singular = sy<1e-6

    if not singular:
        pitch = math.atan2(R_mat[2,1],R_mat[2,2])
        roll = math.atan2(-R_mat[2,0],sy)
        yaw = math.atan2(R_mat[1,0],R_mat[0,0])
    else:
        pitch = math.atan2(-R_mat[1,2],R_mat[1,1])
        roll = math.atan2(-R_mat[2,0],sy)
        yaw = 0
    return yaw, pitch, roll



if __name__ == '__main__':
    main()
