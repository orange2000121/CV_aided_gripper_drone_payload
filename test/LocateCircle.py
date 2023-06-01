import time
import cv2
import numpy as np


#videoSource = 0	# webcam
# videoSource = 'output_vedio/2023-05-22-17-29-43.mp4'
sampleRange = 3
minRadius = 10



def FindCircle(img, circlesList):
	img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
	img = cv2.medianBlur(img, 9)

	maxRadius = 240
	minDist = (800 + maxRadius * 2) + 1
	circles = cv2.HoughCircles(img, cv2.HOUGH_GRADIENT, 1, minDist = minDist, param1 = 40, param2 = 60, minRadius = 70, maxRadius = maxRadius)
	if circles is not None:
		circles = np.uint16(np.around(circles))
		for circle in circles[0, : ]:
			circlesList.append(circle.tolist())
			break
	else:
		circlesList.append(None)
	circlesList.pop(0)

	return circlesList

def Supression(circlesList, sampleThresh: int, maxRadiusDiff: float = 0.5, maxPositionDiff: int = 60):
	listLength = len(circlesList)
	candidatesList = [None] * listLength

	for circle in circlesList:
		if circle is not None:
			for i in range(len(candidatesList)):
				candidate = candidatesList[i]
				if candidate is not None:
					lastCircle = candidate[-1]
					radiusDiff = abs(float(lastCircle[2] - circle[2]) / float(circle[2]))
					positionDiff = Distance((circle[0], circle[1]), (lastCircle[0], lastCircle[1]))
					if radiusDiff < maxRadiusDiff and positionDiff < maxPositionDiff:
						candidatesList[i].append(circle)
						break
				else:
					candidatesList[i] = [circle]
					break

	#print(candidatesList)
	temp = []
	for candidate in candidatesList:
		if candidate is not None:
			temp.append(candidate)
	candidatesList = temp
	candidatesList = sorted(candidatesList, key = lambda x: len(x), reverse = True)
	if len(candidatesList) > 0 and len(candidatesList[0]) > sampleThresh:
		return candidatesList[0], candidatesList[0][-1]
	else:
		return None, None

def DrawPath(img, circles):
	if circles is not None:
		lastCenter = None
		for circle in circles:
			cv2.circle(img, (circle[0], circle[1]), circle[2], (0, 255, 0), 2)
			cv2.circle(img, (circle[0], circle[1]), 2, (0, 0, 255), 3)
			if lastCenter is not None:
				cv2.line(img, lastCenter, (circle[0], circle[1]) ,(0, 255, 255), 1)
			lastCenter = (circle[0], circle[1])
	return img

def RetriveThresh(sampleRange: int, confidence: float = 0.6):
	thresh = int(float(sampleRange) / 2.0)
	while float(thresh) / float(sampleRange) < confidence:
		thresh += 1

	return thresh

def Distance(p1, p2):
	dis = (((p2[0] - p1[0]) ** 2) + ((p2[1] - p1[1]) ** 2)) ** 0.5
	return dis

# def findcircle(frame,mc):
# 	circlesList = [None] * sampleRange 
# 	sampleThresh = RetriveThresh(sampleRange)
# 	circlesList = FindCircle(frame, circlesList)
# 	supressedCircles, lastestCircle= Supression(circlesList, sampleThresh)
	
# 	print('x = ' ,type(supressedCircles), 'y =' ,[circlesList][0][0])

# 	# if( supressedCircles != None):
# 	# 	x_pxl = supressedCircles[0]
# 	# 	y_pxl = supressedCircles[1]
# 	# 	x_coor,y_coor=pixel2meter(270,510)
# 	# 	mc.set('x_coor',x_coor)
# 	# 	mc.set('y_coor',y_coor)
# 	# 	#print('x = ' ,x_coor, 'y =' ,y_coor)
# 	# else:
# 	# 	mc.set('x_coor',"null")
# 	# 	mc.set('y_coor',"null")


def pixel2meter(x_pxl, y_pxl,r):
	image_width = 320 * 2
	image_height = 240 * 2
	pxl2cmold = 75.0 / 204.0
	pxl2cm = 75.0 / (r*2)


	# Input variables
	y_pxl2 = image_height - y_pxl


	imag2circle_center_y = (image_width / 2 - x_pxl)*-1
	imag2circle_center_x = image_height / 2 - y_pxl

	x_pos = float(imag2circle_center_x) * pxl2cm / 100
	y_pos = float(imag2circle_center_y) * pxl2cm / 100

	return y_pos, x_pos




def quickcircle(frame,mc):
	image_width = 240 * 2
	image_height = 320 * 2
	size =204
	# src = cv2.VideoCapture('bigfire.mp4')
	# src = cv2.VideoCapture(0)
	image =frame
	counter=0
	count = 0
	start_time = cv2.getTickCount()

	# while(True):
		#ret, frame =cv2.read()
		#frame=picam.capture_array()
		# ret,image = src.read()
	output = image.copy()
	height, width = image.shape[:2]
	maxRadius = int(1.1*(size)/2)
	minRadius = int(0.9*(size)/2)
	gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
	circles = cv2.HoughCircles(image=gray,
							method=cv2.HOUGH_GRADIENT,
							dp=1.2,
							minDist=6*minRadius,
							param1=60,
							param2=47,
							minRadius=minRadius,
							maxRadius=maxRadius
							)
	# cv2.line(output, (320,0), (320,480), (255,255,0),1)
	# cv2.line(output, (0,240), (640,240), (255,255,0),1)

	if circles is not None:
		circlesRound = np.round(circles[0, :]).astype("int")
		for (x, y, r) in circlesRound:
			# cv2.circle(output, (x, y), r, (0, 255, 0), 4)
			counter=2
			if counter == 2:
				counter=0
				x_coor,y_coor=pixel2meter(x,y,r)
				print(x_coor,y_coor)
				mc.set('x_coor',str(x_coor),)
				mc.set('y_coor',str(y_coor))
				# print('x pxl = ' ,x, '	y pxl =' ,y,' XCOOR = ' ,x_coor,'	YCOOR = ',y_coor)
			else:
				print('no circle')
				mc.set('x_coor',"0")
				mc.set('y_coor',"0")
					
			# coorx = x*pxl2coor
			# coory = y*pxl2coor

			# pixel = f"x={x} y={y}"
			# coor = f"coorx={coorx:.2f} coory={coory:.2f}"
			# print(x,y,counter)
			# cv2.putText(img=output, text=pixel, org=(20, 420), fontFace=cv2.FONT_HERSHEY_TRIPLEX, fontScale=0.7,
			# 			color=(0, 180, 0), thickness=2)
			# cv2.putText(img=output, text=coor, org=(20, 450), fontFace=cv2.FONT_HERSHEY_TRIPLEX, fontScale=0.7,
			# 			color=(0, 0, 180), thickness=2)
			# count += 1
			# if(count%30 == 0):
			# 	end_time = cv2.getTickCount()
			# 	print('FPS: ----------------', 30/((end_time - start_time)/cv2.getTickFrequency()))
			# 	start_time = cv2.getTickCount()
		# cv2.imshow('ImageWindow', output)
		# if cv2.waitKey(1) == ('q'):
		# 	break
	else:	
		print('no circle')
		mc.set('x_coor',"0")
		mc.set('y_coor',"0")

# if __name__ == '__main__':
# 	import memcache
# 	cap = cv2.VideoCapture(0)
# 	#videoWriter = cv2.VideoWriter('out1.mp4', cv2.VideoWriter_fourcc(*'MP4V'), fps = 20, frameSize = (640, 480))
	
# 	circlesList = [None] * sampleRange 
# 	start_time = time.time()
# 	count = 0
# 	mc = memcache.Client(['127.0.0.1:11211'], debug=True)
# 	while cap.isOpened():
# 		ret, frame = cap.read()
# 		if not ret: break
# 		findcircle(frame,mc)

# 		sampleThresh = RetriveThresh(sampleRange)

# 		circlesList = FindCircle(frame, circlesList)

# 		supressedCircles, lastestCircle = Supression(circlesList, sampleThresh)
		
# 		circleImg = DrawPath(frame.copy(), supressedCircles)
# 		count += 1
# 		end_time = time.time()
# 		fps = count / (end_time - start_time)
# 		print(fps)
# 		cv2.putText(circleImg, "FPS: " + str(int(fps)), (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 255), 2)
# 		cv2.imshow('Frame', frame)
# 		cv2.imshow('Circle', circleImg)

# 		#videoWriter.write(circleImg)

# 		if cv2.waitKey(1) == 'q':
# 			break

# 	cap.release()
# 	#videoWriter.release()
