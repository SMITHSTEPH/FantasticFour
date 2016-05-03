#!/usr/bin/python
import sys
import binascii
import socket
import Tkinter  as tk
import ttk as ttk
from pyaxo import Axolotl
import subprocess
import binascii
import hashlib
import stem.process
from stem.control import Controller
from stem.util import term
from contextlib import contextmanager
import os
import socks
from smp import SMP



TOR_SERVER_PORT             = 9054
TOR_SERVER_CONTROL_PORT     = 9055
TOR_CLIENT_PORT             = 9154
TOR_CLIENT_CONTROL_PORT     = 9155
TOR_CONTROL_PASSWORD        = 'axotor'
TOR_CONTROL_HASHED_PASSWORD = \
    '16:0DF8A51D5BB7A97160265FEDD732D47AB07FC143446943D92C2C584673'

@contextmanager
def torcontext():
    try:
        s = socks.socksocket()
        s.set_proxy(socks.SOCKS5, '127.0.0.1', TOR_CLIENT_PORT)
        yield s
        s.close()
    except socks.SOCKS5Error:
        print ''
        print 'You need to wait long enough for the Hidden Service'
        print 'at the server to be established. Try again in a'
        print 'minute or two.'

@contextmanager
def socketcontext(*args, **kwargs):
    s = socket.socket(*args, **kwargs)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    yield s
    s.close()

def print_bootstrap_lines(line):
    if 'Bootstrapped ' in line:
        chat_window.insert(tk.END, term.format(line, term.Color.RED))        

def axo(my_name, other_name, dbname, dbpassphrase):
    global a
    a = Axolotl(my_name, dbname=dbname, dbpassphrase=dbpassphrase, nonthreaded_sql=False)
    a.loadState(my_name, other_name)    

def tor(port, controlport, tor_dir):
    tor_process = stem.process.launch_tor_with_config(
        tor_cmd = '/Applications/TorBrowser.app/TorBrowser/Tor/tor',
        config = {
                  'ControlPort': str(controlport),
                  'SocksPort'  : str(port),
                  'Log'        : [ 'NOTICE stdout', 'ERR file /tmp/tor_error_log', ],
                  'DataDirectory' : tor_dir,
                  'HashedControlPassword' : TOR_CONTROL_HASHED_PASSWORD,
                 },
        completion_percent = 100,
        take_ownership = True,
        timeout = 90,
        init_msg_handler = print_bootstrap_lines,
        )
    return tor_process

def smptest(secret, sock, is_server):
    # Create an SMP object with the calculated secret
    smp = SMP(secret)

    if is_server:
        # Do the SMP protocol
        buffer = a.decrypt(sock.recv(2311, socket.MSG_WAITALL))
        buffer = smp.step2(buffer)
        buffer = a.encrypt(buffer)
        buffer = buffer+(4412-len(buffer))*b'\x00' # pad to fixed length
        sock.send(buffer)

        buffer = a.decrypt(sock.recv(3345, socket.MSG_WAITALL))
        buffer = smp.step4(buffer)
        buffer = a.encrypt(buffer)
        buffer = buffer+(1243-len(buffer))*b'\x00' # pad to fixed length
        sock.send(buffer)

    else:
        # Do the SMP protocol
        buffer = smp.step1()
        buffer = a.encrypt(buffer)
        buffer = buffer+(2311-len(buffer))*b'\x00' # pad to fixed length
        sock.send(buffer)

        buffer = a.decrypt(sock.recv(4412, socket.MSG_WAITALL))
        buffer = smp.step3(buffer)
        buffer = a.encrypt(buffer)
        buffer = buffer+(3345-len(buffer))*b'\x00' # pad to fixed length
        sock.send(buffer)

        buffer = a.decrypt(sock.recv(1243, socket.MSG_WAITALL))
        smp.step5(buffer)

    # Check if the secrets match
    if smp.match:
        chat_window.insert(tk.END, 'Secrets Match!')
        smp_match = True
    else:
        chat_window.insert(tk.END, 'Secrets DO NOT Match!')
        smp_match = False
    return smp_match    

def hiddenService():
    PORT = 50000
    HOST = '127.0.0.1'
    hidden_svc_dir = '/Applications/TorBrowser.app/TorBrowser/Tor/tor.hs/'

    chat_window.insert(tk.END, ' * Getting controller')
    controller = Controller.from_port(address='127.0.0.1', port=TOR_SERVER_CONTROL_PORT)
    try:
        controller.authenticate(password=TOR_CONTROL_PASSWORD),
        controller.set_options([
            ('HiddenServiceDir', hidden_svc_dir),
            ('HiddenServicePort', '50000 %s:%s' % (HOST, str(PORT))),
            ])
        svc_name = open(hidden_svc_dir + 'hostname', 'r').read().strip()
        chat_window.insert(tk.END, ' * Created onion server: %s' % svc_name)
        print ' * Created onion server: %s' % svc_name
    except Exception as e:
        print e
    return controller 

def toClient():
    global connect_role 
    connect_role = "client"
    print connect_role
    onion_input.config(state=tk.NORMAL)

def toSever():
	global connect_role
	connect_role = "server"
	print connect_role
	onion_input.config(state=tk.DISABLED)

def connect(window, connect_role, nick_textbox, other_nick_textbox, onion_textbox):
	print "in connect"
	global connection_status
	connection_status="connected"
	NICK = nick_textbox.get()
	OTHER_NICK = other_nick_textbox.get()
	HOST = '127.0.0.1'
	PORT=50000
	window.destroy() 
	chat_window.config(state=tk.NORMAL)

	if connect_role == "server":
		axo(NICK, OTHER_NICK, dbname=OTHER_NICK+'.db', dbpassphrase="password")
		tor_process = tor(TOR_SERVER_PORT, TOR_SERVER_CONTROL_PORT, 'tor.server')
		hs = hiddenService()
		print 'Waiting for ' + OTHER_NICK + ' to connect...'
		chat_window.insert(tk.END, 'Waiting for ' + OTHER_NICK + ' to connect...')
		with socketcontext(socket.AF_INET, socket.SOCK_STREAM) as s:
			s.bind((HOST, PORT))
			s.listen(1)
			conn, addr = s.accept()
			chat_window.insert(tk.END, 'Connected...')
			print "Connected"
			chat_window.insert(tk.END, 'Performing per-session SMP authentication...')
			ans = chat_window.insert(tk.END, 'Enter SMP secret: ')
			chat_window.insert(tk.END, 'Running SMP protocol...')
			secret = a.state['DHIs'] + ans + a.state['DHIr'] + a.state['CONVid']
			smp_match = smptest(secret, conn, True)
			if not smp_match:
				a.saveState()
				sys.exit()

	elif connect_role == "client":
		axo(NICK, OTHER_NICK, dbname=OTHER_NICK+'.db', dbpassphrase="password")
		tor_process = tor(TOR_CLIENT_PORT, TOR_CLIENT_CONTROL_PORT, 'tor.client')
		HOST = onion_textbox.get()
		print 'Connecting to ' + HOST + '...'
		with torcontext() as s:
			s.connect((HOST, PORT))
			print 'Connected...'
			print 'Performing per-session SMP authentication...'
			ans = raw_input('Enter SMP secret: ')
			print 'Running SMP protocol...'
			secret = a.state['DHIr'] + ans + a.state['DHIs'] + a.state['CONVid']
			smp_match = smptest(secret, s, False)
			if not smp_match:
				print 'Exiting...'
				a.saveState()
				sys.exit()
            #chatThread(s, smp_match)		


def disconnect():
	print "disconnect"
	global connection_status
	connection_status="disconnected"


def processUserText():
   print "process text"
    #data = text_input.get()
    #if data[0] != "/":  # is not a command
    #    placeText(data)
    #else:
    #    if data.find(" ") == -1:
    #        command = data[1:]
    #    else:
    #        command = data[1:data.find(" ")]
    #    params = data[data.find(" ") + 1:].split(" ")
    #    processUserCommands(command, params)
    #text_input.delete(0, END)

def connect_window(master):
	root = tk.Toplevel(master)
	root.title("Connect")
	root.grab_set()

	nick_frame=tk.Frame(root, width=20) #chat window frame
	nick_frame.pack(expand=tk.YES, fill=tk.BOTH)
	nick_label=tk.Label(nick_frame, text='           My Nick:')
	nick_label.pack(anchor=tk.N, side=tk.LEFT)
	nick_input=tk.Entry(nick_frame, width=20)
	nick_input.pack(anchor=tk.N, side=tk.RIGHT, expand=tk.YES, fill=tk.X)
	
	other_nick_frame=tk.Frame(root, width=20) #chat window frame
	other_nick_frame.pack(expand=tk.YES, fill=tk.BOTH)
	other_nick_label=tk.Label(other_nick_frame, text="Contact's Nick:")
	other_nick_label.pack(anchor=tk.N, side=tk.LEFT)
	other_nick_input=tk.Entry(other_nick_frame, width=20)
	other_nick_input.pack(anchor=tk.N, side=tk.RIGHT, expand=tk.YES, fill=tk.X)

	user_type_frame=tk.Frame(root, width=20) 
	user_type_frame.pack(expand=tk.YES, fill=tk.BOTH)
	
	client_rb=tk.Radiobutton(user_type_frame, text="client", variable=connect_role, value="client", command=toClient)
	client_rb.pack(anchor=tk.W, expand=tk.YES, fill=tk.Y)
	server_rb=tk.Radiobutton(user_type_frame, text="server", variable=connect_role, value="server", command=toSever)
	server_rb.pack(anchor=tk.W, expand=tk.YES, fill=tk.Y)

	onion_frame=tk.Frame(root, width=20) #chat window frame
	onion_frame.pack(expand=tk.YES, fill=tk.BOTH)
	onion_label=tk.Label(onion_frame, text="If Client Enter Onion Server:")
	onion_label.pack(anchor=tk.N, side=tk.LEFT)
	global onion_input
	onion_input=tk.Entry(onion_frame, width=20)
	onion_input.pack(anchor=tk.N, side=tk.RIGHT, expand=tk.YES, fill=tk.X)
	onion_input.config(state=tk.DISABLED)

	connect_button = tk.Button(root, text="Connect", command=lambda: connect(root, connect_role, nick_input, other_nick_input, onion_input))
	connect_button.pack(expand=tk.YES, fill=tk.BOTH)


def add_contact_window(master):
	press_count=0
	root = tk.Toplevel(master)
	root.title("Add Contact")
	root.grab_set()
	nick_frame=tk.Frame(root, width=20) #chat window frame
	nick_frame.pack(expand=tk.YES, fill=tk.BOTH)
	nick_label=tk.Label(nick_frame, text='           My Nick:')
	nick_label.pack(anchor=tk.N, side=tk.LEFT)
	nick_input=tk.Entry(nick_frame, width=20)
	nick_input.pack(anchor=tk.N, side=tk.RIGHT, expand=tk.YES, fill=tk.X)

	other_nick_frame=tk.Frame(root, width=20) #chat window frame
	other_nick_frame.pack(expand=tk.YES, fill=tk.BOTH)
	other_nick_label=tk.Label(other_nick_frame, text="Contact's Nick:")
	other_nick_label.pack(anchor=tk.N, side=tk.LEFT)
	other_nick_input=tk.Entry(other_nick_frame, width=20)
	other_nick_input.pack(anchor=tk.N, side=tk.RIGHT, expand=tk.YES, fill=tk.X)

	password_frame=tk.Frame(root, width=20) #chat window frame
	password_frame.pack(expand=tk.YES, fill=tk.BOTH)
	password_label=tk.Label(password_frame, text='  My Password:')
	password_label.pack(anchor=tk.N, side=tk.LEFT)
	password_input=tk.Entry(password_frame, width=20, show="*")
	password_input.pack(anchor=tk.N, side=tk.RIGHT, expand=tk.YES, fill=tk.X)

	keys_button=tk.Button(root, text="Generate My Keys", command=lambda: generate_keys(press_count, nick_keys_input, nick_input, other_nick_input, password_input))
	keys_button.pack(expand=tk.YES, fill=tk.BOTH)

	nick_keys_frame=tk.Frame(root, width=70)
	nick_keys_frame.pack(expand=tk.YES, fill=tk.BOTH)
	nick_keys_label=tk.Label(nick_keys_frame, text='My Keys: ')
	nick_keys_label.pack(anchor=tk.W, side=tk.TOP)
	nick_keys_input = tk.Text(nick_keys_frame, width=70, height=10)
	nick_keys_input.pack(side=tk.LEFT, expand=tk.YES, fill=tk.BOTH)
	nick_keys_input.config(highlightbackground="grey")
	nick_keys_input.config(state=tk.DISABLED)

	other_nick_keys_frame=tk.Frame(root, width=40)
	other_nick_keys_frame.pack(expand=tk.YES, fill=tk.BOTH)
	other_nick_keys_label=tk.Label(other_nick_keys_frame, text="Contact's Keys: ")
	other_nick_keys_label.pack(anchor=tk.W, side=tk.TOP)

	identity_key_label=tk.Label(other_nick_keys_frame, text='      Identity Key: ')
	identity_key_label.pack(anchor=tk.N, side=tk.LEFT)
	identity_key=tk.Entry(other_nick_keys_frame, width=20)
	identity_key.pack(anchor=tk.N, side=tk.RIGHT, expand=tk.YES, fill=tk.X)

	rachet_key_frame=tk.Frame(root, width=40)
	rachet_key_frame.pack(expand=tk.YES, fill=tk.BOTH)
	rachet_key_label=tk.Label(rachet_key_frame, text='       Rachet Key: ')
	rachet_key_label.pack(anchor=tk.N, side=tk.LEFT)
	rachet_key=tk.Entry(rachet_key_frame, width=20)
	rachet_key.pack(anchor=tk.N, side=tk.RIGHT, expand=tk.YES, fill=tk.X)

	handshake_key_frame=tk.Frame(root, width=40)
	handshake_key_frame.pack(expand=tk.YES, fill=tk.BOTH)

	handshake_key_label=tk.Label(handshake_key_frame, text='Handshake Key: ')
	handshake_key_label.pack(anchor=tk.N, side=tk.LEFT)
	handshake_key=tk.Entry(handshake_key_frame, width=20)
	handshake_key.pack(anchor=tk.N, side=tk.RIGHT, expand=tk.YES, fill=tk.X)

	# fingerprint_button=tk.Button(root, text="Verify fingerprint", command=lambda: verify_fingerprint(identity_key, fingerprint_input))
	# fingerprint_button.pack(expand=tk.YES, fill=tk.BOTH)
	# fingerprint_frame=tk.Frame(root, width=40)
	# fingerprint_frame.pack(expand=tk.YES, fill=tk.BOTH)
	# fingerprint_label=tk.Label(fingerprint_frame, text="Contact's fingerprint: ")
	# fingerprint_label.pack(anchor=tk.W, side=tk.TOP)
	# fingerprint_input = tk.Text(fingerprint_frame, width=40, height=1)
	# fingerprint_input.pack(side=tk.LEFT, expand=tk.YES, fill=tk.BOTH)
	# fingerprint_input.config(highlightbackground="grey")
	# fingerprint_input.config(state=tk.DISABLED)
	# warning_label=tk.Label(root, text="If fingerprint is valid add contact\notherwise try again")
	# warning_label.pack(expand=tk.YES, fill=tk.BOTH)

	keys_button=tk.Button(root, text="Add Contact", command=lambda: add_contact(root, rachet_key, handshake_key, identity_key, other_nick_input))
	keys_button.pack(expand=tk.YES, fill=tk.BOTH)

# def verify_fingerprint(identity_key_texbox, fingerprint_textarea):

# 	fingerprint_textarea.delete(0, 'end') #delete all of the text from the widget
# 	other_identityKey=identity_key_texbox.get()
# 	print "identity key:"
# 	print other_identityKey

# 	if(not len(other_identityKey)):
# 		fingerprint_textarea.config(state=tk.NORMAL)
# 		fingerprint_textarea.insert(tk.END, "all keys need to be specified!")
# 		fingerprint_textarea.config(fg="red", state=tk.DISABLED)

# 	else:
# 		fingerprint = hashlib.sha224(other_identityKey).hexdigest().upper()
# 		fprint = ''
# 		for i in range(0, len(fingerprint), 4):
# 			fprint += fingerprint[i:i+2] + ':'

# 		print fprint[:-1] + '\n'  
# 		fingerprint_textarea.config(state=tk.NORMAL)
# 		fingerprint_textarea.insert(tk.END, fprint[:-1] + '\n')
# 		fingerprint_textarea.config(fg="black", state=tk.DISABLED)

def add_contact(window, ratchet_textbox, handshake_textbox, identity_textbox, other_nick_textbox):
	OTHER_NICK = other_nick_textbox.get()
	identity = identity_textbox.get()
	handshake = handshake_textbox.get()
	ratchet = ratchet_textbox.get()

	newaxo.initState(OTHER_NICK, binascii.a2b_base64(identity), binascii.a2b_base64(handshake),binascii.a2b_base64(ratchet), verify=False)
	newaxo.saveState()
	window.destroy() 

def generate_keys(press_count, text_area, nick_textbox, other_nick_textbox, password_textbox):
	NICK = nick_textbox.get()
	OTHER_NICK = other_nick_textbox.get()
	password = password_textbox.get()

	print "nick: " + NICK
	print "other nick: " + OTHER_NICK
	print "password: " + password

	if press_count==0:
		text_area.delete(0, 'end') #delete all of the text from the widget

	if((not len(NICK)) or (not len(OTHER_NICK)) or (not len(password))):
		text_area.config(state=tk.NORMAL)
		text_area.insert(tk.END, "make sure nick, other nick and password entries are filled out!\n\n")
		text_area.config(fg="red",state=tk.DISABLED)

	else: 
		global newaxo
		newaxo = Axolotl(NICK, dbname=OTHER_NICK+'.db', dbpassphrase=password)
		newaxo.printKeys()
		identity_key = "identity key: " + binascii.b2a_base64(newaxo.state['DHIs'])
		fingerprint = hashlib.sha224(newaxo.state['DHIs']).hexdigest().upper() #getting fingerprint
		fprint = ''
		for i in range(0, len(fingerprint), 4):
		    fprint += fingerprint[i:i+2] + ':'

		fingerprint = "fingerprint:" + fprint[:-1] + "\n"
		rachet_key="rachet_key: " + binascii.b2a_base64(newaxo.state['DHRs'])
		handshake_key = "handshake key: " + binascii.b2a_base64(newaxo.handshakePKey)
		text_area.config(state=tk.NORMAL)
		text_area.insert(tk.END, identity_key+fingerprint+rachet_key+handshake_key)
		text_area.config(fg="black",state=tk.DISABLED)



def main():
	#----------------------------------------------------------------------#
	# Create main window and two frames to go inside it                    #
	# The top frame holds the textarea, the bottom frame holds the         # 
	# textbox as well as the connect and add contact button                #
	#----------------------------------------------------------------------#
	top_frame_rgb = "#%02x%02x%02x" % (64, 64, 64)
	root = tk.Tk()
	root.title("Chat")	#root window
	top_frame=tk.Frame(root, width=40, height=25, bg=top_frame_rgb) #chat window frame
	top_frame.pack(side=tk.TOP, expand=tk.YES, fill=tk.BOTH)
	top_frame.config(highlightbackground="black")
	bottom_frame=tk.Frame(root, width=40, height=5, bg="black") #enter text frame frame
	bottom_frame.pack(side=tk.BOTTOM, expand=tk.YES, fill=tk.BOTH)
	bottom_frame.config(highlightbackground="black")
	#-----------------------------------------------------------------------#
	# Chat window will take up the entire top frame                         #
	# The chat window has a scroll that allows the user to view past text   #
	# The chat window is disabled so the user can't write in it             #
	#-----------------------------------------------------------------------#
	global chat_window
	chat_window = tk.Text(top_frame, bg=top_frame_rgb, fg="white") #main textbox
	chat_window.pack(side=tk.LEFT, expand=tk.YES, fill=tk.BOTH) #fills left side to make room for the scrollbar
	chat_window.focus_set()
	chat_window.config(highlightbackground="black")
	top_frame_scroll = tk.Scrollbar(top_frame) #create scrollbar
	top_frame_scroll.pack(side=tk.RIGHT, fill=tk.BOTH)
	top_frame_scroll.config(command=chat_window.yview)
	top_frame_scroll.config(highlightbackground="black")
	chat_window.config(yscrollcommand=top_frame_scroll.set)
	#welcome message
	chat_window.insert(tk.END," Welcome to secure chat\n To authenticate contacts: click 'Add a Contact'\n To talk with a contact click 'Connect'\n Contacts MUST be authenticated before you can chat with them\n\n")
	chat_window.config(state=tk.DISABLED) #user can't type in here
	#-------------------------------------------------------------------------#
	# Bottom frame will hold the text input that the user can write in,       #
	# the radio buttons for a user to select whether they will be the         #
	# client or the server & the buttons add contacts or connect to a contact #
 	#-------------------------------------------------------------------------#
	text_input = tk.Text(bottom_frame, width=40, height=5) #create text input
	text_input.pack(side=tk.LEFT, expand=tk.YES, fill=tk.BOTH)
	text_input.config(highlightbackground="black")

	send_button = ttk.Button(bottom_frame, style="send.TButton",text="\n\nsend\n\n", command=processUserText)
	send_button.pack()
	send_button_style = ttk.Style()
	send_button_style.configure('send.TButton', foreground='black', fontweight='bold', borderwidth=2, bordercolor='black', height=5)
	#----------------------------------------------#
	# Menu Bar- user can add contacts,             #
	# connect to users, and disconnect to users    #
	#----------------------------------------------#
	global connect_role 
	connect_role= "server" #the entry is a server
	global connection_status
	connection_status="disconnected"

	menubar = tk.Menu(root)
	#menu for user
	chat_menu = tk.Menu(menubar, tearoff=0)
	chat_menu.add_command(label="Add Contact", command=lambda: add_contact_window(root))
	chat_menu.add_command(label="Connect", command=lambda:connect_window(root))
	chat_menu.add_command(label="Disconnect", command=lambda:disconnect())
	chat_menu.add_command(label="Exit", command=lambda: root.destroy())
	menubar.add_cascade(label="Chat Menu", menu=chat_menu)
	root.config(menu=menubar)


	


	#things to add later
	#connect_button = Button(botton_frame, textvariable=connection_status, command=connects(clientType))
	#connecter.pack(side=BOTTOM, expand=YES, fill=BOTH)
	#client_rb=Radiobutton(bottom_frame, text="client", variable=connect_role, value="client", command=toClient)
	#client_rb.pack(anchor=E, expand=YES, fill=BOTH)
	#server_rb=Radiobutton(bottom_frame, text="server", variable=connect_role, value="server", command=toSever)
	#server_rb.pack(anchor=E, expand=YES, fill=BOTH)
	root.mainloop() #render GUI

if __name__ == '__main__':
    main()