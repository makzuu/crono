from time import sleep
from os import system

h = 0
while 1:
    
    for m in range(59):
        for s in range(59):
            system('clear')
            h = '0'+str(h) if len(str(h)) == 1 else h
            m = '0'+str(m) if len(str(m)) == 1 else m
            s = '0'+str(s) if len(str(s)) == 1 else s
            print(f'{h}:{m}:{s}')
            sleep(1)
