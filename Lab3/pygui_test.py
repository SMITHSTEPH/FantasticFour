#!/usr/bin/python
import sys
import Tkinter  as tk
import ttk as ttk
from pyaxo import Axolotl

def toClient():
    global connect_role 
    connect_role = "client"
    print connect_role

def toSever():
	global connect_role
	connect_role = "server"
	print connect_role

def connect(connect_role):
	print "in connect"
	global connection_status
	connection_status="connected"

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

def add_contact_window(master):
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
    password_input=tk.Entry(password_frame, width=20)
    password_input.pack(anchor=tk.N, side=tk.RIGHT, expand=tk.YES, fill=tk.X)

    keys_button=tk.Button(root, text="Generate My Keys", command=lambda: generate_keys(nick_keys_input, nick_input, other_nick_input, password_input))
    keys_button.pack(expand=tk.YES, fill=tk.BOTH)

    nick_keys_frame=tk.Frame(root, width=40)
    nick_keys_frame.pack(expand=tk.YES, fill=tk.BOTH)
    nick_keys_label=tk.Label(nick_keys_frame, text='My Keys: ')
    nick_keys_label.pack(anchor=tk.W, side=tk.TOP)
    nick_keys_input = tk.Text(nick_keys_frame, width=40, height=5)
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

    keys_button=tk.Button(root, text="Add Contact", command=lambda: add_contact())
    keys_button.pack(expand=tk.YES, fill=tk.BOTH)


def generate_keys(text_area, nick_textbox, other_nick_textbox, password_textbox):
	# newaxo = Axolotl(NICK, dbname=OTHER_NICK+'.db')
	# newaxo.printKeys()
	# identity = raw_input('What is the identity key for the other party? ')
	# ratchet = raw_input('What is the ratchet key for the other party? ')
	# handshake = raw_input('What is the handshake key for the other party? ')
	# newaxo.initState(OTHER_NICK, binascii.a2b_base64(identity),
 	#    	binascii.a2b_base64(handshake),binascii.a2b_base64(ratchet))
	# newaxo.saveState()
	print nick_textbox.get()
	print other_nick_textbox.get()
	print password_textbox.get()

	identity_key = " identity_key: random1\n"
	fingerprint = " fingerprint: random2\n"
	rachet_key = " rachet_key: random3\n"
	handshake_key= " handshake_key: random4\n"
	text_area.config(state=tk.NORMAL)
	text_area.insert(tk.END, identity_key+fingerprint+rachet_key+handshake_key)
	text_area.config(state=tk.DISABLED)
    
def username_options_go(name, window):
    """Processes the options entered by the user in the
    server options window.

    """
    processUserCommands("nick", [name])
    window.destroy()    

def username_options_window(master):
    """Launches username options window for setting username."""
    top = tk.Toplevel(master)
    top.title("Username options")
    top.grab_set()
    tk.Label(top, text="Username:").grid(row=0)
    name = tk.Entry(top)
    name.focus_set()
    name.grid(row=0, column=1)
    go = tk.Button(top, text="Change", command=lambda:
                username_options_go(name.get(), top))
    go.grid(row=1, column=1)   
    
def username_options_go(name, window):
    """Processes the options entered by the user in the
    server options window.

    """
    processUserCommands("nick", [name])
    window.destroy()     

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
	chat_menu.add_command(label="Set Username", command=lambda: username_options_window(root))
	chat_menu.add_command(label="Connect", command=lambda:connect(connect_role))
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