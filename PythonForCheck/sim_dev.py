import paho.mqtt.client as mqtt
import os
from urllib.parse import urlparse

global stop_dev

stop_dev=False
def on_connect(client, userdata, flags, rc):
    print("rc: " + str(rc))

def on_message(client, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

    # here we have to perform th send to django option: ..endswith(T),..endswith(H),.....endswith(ACT)

def on_publish(client, obj, mid):
    #print("mid: " + str(mid))
    a=1


def on_subscribe(client, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(client, obj, level, string):
    print(string)

def existence_callback_dev(client, userdata, message):

    if len(message.payload.decode()) > 63:
        print('token received!!')
        print(message.payload.decode())
        client.publish('OK')
        stop_dev=True
        print(stop_dev)



dev_reg='DEV-2/REG'

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


sub_topics= [('NEW-DEV',0),(dev_reg,0)]
#pub_topics = ['DEV-42/ACT','DEV-43/ACT']
#pub_contents = ['0','1']


# Connect
mqttc.username_pw_set(url.username, url.password)
mqttc.connect(url.hostname, url.port)

# Start subscribe, with QoS level 0
mqttc.subscribe(sub_topics)

# Let's try adding a callback!!!
mqttc.message_callback_add('DEV-2/REG',existence_callback_dev)



# Continue the network loop, exit when an error occurs
rc = 0
count=0
while rc == 0:
    rc = mqttc.loop()
    if count %10 ==0:
        mqttc.publish('NEW-DEV','DEV-1/1245')
        mqttc.publish('DEV-2/REG','?')

    count += 1
    if stop_dev:
        rc = 1
