/************************************************************************************************************
 * NEED TO ADD JARS FOR LIBRARIES FOR TEXTING SERVICE, CHART, AND SERIAL CABLE
 * All Jars needed in JarLibrary Folder
 * Right click project --> Build Path --> Configure Build Path --> Add External JARs
 *************************************************************************************************************/

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Random;
import java.util.Scanner;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JPanel;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.NumberTickUnit;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.NumberTickUnit;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYItemRenderer;
import org.jfree.data.xy.XYDataset;

import com.fazecast.jSerialComm.SerialPort;

//TEXT MESSAGE IMPORTS
import com.twilio.sdk.TwilioRestClient;
import com.twilio.sdk.TwilioRestException;
import com.twilio.sdk.resource.factory.MessageFactory;
import com.twilio.sdk.resource.instance.Message;
import org.apache.http.NameValuePair;
import org.apache.http.message.BasicNameValuePair;
 
import java.util.ArrayList;
import java.util.List;

/*
 * http://stackoverflow.com/questions/7231824/setting-range-for-x-y-axis-jfreechart
 * */
public class SensorGraph {
	public static final String ACCOUNT_SID = "AC2d94e3ab9715a2de6a58fead2c2861fe";
	public static final String AUTH_TOKEN = "17ae461a1db9348ab8fcfe938af86111";
	
	static SerialPort chosenPort;
	static int x = 0;

	public static void main(String[] args) {
		//Create and configure the window
		JFrame window = new JFrame();
		window.setTitle("Temperature Readings");
		//window.setSize(900, 900);
		window.setExtendedState(window.MAXIMIZED_BOTH);
		window.setLayout(new BorderLayout());
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		//Create a drop-down box and connect button, then place them at the top of the window
		JComboBox<String> portList = new JComboBox<String>();
		JButton connectButton = new JButton("Connect");
		JPanel topPanel = new JPanel();
		topPanel.add(portList);
		topPanel.add(connectButton);
		window.add(topPanel, BorderLayout.NORTH);

		//Populate the drop-down box
		SerialPort[] portNames = SerialPort.getCommPorts();
		//SerialPort myPort = SerialPort.getCommPort("COM3");
		for(int i=0; i<portNames.length; i++){
			System.out.println("Port Name: " + portNames[i]);
			System.out.println("Port Name: " + portNames[i].getSystemPortName());
		}
		
		for (int i = 0; i < portNames.length; i++){
			portList.addItem(portNames[i].getSystemPortName());
		}
/*------------------------------------------------------------------------------------------------
 * 		CREATE GRAPH
 *------------------------------------------------------------------------------------------------*/		
		
		XYSeries series = new XYSeries("Temperature Readings");
		//series.setMaximumItemCount(300);
		XYSeriesCollection dataset = new XYSeriesCollection(series);
		JFreeChart chart = ChartFactory.createXYLineChart("Temperature", "Time (seconds)", "Degrees (C)", dataset);
		
		XYPlot xyPlot = (XYPlot) chart.getPlot();
        xyPlot.setDomainCrosshairVisible(true);
        xyPlot.setRangeCrosshairVisible(true);
        XYItemRenderer renderer = xyPlot.getRenderer();
        renderer.setSeriesPaint(0, Color.black);
        NumberAxis domain = (NumberAxis) xyPlot.getDomainAxis();
        domain.setRange(0, 300);
        domain.setTickUnit(new NumberTickUnit(20.0));
        domain.setVerticalTickLabels(true);
        domain.setTickLabelsVisible(false);
        NumberAxis range = (NumberAxis) xyPlot.getRangeAxis();
        range.setRange(-25, 75);
        range.setTickUnit(new NumberTickUnit(10));
        domain.setLabel("Time Elapsed (s)");
		
		window.add(new ChartPanel(chart), BorderLayout.CENTER);

/*------------------------------------------------------------------------------------------------
 * 		INCOMING DATA TO DRAW GRAPH
 *------------------------------------------------------------------------------------------------*/
		//Configure the connect button and use another thread to listen for data
		connectButton.addActionListener(new ActionListener() {
			int temperatureReading;
			int tempExtremeCount=0;
			int[] temperatureList= new int[300];
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				if (connectButton.getText().equals("Connect")) {
					Thread thread = new Thread() {
						@Override
						public void run() {
							int tempCount=0;
							for(int i=0; i<1000; i++){
								temperatureReading= randInt();
								
								if(tempCount<300){
									temperatureList[tempCount]= temperatureReading;
								}
								
								else{
									for(int j=0; j<299; j++){
										temperatureList[j]= temperatureList[j+1];
									}
									temperatureList[299]= temperatureReading;
								}
								System.out.println(temperatureList[0]);
								
								//Code to check if text should be sent for too high or too low of a temperature
								if(temperatureReading > 63 || temperatureReading< -10){
									tempExtremeCount++;
								}
								
								if(tempCount<300){
									series.add(x++, temperatureReading);
								}
								else{
									series.clear();
									for(int k=0; k< temperatureList.length; k++){
										series.add(k, temperatureList[k]);
									}
								}
								
								//NOTE: If sleep time is too fast, processor can't keep up with
								//	 	clearing and repainting the graph and exceptions occur
								try {
									Thread.sleep(500);
								} catch (InterruptedException e) {
									e.printStackTrace();
								}
								window.repaint();
								tempCount++;
							} 
						}
					};
					thread.start();
					
					
				/*
					// attempt to connect to the serial port
					chosenPort = SerialPort.getCommPort(portList.getSelectedItem().toString());
					chosenPort.setComPortTimeouts(SerialPort.TIMEOUT_SCANNER, 0, 0);
					if (chosenPort.openPort()) {
						connectButton.setText("Disconnect");
						portList.setEnabled(false);
					}

					// create a new thread that listens for incoming text and
					// populates the graph
					Thread thread = new Thread() {
						@Override
						public void run() {
							Scanner scanner = new Scanner(chosenPort.getInputStream());
							while (scanner.hasNextLine()) {
								try {
									String line = scanner.nextLine();
									int temperatureReading = Integer.parseInt(line);
									
									if(temperatureReading>63 || temperatureReading<-10){
										sendTextMessage(temperatureReading);
									}
									
									series.add(x++, 1023 - temperatureReading);
									window.repaint();
								} catch (Exception e) {
								}
							}
							scanner.close();
						}
					};
					thread.start();
					*/
				} else {
					// disconnect from the serial port
					chosenPort.closePort();
					portList.setEnabled(true);
					connectButton.setText("Connect");
					series.clear();
					x = 0;
				}
				
			}
		});

		// show the window
		window.setVisible(true);

	}

/*------------------------------------------------------------------------------------------------
 * 		RANDOM NUMBER METHOD
 * ------------------------------------------------------------------------------------------------*/
	public static int randInt() {
	    int max= 63;
	    int min= -10;
		Random rand = new Random();
	    int randomNum = rand.nextInt((max - min) + 1) + min;
	    return randomNum;
	}

/*------------------------------------------------------------------------------------------------
 * 		SEND TEXT METHOD
 * ------------------------------------------------------------------------------------------------*/
	public static void sendTextMessage(int temperature){
		String temperatureString = Integer.toString(temperature);
		TwilioRestClient client = new TwilioRestClient(ACCOUNT_SID, AUTH_TOKEN);
		String temperatureTooLow = "Our sensor is picking up some frigid temperatures! The current reading is " + temperature + " degrees Celsius";
		String temperatureTooHot = "Our sensor is picking up some hot temperatures! The current reading is " + temperature + " degrees Celsius";
	    String body;
	    //body = "3:38";
	    body= "Armond testing the text method";
	    String number;
	    number = "+15153714907";
	    //SEND TEXT MESSAGE TO xxx-xxx-xxxx
	    String armond= "+15154025546";
	 
	    // Build a filter for the MessageList
	    List<NameValuePair> params = new ArrayList<NameValuePair>();
	    if(temperature< -10){
	    	params.add(new BasicNameValuePair("Body", temperatureTooLow));
	    }
	    else{
	    	params.add(new BasicNameValuePair("Body", temperatureTooHot));
	    }
	    //params.add(new BasicNameValuePair("To", number));
	    params.add(new BasicNameValuePair("From", "+15153032132"));
	    //params.add(new BasicNameValuePair("Body", temperatureString));
	    params.add(new BasicNameValuePair("To", armond));
	    //params.add(new BasicNameValuePair("From", armond));
	 
	    MessageFactory messageFactory = client.getAccount().getMessageFactory();
	    Message message = null;
		try {
			message = messageFactory.create(params);
		} catch (TwilioRestException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    System.out.println(message.getSid());
		
		
	}

}
