import json
import RPi.GPIO as GPIO
import time
import memcache

trigger_pin = 23
echo_pin = 24

GPIO.setmode(GPIO.BCM)
GPIO.setup(trigger_pin, GPIO.OUT)
GPIO.setup(echo_pin, GPIO.IN)

def send_trigger_pulse():
    GPIO.output(trigger_pin, True)
    time.sleep(0.001)
    GPIO.output(trigger_pin, False)

def wait_for_echo(value, timeout):
    count = timeout
    while GPIO.input(echo_pin) != value and count > 0:
        count = count - 1

def get_distance():
    send_trigger_pulse()
    wait_for_echo(True, 5000)
    start = time.time()
    wait_for_echo(False, 5000)
    finish = time.time()
    pulse_len = finish - start
    distance_m = pulse_len * 340/2
    return distance_m

if __name__ == '__main__':
    mc = memcache.Client(['127.0.0.1:11211'], debug=True)
    while True:
        print("m=%f\t" % get_distance())
        distance = get_distance()
        json_string = json.dumps([{'sonar': distance}])
        mc.set("sonar", json_string)
        time.sleep(0.2)