import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.EOFException;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.InetAddress;
//import java.net.ServerSocket;
import java.net.Socket;
import java.security.GeneralSecurityException;
import java.security.PrivateKey;

import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;

public class Client extends JFrame{

	private static final long serialVersionUID = 1L;
	private JTextField enterField;
	private JTextArea displayArea;
	private ObjectOutputStream output;
	private ObjectInputStream input;
	private String message = "";
	private String chatServer;
	private Socket client;
	
	Chat client1;
	Chat clientConnection;
	
	public Client(String host){
		super("Client");
		
		//Encryption stuff
		client1 = new Chat("Armond", "Steph");
		clientConnection = new Chat("Steph", "Armond");
		PrivateKey clientConnectionPrivateKey = clientConnection.generateMyKeys();
		PrivateKey clientPrivateKey = client1.generateMyKeys();
		
		try {
			client1.setDecryptionKey(clientConnectionPrivateKey);
			clientConnection.setDecryptionKey(clientPrivateKey);
		} catch (GeneralSecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		
		
		//End of Encryption stuff
		
		chatServer = host;
		
		enterField = new JTextField();
		enterField.setEditable(false);
		enterField.addActionListener(
			new ActionListener(){
				public void actionPerformed(ActionEvent event){
					sendData(event.getActionCommand());
					enterField.setText("");
				}
			}		
		);
		add(enterField, BorderLayout.NORTH);
		
		displayArea = new JTextArea();
		add(new JScrollPane(displayArea), BorderLayout.CENTER);
		setSize(300, 150);
		setVisible(true);
				
	}
	
	public void runClient(){
		try{
			connectToServer();
			getStreams();
			processConnection();
		}
		catch(EOFException eofException){
			displayMessage("\nClient Terminated Connection");
		}
		catch(IOException ioException){
			ioException.printStackTrace();
		}
		finally{
			closeConnection();
		}
	}
	
	private void connectToServer() throws IOException{
		displayMessage("Attempting connection\n");
		
		client = new Socket(InetAddress.getByName(chatServer), 23555);
		displayMessage("Connected to: " + client.getInetAddress().getHostName());
	}
	
	private void getStreams() throws IOException{
		output = new ObjectOutputStream(client.getOutputStream());
		output.flush();
		
		input = new ObjectInputStream(client.getInputStream());
		displayMessage("\nGot I/O Streams\n");
	}
	
	private void processConnection() throws IOException{
		setTextFieldEditable(true);
		
		do{
			try{
				message = (String) input.readObject();
				displayMessage("\n" + message);
			}
			catch(ClassNotFoundException classNotFoundException){
				displayMessage("\nUnknown object type received");
			}
		} while(!message.equals("SERVER>>> TERMINATE"));
	}
	private void closeConnection(){
		displayMessage("\nClosing connection\n");
		setTextFieldEditable(false);
		
		try{
			output.close();
			input.close();
			client.close();
		}
		catch(IOException ioException){
			ioException.printStackTrace();
		}
	}
	
	private void sendData(String message){
		byte[] encryptedMessage = clientConnection.encrypt(message);
		System.out.println("Encrypted Message: " + encryptedMessage);
		try{
			output.writeObject("CLIENT>>> " + encryptedMessage);
			output.flush();
			displayMessage("\nCLIENT>>> " + encryptedMessage);
		}
		catch(IOException ioException){
			displayArea.append("\nError writing object");
		}
	}
	
	private void displayMessage(final String messageToDisplay){
		//String 
		SwingUtilities.invokeLater(
			new Runnable(){
				public void run(){
					displayArea.append(messageToDisplay);
				}
			}
		);
	}
	
	private void setTextFieldEditable(final boolean editable){
		SwingUtilities.invokeLater(
			new Runnable(){
				public void run(){
					enterField.setEditable(editable);
				}
			}
		);
	}
	/*
	private static final long serialVersionUID = 1826871141583020576L;
	private String clientHost;	//host name for server
	private Socket connection; // connection to server
	private ObjectInputStream input; // input from server
	private ObjectOutputStream output;
	private final int port = 23555; 
	private JTextArea displayArea; // textfield to display conversation
	private JTextField messageField;
	
	public Client(String host){
		super("Messenger Client");
		clientHost = host; // set name of server
		
		messageField = new JTextField("Enter message here.");
		messageField.setEditable(true);
		messageField.addKeyListener(new MyKeyListener());
		add(messageField, BorderLayout.SOUTH);
		
		displayArea = new JTextArea();
		add(new JScrollPane(displayArea), BorderLayout.CENTER);
		
		setSize(450,200);
		setVisible(true);
	}
	
	// start the client thread
	public void startClient(){
		try{
			connectToServer();
			getStreams();
			handleConnection();
		}catch(Exception e){
			
		}finally{
			closeConnection();
		}
	}
	public void connectToServer() throws IOException{
		InetAddress ipAddress = InetAddress.getByName(clientHost);
		connection = new Socket(ipAddress,port);
	}
	public void getStreams()throws IOException{
		input = new ObjectInputStream(connection.getInputStream());
		output = new ObjectOutputStream(connection.getOutputStream());
		
	}
	public void handleConnection(){
		ExecutorService worker = Executors.newFixedThreadPool(1);
		worker.execute(this); // execute client
	}

	// control thread that allows continuous update of displayArea
	@Override
	public void run() {
		// continually keep the connection open and read messages from the server
		while(true){
			try {
				Object ob = input.readObject();
				// if there is input data from the server
				if(ob.getClass().equals(String.class)){
					processMessageDecryption(((String) ob).toString());
				}
				
			} catch (ClassNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} catch(NullPointerException e){
				e.printStackTrace();
			}
		}
	}
	
	private void processMessageEncryption(String message){
		//encryption of message that returns key
		//TO DO: ENCRYPTION
		
		//send encrypted message
		sendMessage(message);
	}
	
	private void sendMessage(String message){
		try {
			output.writeObject(message);
			output.flush();
			displayMessage("ME: " + message+"/n");
		} catch (Exception e) {
			//e.printStackTrace();
		}
	}
	
	// process messsage recieved by client
	private void processMessageDecryption(String message){
		// decryption of key that returns message
		// TO DO: DECRYPTION
		
		//display decrypted message
		displayMessage("Other Client: " + message + "\n");
	}
	*/
	/**
	 * 
	 * @param messageToDisplay
	 */
	/*
	private void displayMessage(final String messageToDisplay){
		SwingUtilities.invokeLater(
			new Runnable(){
				public void run(){
					displayArea.append(messageToDisplay);
				}
			}
		);
	}
	private void closeConnection(){
		try{
			output.close();
			input.close();
			connection.close();
		}catch(IOException io){
			
		}
	}
	
	private class MyKeyListener implements KeyListener {

		@Override
		public void keyPressed(KeyEvent e) {
			
			
			if(e.getKeyCode() == KeyEvent.VK_ENTER){
				processMessageEncryption(messageField.getText());
			}
		}

		@Override
		public void keyReleased(KeyEvent arg0) {}

		@Override
		public void keyTyped(KeyEvent arg0) {}
		
	}
	*/
}
