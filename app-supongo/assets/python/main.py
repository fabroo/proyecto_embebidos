import cv2
import sys
import pusher
from time import sleep

cascPath = "assets/python/haarcascade_frontalface_default.xml"
faceCascade = cv2.CascadeClassifier(cascPath)

video_capture = cv2.VideoCapture(2)

pusher_client = pusher.Pusher(
    app_id = "1082208",
    key = "b103ad2b1e20a1198455",
    secret = "5ddd5781b85de3eed2d7",
    cluster = "us2",
    ssl=True
)

while True:
    # Capture frame-by-frame
    ret, frame = video_capture.read()

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    faces = faceCascade.detectMultiScale(
        gray,
        scaleFactor=1.1,
        minNeighbors=5,
        minSize=(30, 30),
        flags=cv2.CASCADE_SCALE_IMAGE
    )
    
    if(len(faces) > 0) :
        cv2.imwrite("assets/sendImage.png", frame)
        pusher_client.trigger('monomon', 'faceDetectedPython',{})
        break

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything is done, release the capture
video_capture.release()
cv2.destroyAllWindows()