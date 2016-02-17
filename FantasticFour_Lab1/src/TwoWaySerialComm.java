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
	            final InputStream in = serialPort.getInputStream();
	            final OutputStream out = serialPort.getOutputStream();
	            
	            
	            //byte b1 = '1';
	            //char b2 ='1';
	            //Scanner sc = new Scanner(System.in);
	            
	            //out.write(b1);
	            //out.flush();
	            //out.write(b2);
	            //out.flush();
	            //System.out.println("output: " + (b1-48));
	            
	            short x = 0;
	            byte upper = (byte) (x >> 8); //Get the upper 8 bits
	            byte lower = (byte) (x & 0xFF); //Get the lower 8bits
	            out.write(upper);
	            //out.flush();
	            out.write(lower);
	            out.flush();
	            if(in.available()>0){System.out.println("read: " + in.read());} //if there is data available read it
	           ///if(c==-1){System.out.println("-1!!!");}
	            
                    //
	            //System.out.println("out of while loop");
	            //sc.close();
	            //(new Thread(new SerialReader(in))).start();
	            //(new Thread(new SerialWriter(out))).start();
	        }
	    }
	    
	    
	    public static void main ( String[] args )
	    {
	        try
	        {
	            (new TwoWaySerialComm()).connect("/dev/cu.usbserial"); //RIGHT PORT ARMOND'S MACBOOK AIR
	           
	        }
	        catch ( Exception e )
	        {
	            // TODO Auto-generated catch block
	            e.printStackTrace();
	        }
	        System.out.println("out of main almost");
	       
	    }
}