import paho.mqtt.client as mqtt
import os
from urllib.parse import urlparse

message = b''
# Define event callbacks
def on_connect(client, userdata, flags, rc):
    print("rc: " + str(rc))

def on_message(client, obj, msg):
    global message
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    message = msg.payload

def get_message():
    return message;




def on_publish(client, obj, mid):
    print("mid: " + str(mid))

def on_subscribe(client, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(client, obj, level, string):
    print(string)


def verify_dev(message_decoded):

    print(message_decoded[0:3])
    if message_decoded[0:3]=='DEV':
        return True
    else:
        return False



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

#sub_topic = 'devices/+/temperature'
#pub_topic = 'devices/1/temperature'

main_sub_topic = 'DEV-1/T'
main_pub_topic = 'DEV-1/T'

sub_topics =[(main_sub_topic,0)]



# Connect
mqttc.username_pw_set(url.username, url.password)
mqttc.connect(url.hostname, url.port)

# Start subscribe, with QoS level 0
mqttc.subscribe(sub_topics)

# Publish a message
#mqttc.publish(pub_topic, " hello from The good-Old-Python")

# Continue the network loop, exit when an error occurs
while True:

    #print('brakked')

    rc = 0
    while rc == 0:
          rc = mqttc.loop()#-> it's like the subsciption to the channel [mosquitto_sub on server]
                           #   constanly listen to the channel

          mqttc_message = get_message()
          mqttc_message_decoded = mqttc_message.decode("utf-8")

          if len(mqttc_message_decoded)>0:
             print(mqttc_message_decoded)

          if (verify_dev(mqttc_message_decoded)):
              print('add-device')
              sub_topics.append((mqttc_message_decoded,0))
              mqttc.subscribe(sub_topics)

              break





'''
mqttc.subscribe(dev_sub_topic[0])

rc = 0
while rc == 0:
      rc = mqttc.loop()

'''







print("rc: " + str(rc))
