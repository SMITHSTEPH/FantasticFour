import binascii
import socket
import threading
import sys
import os
import curses
import socks
import stem.process
from smp import SMP
from stem.control import Controller
from stem.util import term
from curses.textpad import Textbox
from random import randint
from contextlib import contextmanager
from pyaxo import Axolotl
from time import sleep

#global tor info
TOR_SERVER_PORT             = 9054
TOR_SERVER_CONTROL_PORT     = 9055
TOR_CLIENT_PORT             = 9154
TOR_CLIENT_CONTROL_PORT     = 9155
TOR_CONTROL_PASSWORD        = 'axotor'
TOR_CONTROL_HASHED_PASSWORD = \
    '16:0DF8A51D5BB7A97160265FEDD732D47AB07FC143446943D92C2C584673'

def print_bootstrap_lines(line):
    if 'Bootstrapped ' in line:
        print(term.format(line, term.Color.RED))    

def getPasswd(nick):
    return '1'    

def axo(my_name, other_name, dbname, dbpassphrase):
    global a
    a = Axolotl(my_name, dbname=dbname, dbpassphrase=dbpassphrase,
                nonthreaded_sql=False)
    a.loadState(my_name, other_name)

def arg_info():
    print 'Usage: ' + sys.argv[0] + ' --(server,client, auth)'
    print ' --server: start a chat in server mode'
    print ' --client: start a chat in client mode'
    print (' --auth: triple diffie hellman authentication:\n\t create a database'+
    	  ' for you and the other user you are trying to chat with!')	
    exit()

if __name__ == '__main__':
    try:
        mode = sys.argv[1]
    except:
        arg_info()

    if (mode == '--server' or mode == '--client' or mode == '--auth'):
        
        USER1 = raw_input('Enter your username: ')
        USER2 = raw_input('Enter the username of the other party: ')
        HOST = '127.0.0.1'
        PORT=1025

        if mode == '--server':
           axo(USER1, USER2, dbname=USER2+'.db',
           dbpassphrase=getPasswd(USER1))
           tor_process = tor(TOR_SERVER_PORT, TOR_SERVER_CONTROL_PORT, 'tor.server')
           hs = hiddenService()
           print 'Waiting for ' + USER2 + ' to connect...'

        elif mode == '--client':
    	   print "client"

        #mode == --auth
        else:
            newaxo = Axolotl(USER1, dbname=USER2+'.db')
            newaxo.printKeys()
            ans = raw_input('Do you want to create a new Axolotl database? Y/N ')

            if ans.lower() == 'y':
                identity = raw_input('What is the identity key for the other party? ')
                ratchet = raw_input('What is the ratchet key for the other party? ')
                handshake = raw_input('What is the handshake key for the other party? ')

                newaxo.initState(OTHER_NICK, binascii.a2b_base64(identity),
                              binascii.a2b_base64(handshake),
                         binascii.a2b_base64(ratchet))
                newaxo.saveState()

                print 'The database for ' + USER1 + ' -> ' + USER2 + ' has been saved.'
            else:
                print 'OK, nothing has been saved...' 

    else:
        arg_info()	


