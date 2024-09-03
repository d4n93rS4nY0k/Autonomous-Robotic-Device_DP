import RPi.GPIO as GPIO
from picamera2 import Picamera2, Preview
import time
import serial
import cv2
import numpy as np
from lib_nrf24 import NRF24
import time
import spidev

GPIO.setmode(GPIO.BCM)
###############################################################################################################################################################################################
###################################################################	CAMERA INIT	#######################################################################################################
###############################################################################################################################################################################################
camera = Picamera2()
camera.configure(camera.create_preview_configuration(main={"format": 'XRGB8888', "size": (300, 180)}))
camera.start()

x_last = 160
y_last = 90
ang = 10
error = 7

ser = serial.Serial('/dev/ttyACM0', 9600) # Replace '/dev/ttyACM0' to your own Arduino Uno port 

pipes = [[0xE8, 0xE8, 0xF0, 0xF0, 0xE1], [0xF0, 0xF0, 0xF0, 0xF0, 0xE1]]

radio = NRF24(GPIO, spidev.SpiDev())
radio.begin(0,17)

radio.setPayloadSize(32)
radio.setChannel(0x76)
radio.setDataRate(NRF24.BR_1MBPS)
radio.setPALevel(NRF24.PA_MIN)

radio.setAutoAck(True)
radio.enableDynamicPayloads()
radio.enableAckPayload()

radio.openWritingPipe(pipes[0])

radio.openReadingPipe(1, pipes[1])
radio.printDetails()

text = ['0',' ','1',' ','0',' ','1',' ','0',' ','0',' ','0',' ','1']

with open('exampleData.txt', 'w') as f:
	for line in text:
		f.write(line)
		f.write('\n')

with open('exampleData.txt') as f:
    lines = f.read().splitlines()
    print(lines)
    
if len(lines) < 32:
    lines.append(0)

while True:
    
    camera.capture_array()

    frame = camera.capture_image()
    image = camera.capture_array()

    height = 180
    width = 300
    center_x = int(300 / 2)
    center_y = int(180 / 2)
    radius = int(65)
    
    mask_black = np.zeros((height, width), dtype=np.uint8)
    cv2.rectangle(mask_black, (0, 0), (300, 180), 255, -1)
    center_frame_black = cv2.bitwise_and(image, image, mask=mask_black)
    
    black_min = (60,0,0)
    black_max = (200,255,50)
    
    Blackline = cv2.inRange(cv2.cvtColor(image, cv2.COLOR_BGR2HSV), black_min, black_max)
    kernel = np.ones((3,3), np.uint8)
    Blackline = cv2.erode(Blackline, kernel, iterations=2)
    Blackline = cv2.dilate(Blackline, kernel, iterations=9)
    contours_blk, hierarchy_blk = cv2.findContours(Blackline.copy(),cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    contours_blk_len = len(contours_blk)
    
    if contours_blk_len > 0:
        if contours_blk_len == 1:
            blackbox = cv2.minAreaRect(contours_blk[0])
        else:
            candidates = []
            off_bottom = 0
            for con_num in range (contours_blk_len):
                contour_area = cv2.contourArea(contours_blk[con_num])
                if contour_area > 5000:
                    blackbox = cv2.minAreaRect(contours_blk[con_num])
                    (x_min, y_min), (w_min, h_min), ang = blackbox
                    box = cv2.boxPoints(blackbox)
                    (x_box, y_box) = box[0]
                    if y_box > 178:
                        off_bottom +=1;
                    candidates.append((y_box,con_num,x_min,y_min))
                    candidates = sorted(candidates)
            if off_bottom > 1:
                candidates_off_bottom = []
                for con_num in range ((contours_blk_len - off_bottom), contours_blk_len):
                    (y_highest, con_highest, x_min, y_min) = candidates[con_num]
                    total_distance = (abs(x_min - x_last)**2 + abs(y_min - y_last)**2)**0.5
                    candidates_off_bottom.append((total_distance,con_highest))
                candidates_off_bottom = sorted(candidates_off_bottom)
                (total_distance, con_highest) = candidates_off_bottom[0]
                blackbox = cv2.minAreaRect(contours_blk[con_highest])
    (x_min, y_min), (w_min, h_min), ang = blackbox
    x_last = x_min
    y_last = y_min
    if ang < -45 :
        ang = 90 + ang
    if w_min < h_min and ang > 0:	  
        ang = (90 - ang)*-1
    if w_min > h_min and ang < 0:
        ang = 90 + ang
    setpoint = 160 #320 centre of the screen
    error = int(setpoint - x_min) 
    ang = int(ang)
    
    box = cv2.boxPoints(blackbox)
    box = np.int0(box)
    cv2.drawContours(image,[box],0,(0,0,255),3)
    cv2.putText(image,str(ang),(10, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 1)
    cv2.putText(image,str(error),(10, 180), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 1)
    cv2.line(image, (int(x_min),100 ), (int(x_min),150 ), (255,0,0),3)
###############################################################################################################################################################################################
###################################################################	ERROR SEND TO ARDUINO	###############################################################################################
###############################################################################################################################################################################################
    if True:
        angle_value_list = [str(error) + "."]
        send_string = ','.join(angle_value_list)
        send_string += "\n"
        ser.write(send_string.encode('utf-8'))
	    
    start = time.time()
    radio.write(lines)
    print("Sent the message: {}".format(lines))
    time.sleep(0.1)
    
    cv2.imshow("Frame", image)	
    key = cv2.waitKey(1) & 0xFF	
    if key == ord("q"):
        break
