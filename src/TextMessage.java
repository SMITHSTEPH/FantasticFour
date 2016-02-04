import com.twilio.sdk.TwilioRestClient;
import com.twilio.sdk.TwilioRestException;
import com.twilio.sdk.resource.factory.MessageFactory;
import com.twilio.sdk.resource.instance.Message;
import org.apache.http.NameValuePair;
import org.apache.http.message.BasicNameValuePair;
 
import java.util.ArrayList;
import java.util.List;

public class TextMessage {
	// Find your Account Sid and Token at twilio.com/user/account
	  public static final String ACCOUNT_SID = "AC2d94e3ab9715a2de6a58fead2c2861fe";
	  public static final String AUTH_TOKEN = "17ae461a1db9348ab8fcfe938af86111";
	 
	  public static void main(String[] args) throws TwilioRestException {
	    TwilioRestClient client = new TwilioRestClient(ACCOUNT_SID, AUTH_TOKEN);
	    
	    String body;
	    body = "3:38";
	    String number;
	    number = "+15153714907";
	 
	    // Build a filter for the MessageList
	    List<NameValuePair> params = new ArrayList<NameValuePair>();
	    params.add(new BasicNameValuePair("Body", body));
	    params.add(new BasicNameValuePair("To", number));
	    params.add(new BasicNameValuePair("From", "+15153032132"));
	 
	    MessageFactory messageFactory = client.getAccount().getMessageFactory();
	    Message message = messageFactory.create(params);
	    System.out.println(message.getSid());
	  }
}
