import paho.mqtt.client as mqtt
import os
from urllib.parse import urlparse

import paho.mqtt.client as mqtt
import os
from urllib.parse import urlparse

import secrets


###################### DATABASE VARIABLES ##################################

reg_dev_list = [ ]
start_act_list = [ ]# DB response for devices that need to be fired ON
stop_act_list = [ ]# DB response for devices that need to be fired OFF

############################################################################
file='dev_list.txt'
# Define event callbacks
def on_connect(client, userdata, flags, rc):
    print("rc: " + str(rc))

def on_message(client, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

    # here we have to perform th send to django option: ..endswith(T),..endswith(H),.....endswith(ACT)


def existence_callback(client, userdata, message):
    #print("Received message '" + str(message.payload) + "' on topic '"
    #    + message.topic + "' with QoS " + str(message.qos))

    print(message.topic)
    print(message.topic[0:3])
    print(message.payload)
    print(message.payload.decode())

    if message.topic == 'NEW-DEV':

        Payload = message.payload.decode();

        dev_name =Payload [0:Payload.index('/')]
        print('device-name:'+ dev_name)
        dev_pin = Payload [Payload.index('/')+1: len(Payload)]
        print('device-pin:' + dev_pin)
        client.subscribe(dev_name+'/REG',0) # to be splitted in dev-name and pin
        #print(str(message.payload))
        client.message_callback_add(dev_name+'/REG',existence_callback)

        dict_reg = {'name':dev_name,'pin':dev_pin,'token':secrets.token_hex(32)}

        reg_dev_list.append(dict_reg)

        f=open(file,'w')
        for dict_reg in reg_dev_list:
            f.write(str(dict_reg['name']))
            f.write(str(dict_reg['pin']))
            f.write(dict_reg['token'])
        f.close()


    if str(message.topic)[0:3]=='DEV' and str(message.topic)[str(message.topic).index('/')+1:len(str(message.topic))+1] =='REG':
        print('HERe')

        dev_name_guess =  str(message.topic)[0 : str(message.topic).index('/')]

        if message.payload.decode() == '?':
            print('...checkings for device')
            for dict_now in reg_dev_list:

                if dict_now['name'] == dev_name_guess:
                    print('...sending token')
                    client.publish(str(message.topic),dict_now['token'])
                    client.subscribe(dict_now['token'] + '/T')
                    client.subscribe(dict_now['token'] + '/H')
                    client.subscribe(dict_now['token'] + '/ACT')


        if message.payload.decode() == 'OK':
            #for dict_now in reg_dev_list:
                #if dict_now['name'] == dev_name_guess:
            print('...end registration proceadure')
            client.message_callback_remove(str(message.topic))
            client.unsubscribe(str(message.topic))



#def actuation_callback:


def on_publish(client, obj, mid):
    #print("mid: " + str(mid))

    for start_act in start_act_list:
        client.publish(start_act+'/ACT',1)

    for stop_act in stop_act_list:
        client.publish(stop_act + '/ACT', 0)



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

# Let's try adding a callback!!!
mqttc.message_callback_add('NEW-DEV',existence_callback)



# Continue the network loop, exit when an error occurs
rc = 0
while rc == 0:
    rc = mqttc.loop()

