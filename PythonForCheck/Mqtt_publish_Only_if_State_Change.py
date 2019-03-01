import paho.mqtt.client as mqtt
import os
from urllib.parse import urlparse


before_list = [False,True,False]
BEF_list = before_list
now_list=[False,False,False]



message = None
# Define event callbacks
def on_connect(client, userdata, flags, rc):
    print("rc: " + str(rc))

def on_message(client, obj, msg):
    global message
    #print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    message = msg.payload

def callback_update_list(client, obj, msg):

    client.subscribe('DEV-1',0)

    # pick from database instead of mqtt transmit
    if msg.topic =='DEV-1' and msg.payload.decode()=='1':
        now_list[2]=True
        print(now_list)
    # pick from database instead of mqtt transmit
    if msg.topic =='DEV-1' and msg.payload.decode()=='0':
        now_list[2] = False
        print(now_list)





def on_publish(client, obj, mid):
    #print("mid: " + str(mid))
    a=2

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

#sub_topic = 'devices/+/temperature'
#pub_topic = 'devices/1/temperature'

sub_topic = 'NEW-DEV'
pub_topic = 'DEV-1'


# Connect
mqttc.username_pw_set(url.username, url.password)
mqttc.connect(url.hostname, url.port)

# Start subscribe, with QoS level 0
mqttc.subscribe(sub_topic, 0)

mqttc.message_callback_add('NEW-DEV',callback_update_list)

# Publish a message
#mqttc.publish(pub_topic, " hello from The good-Old-Python")

# Continue the network loop, exit when an error occurs
rc = 0
while rc == 0:
      rc = mqttc.loop()#-> it's like the subsciption to the channel [mosquitto_sub on server
      print(now_list)
      print(before_list)

      print(now_list[2]-before_list[2])

      if (now_list[2]-before_list[2])==1:
          mqttc.publish('DEV-1/ACT',1)
          print('now-list-->'+str(now_list))
          print('bef-list-->'+str(before_list))
          before_list[2] = now_list[2]

      elif (now_list[2]-before_list[2])==-1:
          mqttc.publish('DEV-1/ACT', 0)
          print('now-list-->' + str(now_list))
          print('bef-list-->' + str(before_list))
          before_list[2] = now_list[2]







