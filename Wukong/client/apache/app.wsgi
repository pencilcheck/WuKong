#!/usr/bin/python

import os
os.chdir(os.path.dirname(__file__))

import bottle
from bottle import route, static_file, template, request


@route('/static/:path#.+#')
def server_static(path):
  return static_file(path, root='/var/www/people-counter/static')

@route('/')
def hello():
  return template('index_template')

@route('/', method='POST')
def update():
  id = request.POST.get('id')
  if id == '42':
    while not os.path.exists('/home/iagentserver00/people_counter/count.txt'):
      pass
    f = open('/home/iagentserver00/people_counter/count.txt')

    while not os.path.exists('/home/iagentserver00/people_counter/door.txt'):
      pass
    d = open('/home/iagentserver00/people_counter/door.txt')
    return {'door': d.read(), 'counter': f.read()}
  else:
    return {'door': 'Error', 'counter': "Error"}


application = bottle.default_app()
