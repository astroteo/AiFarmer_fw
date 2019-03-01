from flask import Flask
from flask import request

app = Flask(__name__)

@app.route('/setup', methods=['POST'])
def login():
    return request.form['ssid'] + ' ' +  request.form['password']
