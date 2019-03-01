import paho.mqtt.client as mqtt
import os
from urllib.parse import urlparse
import secrets
import django
django.setup()
from django.contrib.auth.models import User
from aifarmer.models import Sensor, SensorsUsers, Temperature, Humidity



stop_dev=False
def on_connect(client, userdata, flags, rc):
    print("rc: " + str(rc))

def on_message(client, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

    mess = msg.payload.decode()
    top = msg.topic
    if len(top)>63:
        tok_loc=top[0:top.index('/')]
        if top.endswith('T'):
            sens = Sensor.objects.get(token_sensor=tok_loc.strip())
            t = Temperature(sensor_temperature=sens, temperature_read=float(mess))
            t.save()
        elif top.endswith('H'):
            sens = Sensor.objects.get(token_sensor=tok_loc.strip())
            h = Humidity(sensor_humidity=sens, humidity_read=float(mess))
            h.save()


def on_publish(client, obj, mid):
    #print("mid: " + str(mid))
    a=1

def existence_callback(client,obj,message):
    mess = message.payload.decode()
    dev_name = mess[0:mess.index('/')]
    print('device-name:'+ dev_name)
    dev_pin = mess[mess.index('/')+1: len(mess)]
    print('device-pin:' + dev_pin)

    if Sensor.objects.filter(name_sensor=dev_name).count() == 0:
        client.subscribe(dev_name+'/REG',0) # to be splitted in dev-name and pin
        client.message_callback_add(dev_name+'/REG',registration_callback)
        tok = secrets.token_hex(32)
        sensor = Sensor(name_sensor=dev_name,pin_sensor=dev_pin,token_sensor=tok,latitude_sensor=45,longitude_sensor=45)
        sensor.save()


    # send dev_name, dev_pin, GENERATE token and write DB.
    #                           <==> it doesn't already exist!!!


def registration_callback(client,obj,message):
    mess = message.payload.decode()
    print('?? HERE ??')
    top = str(message.topic)
    if mess == '?':
        print('?!!HERE !!')
        dev_name = top[0:top.index('/')]
        print(dev_name)
        Sens=Sensor.objects.filter(name_sensor=dev_name)
        print(len(Sens))
        for sen in Sens:
            tok_loc = str(sen.token_sensor)
            client.publish(str(message.topic),tok_loc)
            print('!!SUB-T !!')
            client.subscribe(tok_loc+'/T',0)
            print('!!SUB-H!!')
            client.subscribe(tok_loc+'/H',0)



def on_subscribe(client, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(client, obj, level, string):
    print(string)


mqttc = mqtt.Client()
# Assign event callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_subscribe = on_subscribe

# Uncomment to enable debug messages
#mqttc.on_log = on_log

# Parse CLOUDMQTT_URL (or fallback to localhost)
#url_str = 'mqtt://try:try@broker.shiftr.io:1883'
url_str = 'mqtt://astroteo:Gold4Himself@mqtt.aifarmer.du.cdr.mn:8883'
url = urlparse(url_str)
print(url.hostname)
print(url.username)
print(url.password)
print(url.port)


sub_topics= [('NEW-DEV',0)]
#pub_topics = ['DEV-42/ACT','DEV-43/ACT']
#pub_contents = ['0','1']


# Connect
mqttc.username_pw_set(url.username, url.password)
mqttc.connect(url.hostname, url.port)

# Start subscribe, with QoS level 0
mqttc.subscribe(sub_topics)
mqttc.message_callback_add('NEW-DEV',existence_callback)
# Continue the network loop, exit when an error occurs
rc = 0
count=0
while rc == 0:
    rc = mqttc.loop()
