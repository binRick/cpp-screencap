import os, sys, json
from flask import Flask, request, jsonify


app = Flask(__name__)

@app.route('/data', methods=['POST']) 
def foo():
    data = request.json
    j = jsonify(data)
    print(j)
    return j
