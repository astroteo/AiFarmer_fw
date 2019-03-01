# This file has the same function of server.py ==> generating/reading  the POST/GET
# from  the app to be read and interpreted by ESP32 !
# RUN FROM COMMAND: $ cd /Users/teobaiguera/Google\ Drive/Digital\ Universitas\ 2017\ @\ Studenti/Coding/Project/Matteo
#                    $
from flask import Flask
from flask import request


Hello = False
POST_test= True
GET_test = False

app = Flask(__name__)



if POST_test:
    #BEPPE POST
    @app.route('/'
        ,methods=['POST'])
    def login():
        return request.form['ssid'] + ' ' +request.form['password']


if Hello:
    @app.route('/')
    def hello_world():
        return 'Hello, World!'

if GET_test:
    #BEPPE POST
    @app.route('/'
        ,methods=['GET'])
    def login():
        return request.form['ssid'] + ' ' +request.form['password']

