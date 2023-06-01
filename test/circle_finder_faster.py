import json
import cv2
import numpy as np
import time


videoSource = 0	# webcam
# videoSource = 'big fire.mp4'
sampleRange = 3
# minRadius = 10
#resolution = (640, 480)
resolution = (320, 240)
circlesList = [None] * sampleRange

def FindCircle(img, circlesList):
	img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
	img = cv2.medianBlur(img, 11)

	maxRadius = min(resolution[0], resolution[1])
	minRadius = int(70*(resolution[0]/640))
	crosscornerLength = int((resolution[0]**2 + resolution[1]**2) ** 0.5)
	minDist = (crosscornerLength + maxRadius * 2) + 1
	#param1 = int(40*(resolution[0]/640))
	param2 = int(60*(resolution[0]/640))
	#print(maxRadius, minRadius, minDist)
	circles = cv2.HoughCircles(img, cv2.HOUGH_GRADIENT, 1, minDist = minDist, param1 = 40, param2 = param2, minRadius = minRadius, maxRadius = maxRadius)
	if circles is not None:
		circles = np.uint16(np.around(circles))
		for circle in circles[0, : ]:
			circlesList.append(circle.tolist())
			break
	else:
		circlesList.append(None)
	circlesList.pop(0)

	return circlesList

def Supression(circlesList, sampleThresh: int, maxRadiusDiff: float = 0.5, maxPositionDiff: int = int(60*(resolution[0]/640))):
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
			cv2.circle(img, (circle[0], circle[1]), 2, (0, 0, 255), -1)
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

def QQcircle(frame,mc):
	global circlesList

	frame = cv2.resize(frame, resolution, interpolation = cv2.INTER_AREA)
	sampleThresh = RetriveThresh(sampleRange)
	circlesList = FindCircle(frame, circlesList)
	supressedCircles, lastestCircle = Supression(circlesList, sampleThresh)
	circleImg = DrawPath(frame.copy(), supressedCircles)

	sendCircle = []
	if supressedCircles is not None:
		lastCenter = None
		for circle in supressedCircles:
			if lastCenter is not None:
				x_coor,y_coor=pixel2meter(circle[0],circle[1],circle[2])
				mc.set('x_coor',str(x_coor),)
				mc.set('y_coor',str(y_coor))
				print('{:0.3f}'.format(x_coor),'{:0.3f}'.format(y_coor))
				cv2.line(circleImg, lastCenter, (circle[0], circle[1]) ,(0, 255, 255), 1)
			lastCenter = (circle[0], circle[1])
			x_coor,y_coor=pixel2meter(circle[0],circle[1],circle[2])
			print('{:0.3f}'.format(x_coor),'{:0.3f}'.format(y_coor))
			mc.set('x_coor',str(x_coor),)
			mc.set('y_coor',str(y_coor))
			temp = {
				'x_coor' : x_coor,
				'y_coor' : y_coor
			}
			sendCircle.append(temp)
			# cv2.putText(circleImg, 'x: '+str(x_coor), (10, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1, cv2.LINE_AA)
			# cv2.putText(circleImg, 'y: '+str(y_coor), (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1, cv2.LINE_AA)
	else:
		# print('no circle')
		mc.set('x_coor',"0")
		mc.set('y_coor',"0")
	if(sendCircle != []):
		mc.set('circle',json.dumps(sendCircle[0]))
	else:
		mc.set('circle',json.dumps({'x_coor':0,'y_coor':0}))
	mc.set('circle_time', cv2.getTickCount())
	# cv2.imshow('Circle', circleImg)
	if cv2.waitKey(1) == 'q':
		cap.release()
		cv2.destroyAllWindows
	return circleImg

def pixel2meter(x_pxl, y_pxl,r):
	image_width = 320 
	image_height = 240 
	pxl2cmold = 75.0 / 204.0
	pxl2cm = 75.0 / (r*2)

	imag2circle_center_x = (image_width / 2 - x_pxl)*-1
	imag2circle_center_y = image_height / 2 - y_pxl

	x_pos = float(imag2circle_center_x) * pxl2cm / 100
	y_pos = float(imag2circle_center_y) * pxl2cm / 100

	return x_pos, y_pos




if __name__ == '__main__':
	cap = cv2.VideoCapture(videoSource)
	#videoWriter = cv2.VideoWriter('out3_3.mp4', cv2.VideoWriter_fourcc(*'MP4V'), fps = 20, frameSize = resolution)
	
	circlesList = [None] * sampleRange

	frameCount = 0
	programExcutionTime = 0.0

	while cap.isOpened():
		startTime = time.time()

		ret, frame = cap.read()
		if not ret: break
		frame = cv2.resize(frame, resolution, interpolation = cv2.INTER_AREA)
		frameCount += 1

		sampleThresh = RetriveThresh(sampleRange)

		circlesList = FindCircle(frame, circlesList)

		supressedCircles, lastestCircle = Supression(circlesList, sampleThresh)

		circleImg = DrawPath(frame.copy(), supressedCircles)

		deltaTime = time.time() - startTime
		programExcutionTime += deltaTime
		frameRate = 1.0 / deltaTime
		cv2.putText(circleImg, 'FPS: ' + str(round(frameRate, 0)), (10, resolution[1]-10), cv2.FONT_HERSHEY_COMPLEX, 0.7, (0, 255, 0), 1)

		#cv2.imshow('Frame', frame)
		cv2.imshow('Circle', circleImg)

		#videoWriter.write(circleImg)

		if cv2.waitKey(1) == 'q':
			break

	averageFrameRate = frameCount / programExcutionTime
	print('Average Rate:', averageFrameRate)

	cap.release()
	#videoWriter.release()
