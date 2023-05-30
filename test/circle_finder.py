import cv2
import numpy as np
import sys
import platform





size =185
pxl2coor=75/185
# src = cv2.VideoCapture('bigfire.mp4')
src = cv2.VideoCapture(0)
counter=0
count = 0
start_time = cv2.getTickCount()

while(True):
	#ret, frame =cv2.read()
	#frame=picam.capture_array()
	ret,image = src.read()
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

	if circles is not None:
		# convert the (x, y) coordinates and radius of the circles to integers
		circlesRound = np.round(circles[0, :]).astype("int")
		# loop over the (x, y) coordinates and radius of the circles
		for (x, y, r) in circlesRound:
			cv2.circle(output, (x, y), r, (0, 255, 0), 4)
			counter=counter+1
			if counter == 5:
				print("drop")
				counter=0
			# coorx = x*pxl2coor
			# coory = y*pxl2coor

			# pixel = f"x={x} y={y}"
			# coor = f"coorx={coorx:.2f} coory={coory:.2f}"
			print(x,y,counter)
			# cv2.putText(img=output, text=pixel, org=(20, 420), fontFace=cv2.FONT_HERSHEY_TRIPLEX, fontScale=0.7,
			# 			color=(0, 180, 0), thickness=2)
			# cv2.putText(img=output, text=coor, org=(20, 450), fontFace=cv2.FONT_HERSHEY_TRIPLEX, fontScale=0.7,
			# 			color=(0, 0, 180), thickness=2)
			count += 1
			if(count%30 == 0):
				end_time = cv2.getTickCount()
				print('FPS: ----------------', 30/((end_time - start_time)/cv2.getTickFrequency()))
				start_time = cv2.getTickCount()
	cv2.imshow('ImageWindow', output)
	if cv2.waitKey(1) == ('q'):
		break





