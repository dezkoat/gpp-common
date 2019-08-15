#!/usr/bin/python3

import os
import sys
import re
import json

class Module:

  build_state = {}
  build_config = {}

  def start(self, args):
    if len(args) < 2:
      print('No command specified. Exiting...')
      return

    command, params = args[1], args[2:]

    if command == 'build':
      # Only Build
      self.build(params[0])
  
  def build(self, path):
    build_path = os.path.join(path, 'build')
    if not os.path.exists(build_path):
      os.makedirs(build_path)

    self.build_state, self.build_config = self.read_config(path)

    try:
      print('Compiling...')
      objects = self.compile(path)
      print('Linking...')
      self.link(objects, path)
    except Exception as e:
      print('Failed to build %s: %s' % (path, str(e)))

  def compile(self, path):
    objects = []
    for root, _, files in os.walk(os.path.join(path, 'src')):
      for f in files:
        if not f.endswith('.cpp'):
          continue

        build_path = os.path.join(path, 'build')
        src_path = os.path.join(path, 'src')
        path_in = os.path.join(root, f)
        path_out = re.sub(src_path, build_path, re.sub(r'\.cpp$', '.o', path_in))

        self.compile_file(path, path_in, path_out)
        objects.append(path_out)

    return objects

  def check_file_change(self, path_in, path_out):
    time = os.stat(path_in).st_mtime
    if (path_in in self.build_state['timestamp'] and
        os.path.isfile(path_out) and
        time == self.build_state['timestamp'][path_in]):
      return False

    return True

  def check_header_change(self, path_in):
    with open(path_in) as f:
      for line in f:
        match = re.match('#include <([a-zA-Z/.]+)>', line)
        if match:
          f = match.groups()[0]
          if os.path.isfile(f) and self.check_file_change(f, f):
            return True

    return False

  def compile_file(self, path, path_in, path_out):
    if (not self.check_file_change(path_in, path_out) and
        not self.check_header_change(path_in)):
      return

    path_out_dir = os.path.dirname(path_out)
    if not os.path.exists(path_out_dir):
      os.makedirs(path_out_dir)

    defines = ''
    for define in self.build_config['defines']:
      defines += ' -D' + define

    print(path_in, '>', path_out)
    cflags = self.build_config['compile_flags']
    cmd = 'g++ -g -rdynamic -c %s -o %s -I.. %s %s' % (path_in, path_out, cflags, defines)
    if os.system(cmd) != 0:
      raise Exception('Build failed')

    self.build_state['timestamp'][path_in] = os.stat(path_in).st_mtime

    # Update headers
    with open(path_in) as f:
      for line in f:
        match = re.match('#include <([a-zA-Z/.]+)>', line)
        if match:
          f = match.groups()[0]
          if os.path.isfile(f):
            self.build_state['timestamp'][f] = os.stat(f).st_mtime

    self.write_state(path)

  def link(self, objects, path):
    bin_path = os.path.join(path, 'bin')
    if not os.path.isdir(bin_path):
      os.makedirs(bin_path)

    bin_in = ' '.join(objects)
    bin_out = os.path.join(bin_path, self.build_config['name'])
    if self.build_config['is_lib']:
      bin_out += '.a'
      cmd = 'ar rvs %s %s' % (bin_out, bin_in)
      if os.system(cmd) != 0:
        return False
    else:
      lflags = self.build_config['link_flags']
      cmd = 'g++ -g -rdynamic -o %s %s %s' % (bin_out, bin_in, lflags)
      if os.system(cmd) != 0:
        return False

    print(bin_out)

  def read_config(self, path):
    state = {}
    config = {}

    build_path = os.path.join(path, 'build')

    state_file = os.path.join(build_path, 'build.json')
    if os.path.isfile(state_file):
      with open(state_file) as f:
        state = json.load(f)

    if 'timestamp' not in state:
      state['timestamp'] = {}
    if 'headers' not in state:
      state['headers'] = {}

    config_file = os.path.join(path, 'config.json')
    if os.path.isfile(config_file):
      with open(config_file) as f:
        config = json.load(f)

    # Set default value
    if 'name' not in config:
      config['name'] = 'app'
    if 'compile_flags' not in config:
      config['compile_flags'] = ''
    if 'link_flags' not in config:
      config['link_flags'] = ''
    if 'defines' not in config:
      config['defines'] = []
    if 'is_lib' not in config:
      config['is_lib'] = False

    return state, config

  def write_state(self, path):
    state_file = os.path.join(os.path.join(path, 'build'), 'build.json')
    with open(state_file, 'w') as f:
      json.dump(self.build_state, f)

if __name__ == "__main__":
  Module().start(sys.argv)
