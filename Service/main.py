import os,subprocess
import serial
import time
DEVICES = [56,# controller
           58,# builtin stereo
           60,# pci 4 channel
           ]
def pop_notif(title, message):
    subprocess.run(["notify-send", "-i", "dialog-info", f"{title}", f"{message}"])   
def get_dev_vol(device):
    if not device in DEVICES:
        raise IndexError("DEVICE NOT FOUND")
    result = subprocess.run(
    ['pactl', 'get-sink-volume', str(device)],  # Command as a list
    capture_output=True,
    text=True
    )   
    result = result.stdout.split(" / ")
    result = int(result[1][:-1])
    #print(result)
    return result
def set_dev_vol(device:int, volume:int):
    if not device in DEVICES:
        raise IndexError("DEVICE NOT FOUND")
    result = subprocess.run(
    ['pactl', 'set-sink-volume', str(device), f"{volume}%"],  # Command as a list
    capture_output=True,
    text=True
    )   
    #print(result)
    return result
def write_dev_volume(device:int, counted_from=101):
    ser.write(b'W0')
    ser.flush()
    print("WRITE - W0")
    readyword = "#"
    word = ""
    counter = counted_from-1
    while not ser.in_waiting > 0:
        if counter > 0:
            time.sleep(.5)
            counter-=1
            print(ser.in_waiting, "failing")
        else:
            print("exit")
            return;
        pass
    while ser.in_waiting or (not word == readyword):
        print(ser.in_waiting, "winning?")
        char = ser.read().decode()
        if char in readyword:
            word=word+char
        if word == readyword:
            break
        pass
    ackowlegdement = word
    if ackowlegdement == readyword:
        v = str(get_dev_vol(device)).encode()
        ser.write(v)
        ser.flush()
        print("WRITE - " + v.decode())
        while not ser.in_waiting > 0:
            pass
        print(ser.read_all().decode())
        clear_serial()
def clear_serial():
    while ser.in_waiting:
        print(ser.read())
        time.sleep(0.1)

def read_new_volume(device:int):
    counter = 100
    while not (ser.in_waiting == 2) and counter > 0:
        counter -= 1
    if not ser.in_waiting == 2:
        return
    msg = ser.read(2)
    if msg.decode() == "0F":
        print("READ - 0F")
        print("WRITE - #")
        ser.write(b"#")
        ser.flush()
    else:
        return
    counter = 100
    while ser.in_waiting < 2 and counter > 0:
        counter-=1
    if counter <= 0:
        print("return")
        return
    data = ser.read(ser.in_waiting)
    print(data)
    data = data.decode()
    if int(data) > 1001:
        data = data[:-1]
    data = int(data)/10
    set_dev_vol(device, data)

#get_dev_vol(56)
get_dev_vol(58)
get_dev_vol(60)
set_dev_vol(56, 50)
ser = serial.Serial('/dev/ttyACM0',9600)
print("Startup! sleep for 2 seconds!")
time.sleep(2)
print("passed!")
#write_dev_volume(60)
#clear_serial()
#time.sleep(1)
#clear_serial()
lastvols = [0,0,0]
while True:
    i = 2
    vol = lastvols[i]
    v = get_dev_vol(DEVICES[i])
    if not (vol == v):
        write_dev_volume(DEVICES[i])
        clear_serial()
        lastvols[i] = v
    read_new_volume(DEVICES[i])
