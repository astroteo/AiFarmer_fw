import paho.mqtt.client as mqtt
from urllib.parse import urlparse

def publish_state(token,state):

    url_str = 'mqtt://astroteo:Gold4Himself@mqtt.aifarmer.du.cdr.mn:8883'
    url = urlparse(url_str)
    print(url.hostname)
    print(url.username)
    print(url.password)
    print(url.port)

    #sub_topic = 'devices/+/temperature'
    #pub_topic = 'devices/1/temperature'

    sub_topic = 'NEW-DEV'
    pub_topic = 'NEW-DEV'


    mqttc = mqtt.Client()

    # Connect
    mqttc.username_pw_set(url.username, url.password)
    mqttc.connect(url.hostname, url.port)

    # Start subscribe, with QoS level 0
    mqttc.publish(token+'/ACT', str(int(state)))

