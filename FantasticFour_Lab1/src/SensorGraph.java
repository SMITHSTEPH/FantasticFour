/************************************************************************************************************
 * PRINCIPLES OF ELECTRICAL ENGINEERING DESIGN: LAB 1 
 * 			PROFESSOR ANDERSEN
 * 				SPRING 2016
 * 
 * TEAM MEMBERS: ARMOND LUTHENS, STEPHANIE SMITH, BRIAN SPEER, JOE HALLMAN
 *************************************************************************************************************/


/************************************************************************************************************
 * NEED TO ADD JARS FOR LIBRARIES FOR TEXTING SERVICE, CHART, AND SERIAL CABLE
 * All Jars needed in JarLibrary Folder
 * Right click project --> Build Path --> Configure Build Path --> Add External JARs
 *************************************************************************************************************/
//JFRAME LIBRARY IMPORTS
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseListener;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Scanner;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import java.awt.Component;

//CHART LIBRARY IMPORTS
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.NumberTickUnit;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYItemRenderer;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.NumberTickUnit;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.xy.XYDataset;

//JSERIALCOMM LIBRARY IMPORTS
import com.fazecast.jSerialComm.SerialPort;
import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import java.io.FileDescriptor;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

//TEXT MESSAGE IMPORTS
import com.twilio.sdk.TwilioRestClient;
import com.twilio.sdk.TwilioRestException;
import com.twilio.sdk.resource.factory.MessageFactory;
import com.twilio.sdk.resource.instance.Message;
import org.apache.http.NameValuePair;
import org.apache.http.message.BasicNameValuePair;

//BASIC JAVA LIRBARY IMPORTS
import java.util.ArrayList;
import java.util.List;

//INTERNET CONNECTION LIBRARY IMPORTS
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.UnknownHostException;
import java.io.IOException;

public class SensorGraph {
	public static final String ACCOUNT_SID = "AC2d94e3ab9715a2de6a58fead2c2861fe"; //texting service accound id
	public static final String AUTH_TOKEN = "17ae461a1db9348ab8fcfe938af86111"; //texting service authentication token

	static SerialPort chosenPort; //port being used
	static int x = 0; //counter for chart series

	public static void main(String[] args) {
		JFrame window = new JFrame(); //Create New JFrame
		window.setTitle("Temperature Readings");
		// window.setSize(900, 900);
		window.setMinimumSize(new Dimension(1200, 600));
		window.setExtendedState(window.MAXIMIZED_BOTH);
		window.setLayout(new BorderLayout());
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		
		JComboBox<String> portList = new JComboBox<String>(); //create a drop-down box and connect button, then place them at the top of the window
		JButton connectButton = new JButton("Connect"); //button to connect port to program
		JButton virtualButton = new JButton("LIGHT LEDs"); //virtual button to light LEDs
		virtualButton.setOpaque(true); 
	    virtualButton.setForeground(Color.RED);
		JLabel enterPhone = new JLabel("10-digit phone Number:");
		JTextField phoneNumberTextField = new JTextField(20); //field to enter 10 digit phone number in
		JLabel upperBound = new JLabel("lower bound:");
		JTextField upperBoundField = new JTextField(3); //field to enter upper temperature limit
		JLabel lowerBound = new JLabel("upper bound:");
		JTextField lowerBoundField = new JTextField(3); //field to enter lower temperature limit
		JPanel topPanel = new JPanel();
		
		//add fields, labels, and buttons to topPanel
		topPanel.add(enterPhone);
		topPanel.add(phoneNumberTextField);
		topPanel.add(portList);
		topPanel.add(connectButton);
		topPanel.add(virtualButton);
		topPanel.add(lowerBound);
		topPanel.add(lowerBoundField);
		topPanel.add(upperBound);
		topPanel.add(upperBoundField);
		
		//add topPanel to window
		window.add(topPanel, BorderLayout.NORTH);
		
		JPanel bottomPanel = new JPanel(); //panel for real time temperature display
		JLabel realTimeTempLabel = new JLabel("Real Time Temperature:");
		JLabel realTimeTemp = new JLabel("24 "); //label that gets constantly reset and repainted
		JLabel realTimeTempUnits = new JLabel("Degrees Celsius");
		
		//add labels to bottom panel
		bottomPanel.add(realTimeTempLabel);
		bottomPanel.add(realTimeTemp);
		bottomPanel.add(realTimeTempUnits);
		bottomPanel.setPreferredSize(new Dimension(100, 50));
		Font f = new Font ("Arial", Font.BOLD, 26);
		realTimeTempLabel.setFont(f);
		realTimeTemp.setFont(f);
		realTimeTempUnits.setFont(f);
		
		//add bottomPanel to window
		window.add(bottomPanel, BorderLayout.SOUTH);

		//populate the drop-down box
		SerialPort[] portNames = SerialPort.getCommPorts(); //searches for CommPorts on machine
		SerialPort leftUSBPort = SerialPort.getCommPort("/dev/cu.usbmodem1421"); //specific port on our local machine
		SerialPort rightUSBPort = SerialPort.getCommPort("/dev/cu.usbmodem1411"); //specific port on our local machine
		
		//check to print the ports detected to console
		for (int i = 0; i < portNames.length; i++) {
			System.out.println("Port Name: " + portNames[i]);
			System.out.println("Port Name: " + portNames[i].getSystemPortName());
		}
		
		//adds ports to dropdown list
		for (int i = 0; i < portNames.length; i++) {
			portList.addItem(portNames[i].getSystemPortName());
		}
		
		
		portList.addItem(leftUSBPort.getSystemPortName());
		portList.addItem(rightUSBPort.getSystemPortName());
		//portList.addItem("Left USB Port");
		//portList.addItem("Right USB Port");
		
/*------------------------------------------------------------------------------------------------
 * 		CREATE GRAPH
 *------------------------------------------------------------------------------------------------*/
		XYSeries series = new XYSeries("Temperature Readings");
		XYSeriesCollection dataset = new XYSeriesCollection(series);
		JFreeChart chart = ChartFactory.createXYLineChart("Temperature", "Time (seconds)", "Degrees (C)", dataset);
		XYPlot xyPlot = (XYPlot) chart.getPlot();
		xyPlot.setDomainCrosshairVisible(true);
		xyPlot.setRangeCrosshairVisible(true);
		XYItemRenderer renderer = xyPlot.getRenderer();
		renderer.setSeriesPaint(0, Color.black);
		NumberAxis domain = (NumberAxis) xyPlot.getDomainAxis();
		domain.setRange(0, 300); //set domain of graph (x-axis)
		domain.setTickUnit(new NumberTickUnit(20.0)); //sets x-axis interval rate
		domain.setVerticalTickLabels(true);
		domain.setTickLabelsVisible(false); //hide the domain intervals and tick marks
		NumberAxis range = (NumberAxis) xyPlot.getRangeAxis();
		range.setRange(-25, 75); //sets range of graph (y-axis) 
		range.setTickUnit(new NumberTickUnit(10)); //sets y-axis interval rate
		domain.setLabel("Time Elapsed (s)");
		
		//adds the chart to the window
		window.add(new ChartPanel(chart), BorderLayout.CENTER); 
		
/*------------------------------------------------------------------------------------------------
 * 		INCOMING DATA TO DRAW GRAPH
 *------------------------------------------------------------------------------------------------*/
	//Connect button listens for click to connect to the port the user selects from dropdown list
	connectButton.addActionListener(new ActionListener() {
			int temperatureReading; //temperature read in from Arduino
			int tempExtremeCount = 0; //count that increments each time out of bounds temperature read in
			int tempHotCount = 0; //hot temperature out of bounds counter
			int tempColdCount = 0; //cold temperature out of bounds counter
			String recipientNumber=""; //default recipient number
			int upperRangeInt= 63; //default upper limit of temperature
			int lowerRangeInt= -10; //default lower limit of temperature
			boolean internet= false; //Internet connection boolean
			int[] temperatureList = new int[300]; //list that holds most recent 300 temperature values
			
			//Connect button gets clicked
			public void actionPerformed(ActionEvent arg0) {
				if (connectButton.getText().equals("Connect")) {
					recipientNumber= phoneNumberTextField.getText(); //gets phone number in cell phone field
					String lowerRange= lowerBoundField.getText(); //gets lower limit in lower range field
					String upperRange= upperBoundField.getText(); //gets upper limit in upper range field
					
					if(!lowerRange.equals("") && !upperRange.equals("")){
						lowerRangeInt = Integer.parseInt(lowerRange); //changes string to integer
						upperRangeInt = Integer.parseInt(upperRange); //changes string to integer
						range.setRange(lowerRangeInt, upperRangeInt); //sets the new range on the graph
						window.repaint(); //repaints the graph with the new ranges
					}
					
					//attempt to connect to the serial port
					chosenPort = SerialPort.getCommPort(portList.getSelectedItem().toString());
					chosenPort.setComPortTimeouts(SerialPort.TIMEOUT_SCANNER, 0, 0);
					if (chosenPort.openPort()) {
						connectButton.setText("Disconnect");
						portList.setEnabled(false);
					}
					 
					//create new thread for the graph data to run on and update the graph
					Thread thread = new Thread() {
						@Override
						public void run() {
							//Try to run the whole program, if no connection to arduino, exception gets thrown
							try{
								Scanner scanner = new Scanner(chosenPort.getInputStream()); //open scanner for input stream from arduino
								OutputStream output = chosenPort.getOutputStream(); //create output stream to send serialized data back to arduino
								internet= internetCheck(); //call method to check if machine is connected to internet
								if(internet == true){
									try {
										output.write(0); //send signal to arduino that machine is ready
									} catch (IOException e2) {
										e2.printStackTrace();
									}
								}
								else{
									try {
										output.write(3); //send signal to arduino that machine is NOT ready (No Internet)
									} catch (IOException e2) {
										e2.printStackTrace();
									}
								}
								
								int tempCount = 0; //counter for the number of temperatures read in from arduino
						
								while (scanner.hasNextLine()) {
									try {
										String temperatureString = scanner.nextLine(); //reads in temperature as a string from arduino
										realTimeTemp.setText(temperatureString); //updates the temperature display on the GUI
										bottomPanel.repaint(); //repaints the panel
										
										int temperatureReading = Integer.parseInt(temperatureString); //parses temperatureString to change to integer
										
										if (temperatureReading > 63 || temperatureReading < -10) {
											tempExtremeCount++; //counter of out of bounds temperatures on either limit
										}
										//send text if temperature is above upper limit
										if (temperatureReading > upperRangeInt && temperatureReading != 1000) {
											sendTextMessage(temperatureReading, tempHotCount, recipientNumber);
											tempHotCount++;
										} 
										//send text if temperature is below lower limit
										else if (temperatureReading < lowerRangeInt) {
											sendTextMessage(temperatureReading, tempColdCount, recipientNumber);
											tempColdCount++;
										}
										//arduino sends 1000 (assumed impossible temperature for this hardware) if sensor is unplugged
										else if(temperatureReading == 1000){
											chart.setTitle("UNPLUGGED SENSOR"); //alert on monitor that sensor cable is unplugged
											chart.setBackgroundPaint(Color.red); //repaints the window with message
										}
										//arduino sends 2000 (assumed impossible temperature for this hardware) if third box switch is off
										else if(temperatureReading == 2000){
											chart.setTitle("NO DATA AVAILABLE"); //alert on monitor that no data is being collected
											chart.setBackgroundPaint(Color.red); //repaints the window with message
										}
										else{
											//default window when conditions are normal
											chart.setTitle("TEMPERATURE READINGS"); 
											chart.setBackgroundPaint(Color.white);
										}
	
										if (tempCount < 300) {
											temperatureList[tempCount] = temperatureReading; //add temperatures in array until 300
										}
										else {
											//after 300 values have been collected, simulate graph movement
											for (int j = 0; j < 299; j++) {
												temperatureList[j] = temperatureList[j + 1]; //shift each element of array to left
											}
											temperatureList[299] = temperatureReading; //add most recent temperature reading
										}
										System.out.println(temperatureList[0]); //prints most current temperature
	
										if (tempCount < 300) {
											series.add(x++, temperatureReading); //adds only newest value for repaint
										} else {
											series.clear(); //clears the whole graph and adds all the new shifted values
											for (int k = 0; k < temperatureList.length; k++) {
												series.add(k, temperatureList[k]);
											}
										}
	
										// NOTE: If sleep time is too fast, processor can't keep up with
										// clearing and repainting the graph and exceptions occur
										try {
											Thread.sleep(500); //sleep time interval for repainting graph new new value
										} catch (InterruptedException e) {
											e.printStackTrace();
										}
										window.repaint(); //updates the graph with the newest value
										tempCount++; //increments total temperature count
										
										//VIRTUAL BUTTON LISTENER
										virtualButton.addMouseListener(new MouseListener(){
											@Override
											public void mousePressed(java.awt.event.MouseEvent e) {
												// sends message to arduino while virtual button is being pressed down
												try {
													System.out.println("Button Pressed");
													//send 1 to arduino for: push third box button 
													output.write(1);
												} catch (IOException e1) {
													e1.printStackTrace();
												}
											}
	
											@Override
											public void mouseReleased(java.awt.event.MouseEvent e) {
												// sends message to arduino when virtual button is released
												try {
													//send 2 to arduino for: release third box button
													output.write(2);
												} catch (IOException e1) {
													e1.printStackTrace();
												}
											}
											
											//UNUSED IMPLEMENTED METHODS
											@Override
											public void mouseClicked(java.awt.event.MouseEvent e) {} //no action on just click
											@Override
											public void mouseEntered(java.awt.event.MouseEvent e) {} //no action on mouseEntered
											@Override
											public void mouseExited(java.awt.event.MouseEvent e) {} //no action on mouseExited
	
										});
									} 
									catch (Exception e){}
								} //END WHILE LOOP
							}
							//catch if machine is not properly connected to arduino
							
							catch(Exception e){
								System.out.println("NO CONNECTION");
								chart.setTitle("NO CONNECTION AVAILABLE"); //alert on monitor that no data is being collected
								chart.setBackgroundPaint(Color.red); //repaints the window with message
								window.repaint();
								JOptionPane.showMessageDialog(window, "Please connect to a device in your selected port");
							}
							
						} 
					};
					//BEGIN THREAD
					thread.start();

				} 
				else {
					//disconnect from the serial port
					chosenPort.closePort(); //lib method to close the port
					portList.setEnabled(true); 
					connectButton.setText("Connect"); //reset button from 'disconnect' to 'connect'
					//series.clear(); //clear the graph
					x = 0;
				}
			}
		});

		window.setVisible(true); //show the JFrame window
	}

/*------------------------------------------------------------------------------------------------
 * 		SEND TEXT METHOD
 * ------------------------------------------------------------------------------------------------*/
/**
 * This method gets called if the temperature exceeds a lower or upper bound that the user has
 * entered.  When this method is called, an sms text is sent to the 10-digit cell phone number
 * that was entered in the GUI.  If no number was entered,
 * @param temperature
 * @param textCount
 * @param recipientNumber
 */
	public static void sendTextMessage(int temperature, int textCount, String recipientNumber) {
		recipientNumber= "+" + recipientNumber; //appends plus sign to number (needed for API)
		int maxTextCount=15;
		String temperatureString = Integer.toString(temperature); //converts the temperature to a string
		
		TwilioRestClient client = new TwilioRestClient(ACCOUNT_SID, AUTH_TOKEN);
		
		//Message to send if too cold
		String temperatureTooLow = "Our sensor is picking up some frigid temperatures! The current reading is "
				+ temperature + " degrees Celsius";
		
		//Message to send if too hot
		String temperatureTooHot = "Our sensor is picking up some flamin temperatures! The current reading is "
				+ temperature + " degrees Celsius";

		// SEND TEXT MESSAGE TO x-xxx-xxx-xxxx
		String defaultNumber = "+15154025546"; //ARMOND CELL

		// Build a filter for the MessageList
		List<NameValuePair> params = new ArrayList<NameValuePair>();

		//HOT OR COLD MESSAGE SENT (Condition: will only send a max of 15 messages)
		if (temperature < -10 && textCount < maxTextCount) {
			params.add(new BasicNameValuePair("Body", temperatureTooLow));
		} 
		else if (temperature > 63 && textCount < maxTextCount) {
			params.add(new BasicNameValuePair("Body", temperatureTooHot));
		}

		//FROM SERVICE
		params.add(new BasicNameValuePair("From", "+15153032132"));

		// TO RECEIVER
		if(recipientNumber.length() == 11){
			//sends text to number passed in if valid
			params.add(new BasicNameValuePair("To", recipientNumber));
		}
		else{
			//sends text to Armond's cell phone (demonstration purposes only)
			params.add(new BasicNameValuePair("To", defaultNumber));
		}

		MessageFactory messageFactory = client.getAccount().getMessageFactory();
		Message message = null;
		try {
			message = messageFactory.create(params);
		} catch (TwilioRestException e) {
			e.printStackTrace();
		}
		System.out.println(message.getSid());
	}

/*------------------------------------------------------------------------------------------------
 * 		INTERNET CONNECTION METHOD
 * ------------------------------------------------------------------------------------------------*/
/**
 * Method that tests if the machine running the program is connected to the internet by
 * trying to connect to http://www.google.com
 * @return boolean indicating if there was a connection
 */
	public static boolean internetCheck(){
        boolean connection=false;
            try {
                try {
                    URL url = new URL("http://www.google.com");
                    HttpURLConnection con = (HttpURLConnection) url.openConnection();
                    con.connect(); //connect URL to the Internet
                    
                    //response code sent from Internet if connection is established
                    if (con.getResponseCode() == 200) {
                        //connection established
                        connection=true;
                    }
                } 
                catch (Exception exception) {
                	//NO connection established
                }
            } 
            catch (Exception e) {
                e.printStackTrace();
            }
        return connection;
    }
	
} //END OF SENSOR GRAPH CLASS
