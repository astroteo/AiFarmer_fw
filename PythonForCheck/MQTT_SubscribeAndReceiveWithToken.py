import secrets

import paho.mqtt.client as mqtt
import os
from urllib.parse import urlparse
import time


Payload = b''
Topic =b''
# Define event callbacks
def on_connect(client, userdata, flags, rc):
    print("rc: " + str(rc))

def on_message(client, obj, msg):
    global Payload
    global Topic
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    Payload = str(msg.payload)[2:len(msg.payload)+2]
    Topic = msg.topic

def on_publish(client, obj, mid):

    pp=0

    #print("mid: " + str(mid))

def on_subscribe(client, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(client, obj, level, string):
    print(string)

def verify_device(pay,sub_topic_list):

    last_topic = sub_topic_list[len(sub_topic_list)-1][0]
    #print(last_topic[0:last_topic.index('-')+2])
    last_topic=last_topic[0:last_topic.index('-')+2]

    print("last_topic = "+last_topic)
    print("pay = ")

    if last_topic == pay:
        #print(pay)
        return False

    else:
        print(".....wow register new device")
        return True




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

main_sub_topic = 'NEW-DEV'
main_pub_topic = 'NEW-DEV'

sub_topics = [(main_sub_topic,0)]
# Connect
mqttc.username_pw_set(url.username, url.password)
mqttc.connect(url.hostname, url.port)

# Start subscribe, with QoS level 0
mqttc.subscribe(sub_topics, 0)

# Publish a message
#mqttc.publish(pub_topic, "ajsdgahgsDagsdjhagsdH")

len_subtopics_before = len(sub_topics)
payload_before = Payload.decode('utf-8')
payload_dev_before = payload_before

count = 0

token_dev_list = [ ]


# Continue the network loop, exit when an error occurs
while True:

    print('MAIN-WHILE')
    print(len(sub_topics))



    if len(sub_topics) != len_subtopics_before:

        len_subtopics_before = len(sub_topics)
        file_subtopic = open("sub_topic_list.txt","a")


        for sub_topic in sub_topics:
            file_subtopic.write(sub_topic[0]+'\n')

        file_subtopic.close()



    print('subscribing to updated list...')
    mqttc.subscribe(sub_topics, 0)
    count = 0
    rc = 0
    while rc == 0:
        rc = mqttc.loop()


        Topic = str(Topic)
        Payload=str(Payload)

        # add here the actuation problem!! It's a "publish" problem only.


 # ******************************************************************************** #
        if Topic == 'NEW-DEV':
            print('here--> Registering new device')
            print(Payload)

        #if(Topic.endswith('T') and Topic != 'NEW-DEV'):


            Payload_dev = Payload[0:Payload.index('/')]
            print(Payload_dev)
            # send dev to Django
            Payload_pin = Payload[Payload.index('/')+1:len(Payload)]
            print(Payload_pin)
            #send pin to Django





            #if verify_device(Payload,sub_topics):
            if Payload != payload_before and len(Payload)>3:
                ### to be substituted with control on all the previously registered device
                #  ( stored in sub_topics
                #   saved in sub_topic_list.txt)###

                payload_before = Payload
                print('here--> HERE')

                token_dev = secrets.token_hex(32)
                print(token_dev)

                token_dev_list.append((Payload,token_dev))
                #send token to Django


                mqttc.subscribe((Payload_dev+'/REG',0))

                sub_topics.append((token_dev + '/T', 0))
                sub_topics.append((token_dev + '/H', 0))
                sub_topics.append((token_dev + '/ACT', 0))

                mqttc.loop_stop(True)
                print('Loop stopped')
                rc = 1

 # ******************************************************************************** #
        flag_pub = True

        if Topic.endswith('/REG'):



            if  Payload =='OK':
                    #print(Topic)
                    #print(Payload)
                    print('finishing registering to device')
                    mqttc.unsubscribe(Topic)

                    mqttc.loop_stop(True)
                    #print('Loop stopped')
                    flag_pub = False
                    rc =1
                    Topic = ""

            if Payload == '?':

                    dev_name = Topic[0:Topic.index('/')]
                    #print(dev_name)

                    for dev_tok_guess in token_dev_list:

                        #print(dev_tok_guess[0][0:dev_tok_guess[0].index('/')])
                        dev_name_guess = dev_tok_guess[0][0:dev_tok_guess[0].index('/')]
                        #print(dev_tok_guess[1])

                        if dev_name_guess == dev_name:
                            #print('GOT_DEVVY')
                            token_magic = dev_tok_guess[1]
                            #print(token_magic)
                            #print(Topic)

                            if flag_pub:
                                #print('publishin??')
                                mqttc.publish(Topic,token_magic)
                                flag_pub = False


print("rc: " + str(rc))