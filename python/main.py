import time
from typing import Collection
import cv2
import sys
import requests
from time import sleep
import boto3
import warnings
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore

cred = credentials.Certificate('firebase.json')
firebase_admin.initialize_app(cred)

warnings.filterwarnings("ignore")

cascPath = "haarcascade_frontalface_default.xml"
faceCascade = cv2.CascadeClassifier(cascPath)
checking = True
client = boto3.client('rekognition')
url = "192.168.0.117"
print("Detectando caras.")

while True:
    video_capture = cv2.VideoCapture(2, cv2.CAP_DSHOW)

    while checking:
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

        if(len(faces) > 0):
            
            for (x, y, w, h) in faces:
                if w > 100 and h > 100:
                    cv2.imwrite("../sendImage.png", frame)
                    checking = False

        cv2.imshow('my webcam', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # When everything is done, release the capture
    video_capture.release()
    cv2.destroyAllWindows()
    print("Cara detectada, intentando reconocer.")

    class RekognitionImage:

        def __init__(self, image, image_name, rekognition_client):

            self.image = image
            self.image_name = image_name
            self.rekognition_client = rekognition_client

        @classmethod
        def from_file(cls, image_file_name, rekognition_client, image_name=None):
            with open(image_file_name, 'rb') as img_file:
                image = {'Bytes': img_file.read()}
            name = image_file_name if image_name is None else image_name
            return cls(image, name, rekognition_client)

        def search_faces(self):
            res = client.search_faces_by_image(
                CollectionId="1a2b3c",
                Image=self.image

            )
            faces = res["FaceMatches"]
            arr = []
            for face in faces:
                arr.append(face['Face']["ExternalImageId"])
            if len(list(set(arr))) == 1:
                dni = arr[0]
                db = firestore.client()
                sn = db.collection(u'users').where(u'dni', u'==', dni).stream()

                for doc in sn:
                    name = doc.to_dict()['nombre']

                print("Cara reconocida, " + name)
                print("Use su llave rfid para ingresar.")
                requests.post("http://" + url + "/openGate", json={
                    "rfidList": ["algunos valores", "que hardcodearemos"],
                    "name": name
                })

    image = RekognitionImage.from_file('../sendImage.png', client)
    image.search_faces()
    time.sleep(10)
