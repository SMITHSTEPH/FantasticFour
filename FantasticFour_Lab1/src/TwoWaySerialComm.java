import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;

import java.io.FileDescriptor;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Scanner;

public class TwoWaySerialComm 
{
	 public TwoWaySerialComm(){super();}
	  
	    
	    void connect ( String portName ) throws Exception
	    {
	        CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(portName);
	        CommPort commPort = portIdentifier.open(this.getClass().getName(),2000);
	        if ( commPort instanceof SerialPort )
	        {
	            SerialPort serialPort = (SerialPort) commPort;
	            serialPort.setSerialPortParams(57600,SerialPort.DATABITS_8,SerialPort.STOPBITS_1,SerialPort.PARITY_NONE);
	            //InputStream in = serialPort.getInputStream();
	            OutputStream out = serialPort.getOutputStream();
	            int c=0;
	            Scanner sc = new Scanner(System.in);
	            while(c!=-1)
	            {
	            	System.out.print("input: ");
                	c =  sc.nextInt();
                	out.write(c);
                	out.flush();
                	System.out.println("c is:" +c);
                	if(c==-1){System.out.println("-1!!!");}
	            }	
                    //
	            System.out.println("out of while loop");
	            sc.close();
	            //(new Thread(new SerialReader(in))).start();
	            //(new Thread(new SerialWriter(out))).start();
	        }
	    }
	    
	    
	    public static void main ( String[] args )
	    {
	        try
	        {
	            (new TwoWaySerialComm()).connect("/dev/cu.usbmodem1411"); //RIGHT PORT ARMOND'S MACBOOK AIR
	            //(new TwoWaySerialComm()).connect("/dev/cu.usbmodem1411"); //RIGHT PORT ARMOND'S MACBOOK AIR
	            System.out.println("out of function");
	        }
	        catch ( Exception e )
	        {
	            // TODO Auto-generated catch block
	            e.printStackTrace();
	        }
	        System.out.println("out of main almost");
	       
	    }
}
