import paho.mqtt.client as mqtt
import os
from urllib.parse import urlparse

Message = b''
Topic =b''
# Define event callbacks
def on_connect(client, userdata, flags, rc):
    print("rc: " + str(rc))

def on_message(client, obj, msg):
    global Message
    global Topic
    Message = " "
    Topic = " "
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    Message = msg.payload
    Topic= msg.topic



def get_message():
    return Message.decode('utf-8')

def get_topic():
    return Topic.decode('utf-8')




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

main_sub_topic = 'NEW-DEV'
main_pub_topic = 'NEW-DEV'

sub_topics =[(main_sub_topic,0)]
mqttc.subscribe(sub_topics)
#mqttc.subscribe(main_sub_topic,0);

new_dev = False




while True:



    if(new_dev):
        mqttc.subscribe(sub_topics)
        print(" New Dev found")
        new_dev = False



    rc =0
    while rc==0:
        mqttc.loop()

        #mqttc.publish(main_pub_topic,"OK")
        #print(get_topic())


        if (get_topic() == "NEW-DEV"):

            mqttc.unsubscribe(sub_topics)
            sub_topics.append(get_message(),0)
            new_dev = True

            break








