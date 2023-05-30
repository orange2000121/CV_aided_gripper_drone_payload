import cv2

# Initialize the camera capture objects
cap1 = cv2.VideoCapture(0)  # First USB camera
cap2 = cv2.VideoCapture(1)  # Second USB camera

# Check if camera initialization was successful
if not cap1.isOpened() or not cap2.isOpened():
    print("Failed to open one or more cameras.")
    exit()

# Loop for capturing and displaying frames from the cameras
while True:
    # Read frames from the cameras
    ret1, frame1 = cap1.read()
    ret2, frame2 = cap2.read()

    # Check if frames are captured successfully
    if not ret1 or not ret2:
        print("Failed to capture frames from one or more cameras.")
        break

    # Display the frames in separate windows
    cv2.imshow('Camera 1', frame1)
    cv2.imshow('Camera 2', frame2)

    # Exit loop if 'q' key is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the camera capture objects and close windows
cap1.release()
cap2.release()
cv2.destroyAllWindows()
