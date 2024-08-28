# Autonomous Robotic Device [*GRADUATION PROJECT*]

## Description

An **autonomous robotic device** - is a robot that able to perform tasks and make decisions without constant management Man. For the perception of the environment by an autonomous robot - on it sensors, lasers, scanners, microphones, cameras, radars and lidars. These, in combination with the appropriate software, are able to identify and classify objects, their location, ambient temperature, and more, and, in on the information collected, to make appropriate actions.   

The purpose of the graduation project was to develop a model of a robot direction finder with radio signal detection and tracking. Such a project is necessary to test the idea of using a direction finding system in order to In the future, implement a full-scale robot prototype that will directly carry out military tasks.

## Hardware components

- Arduino Uno 
- Raspberry Pi 4 (8Gb RAM)
- Servo MG995
- Voltage converter DC-DC LM2596S
- TinyUPS
- Battery 18650
- WS2812

## Used software

- Arduino IDE (C++) [![arduino-logo-0-1904931468.png](https://i.postimg.cc/xCXBF2bX/arduino-logo-0-1904931468.png)](https://postimg.cc/0Mqtbhnv)
- Geany programmers editor (Python)
- Kompas-3D
- Altium Designer 23

## The control algorithm 

The power system is started through a switch. After her The LED modules on the front of the robot are turned on. Then the Raspberry Pi and Arduino Uno controllers are initialized, and as well as the components connected to them in the form of an RPi camera module camera. The movement of the robot is carried out along a pre-drawn black line to demonstrate the capabilities of computer vision within the of this device layout. 

A prerequisite for starting the script execution on microcomputer is the detection of a black line in the coverage area Camera. If the module does not detect a black line, it is necessary to restart the Raspberry Pi. The image from the camera is processed with neural masks separating the black line from the surface, which allows you to follow her. Depending on the position of the black line relative to the field of view of the RPi camera, an error is sent on Serial Interface to the Arduino Uno board.

## Code explanation

Camera initialization where you can specify the image dimensions (*width & height*) and format.
```python
camera = Picamera2()
camera.configure(camera.create_preview_configuration(main={"format": 'XRGB8888', "size": (300, 180)}))
camera.start()
```

Hard coded values 
```python
x_last = 160
y_last = 90
ang = 10
error = 7
```

This section specifies the transmission port of the Arduino Uno and the baud rate - **standard 9600**
```python
ser = serial.Serial('/dev/ttyACM0', 9600) # Replace '/dev/ttyACM0' to your own Arduino Uno port 
```
