import os
import sys
import shutil
import numpy as np
import pdb
from multiprocessing import Pool

GSUFFIX = 'resplit'

primary = 'data/ee_ab_map.dat'
#maps = [m for m in os.listdir('.') if os.path.isfile(m) and m.startswith('ee_ab')]
maps = ['data/ee_ab_map_2.dat', 'data/ee_ab_map_3r.dat']

dirs= { '10_5': [('10', '5', '5'),
                 ('10', '-5', '5'),
                 ('10', '5', '-5'),
                 ('10', '-5', '-5')],
        '10_7': [('10', '7', '-7'),
                 ('10', '7', '7'),
                 ('10', '-7', '7'),
                 ('10', '-7', '-7')] }

def createCommands():
  cwd0 = os.getcwd()
  commands = []
  for d in dirs:
    os.chdir(cwd0)
    if not os.path.exists(d + GSUFFIX):
      os.makedirs(d + GSUFFIX)

    cwd1 = cwd0 + '/' + d + GSUFFIX
    for conf in dirs[d]:
      for m in maps:
        os.chdir(cwd1)
        fsuffix = conf[0] + '_' + conf[1] + '_' + conf[2]
        dirname = 'ee_' + primary[15] + '_' + m[15] + '_' + fsuffix
        if not os.path.exists(dirname):
          os.makedirs(dirname)

        cwd2 = cwd1 + '/' + dirname

        cmd = '../../binary test learn_fdebug ../../' + primary + ' ../../' + m + \
            ' ' + conf[0] + ' ' + conf[1] + ' ' + conf[2]
        commands.append((cmd, cwd2))
  return commands

def execCommand(c):
  os.chdir(c[1])
  print 'cd to ' + c[1]
  print 'running command' + c[0]
  if not (len(sys.argv) > 1 and sys.argv[1].startswith('sim')):
      os.system(c[0])

p = Pool(4)
p.map(execCommand, createCommands())
