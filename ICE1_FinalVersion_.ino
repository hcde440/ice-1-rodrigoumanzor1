// Here we present a sketch that connects our ESP8266 processor to the internet via wifi. Once connected to the server, we can make HTTP requests to obtain 
//information regarding our current geographical location. One of the data components obtained from this request is the city of our location. This is then 
//used to make a different HTTP request to obtain weather information for our city. To do this though, we need API keys so that our request can be 
//appropriately verified. These keys are incorporated into the respective HTTP requests. Since the data we obtain is in JSON form, we must then parse it 
//using the ArduinoJson library so that we can visualize it in text form on our serial monitor 


#include <ESP8266WiFi.h> // Include the library for our ESP866 wifi microchip processor
#include <ESP8266HTTPClient.h> //Include library required to setup the HTTP server 
#include <ArduinoJson.h> //Include library that let's us parse and construct JSON objects 

const char* ssid = "STC2"; //Declare a constant and save our wifi name onto it (saves memory) 
const char* pass = "bahtsang";// Declare constant and save our wifi network password in it 

const char* key = "400b2e0842d1df144f1d8429c8acf93c"; //Save our personal API key for later use 
String weatherKey = "f709898e08d320ade1fda7be727be937"; //Save API needed to access weather data later 

typedef struct { //Define new data type that we will hold all of the incoming information in strings
  String ip; //Assign a name to each piece of information obtained regarding our geographical location, this saves ip value 
  String cc; // Save country abbreviation 
  String cn; // Save full country name 
  String rc; // Save state abbraviation 
  String rn; 
  String cy; // Save city information 
  String tz; // Save timezone 
  String ln; //Save longitude information 
  String lt; //Save latitude information 
} GeoData; // Assign this data structure a name for later use 

typedef struct { // Again we define a new data type that will hold our weather information 
  String tp; // Save temperature value 
  String pr; // Save precipitation information
  String hd; // Save humidity information 
  String ws; // Save windspeed information 
  String wd; // Save wind direction information 
  String cd; // Save cloud status informartion 
} MetData; // Assign this data structure a name for later use 

GeoData location; // Create an instance of the GeoData type with the location variable 
MetData conditions; // Create an instance of the MetData type with the conditions variable 

void setup() { //Call the setup function to start our sketch 
  Serial.begin(115200); //Set the desired data for serial communication with computer
  delay(10); //Pause for 10 milliseconds 

  Serial.print("Connecting to "); Serial.println(ssid); //Print this in serial monitor and lets us know that our processor is starting to connect to Wifi    

  WiFi.mode(WIFI_STA); //Initialize the actual connection to Wifi by calling Wifi.mode from the library
  WiFi.begin(ssid, pass); //Input wifi name and password 

  while (WiFi.status() != WL_CONNECTED) { // Loop to check our connection status during the connection process 
    delay(500); // Delay so that we check every 500 milliseconds 
    Serial.print("."); //Print dots until we are connected to the wifi network
  } // The loop will continue until our Wifi.status is not equal to WL_connected 

  Serial.println(); Serial.println("WiFi connected"); Serial.println(); 
  Serial.print("Your ESP has been assigned the internal IP address "); 
  Serial.println(WiFi.localIP()); // Print the IP address assigned to our ESP 

  String ipAddress = getIP(); // Function for HTTP request to get IP address and save value in a string 
  // Once HTTP payload is received, the data regarding our geographical location will be saved in the GeoData structure
  
  getGeo(ipAddress); // Function for HTTP request for geographical data with the IP address we just obtained 

  Serial.println(); // Skip a line in the serial monitor  

  Serial.println("Your external IP address is " + location.ip); // Display our external IP address by accessing ip string from location variable
  Serial.print("Your ESP is currently in " + location.cn + " (" + location.cc + "),"); // Display our current country and abbreviation in parantheses 
  Serial.println(" in or near " + location.cy + ", " + location.rc + "."); // On the same line, print our current city and state 
  Serial.print("You are in the " + location.tz + " timezone "); // Print our current time zone
  Serial.println("and located at (roughly) "); // Print text in quotes on same line as time zone
  Serial.println(location.lt + " latitude by " + location.ln + " longitude."); // Print latitude and longitude in a new line 

  getMet(location.cy); // Function for HTTP request to get weather data using the city we saved in the location variable
  // The weather data will then be saved in the conditions variable we defined earlier and the various strings saved within it can then be accessed by calling it 

  Serial.println(); // Skip a line in the serial monitor 
  Serial.println("With " + conditions.cd + ", the temperature in " + location.cy + ", " + location.rc); // Print the cloud conditions, city, and state 
  Serial.println("is " + conditions.tp + "F, with a humidity of " + conditions.hd + "%. The winds are blowing"); // Print temperature and humidity 
  Serial.println(conditions.wd + " at " + conditions.ws + " miles per hour, and the "); // Print wind and wind speed data 
  Serial.println("barometric pressure is at " + conditions.pr + " millibars."); // Print barometric pressure data 
}

void loop() {
} // Cannot place getIP() here otherwise it would ping the endpoint indefinitely 

String getIP() { // Function that has our string content 
  HTTPClient theClient; // Creating mini HTTP from the HTTP library 
  String ipAddress; // String that stores data we get back from request 

  theClient.begin("http://api.ipify.org/?format=json"); // Website address where we will be making our request 
  int httpCode = theClient.GET(); // Make the http request 

  if (httpCode > 0) {
    if (httpCode == 200) { // 200 is identified as the working code number for the request 

      DynamicJsonBuffer jsonBuffer; // jsonBuffer can be static or dynamic, if we know how long the result will be we can use a static buffer to save memory
      String payload = theClient.getString(); // String that contains the json data 
      JsonObject& root = jsonBuffer.parse(payload); // Parse the json data 
      ipAddress = root["ip"].as<String>(); 
    }
    else { // If httpCode value is anything else enter here, meaning that HTTP request failed 
      Serial.println("Something went wrong with connecting to the endpoint in getIP().");
      return "error"; // Displays that there was an error with our request 
    }
  }
  return ipAddress; // If sucessful return ipAddress 
}

void getGeo(String ipAddress) { // Initialize HTTP request for geographical data 
  
  HTTPClient theClient;
  Serial.println("Making HTTP request"); // Print this text to inform us that we are making the request 
  theClient.begin("http://api.ipstack.com/" + ipAddress + "?access_key=" + key); //return IP as .json object
  int httpCode = theClient.GET(); // Assign the HTTP code 
  if (httpCode > 0) {
    if (httpCode == 200) { // Again we are testing to see if our request is success based on the http code value by entering this loop if it is = 200
      Serial.println("Received HTTP payload."); // If successful, print this out to let us know we have the payload 
      DynamicJsonBuffer jsonBuffer; // jsonBuffer can be static or dynamic, if we know how long the result will be we can use a static buffer to save memory
      String payload = theClient.getString(); // String that contains the json data 
      Serial.println("Parsing..."); // Print this out to let us know that we will start to parse the data 
      JsonObject& root = jsonBuffer.parse(payload); // Parse the data 
      // Now we test if parsing succeeds
      if (!root.success()) { // If we don't succeed enter this loop 
        Serial.println("parseObject() failed"); // Print this to let us know we failed to parse 
        Serial.println(payload);
        return;
      }
      //Some debugging lines below:
      //      Serial.println(payload);
      //      root.printTo(Serial);
      //Using .dot syntax, we refer to the variable "location" which is of
      //type GeoData, and place our data into the data structure.
      location.ip = root["ip"].as<String>();            //we cast the values as Strings because the 'slots' in GeoData are Strings
      location.cc = root["country_code"].as<String>();  //
      location.cn = root["country_name"].as<String>();
      location.rc = root["region_code"].as<String>();
      location.rn = root["region_name"].as<String>();
      location.cy = root["city"].as<String>();
      location.lt = root["latitude"].as<String>();
      location.ln = root["longitude"].as<String>();
    } else {
      Serial.println("Something went wrong with connecting to the endpoint.");
    }
  }
}

void getMet(String city) { // HTTP request to get our weather data for the city 
  HTTPClient theClient; // Create mini http from the http library 
  String apiCall = "http://api.openweathermap.org/data/2.5/weather?q=" + city; // Address where we will be obtaining our weather data
  apiCall += "&units=imperial&appid=";
  apiCall += weatherKey;
  theClient.begin(apiCall);
  int httpCode = theClient.GET();
  if (httpCode > 0) {

    if (httpCode == HTTP_CODE_OK) {
      String payload = theClient.getString();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(payload);
      if (!root.success()) {
        Serial.println("parseObject() failed in getMet().");
        return;
      }
      conditions.tp = root["main"]["temp"].as<String>();
      conditions.pr = root["main"]["pressure"].as<String>();
      conditions.hd = root["main"]["humidity"].as<String>();
      conditions.cd = root["weather"][0]["description"].as<String>();
      conditions.ws = root["wind"]["speed"].as<String>();
      int deg = root["wind"]["deg"].as<int>();
      conditions.wd = getNSEW(deg);
    }
  }
  else {
    Serial.printf("Something went wrong with connecting to the endpoint in getMet().");
  }
}

String getNSEW(int d) {
  String direct;

  //Conversion based upon http://climate.umn.edu/snow_fence/Components/winddirectionanddegreeswithouttable3.htm
  if (d > 348.75 && d < 360 || d >= 0  && d < 11.25) {
    direct = "north"; // 
  };
  if (d > 11.25 && d < 33.75) {
    direct = "north northeast";
  };
  if (d > 33.75 && d < 56.25) {
    direct = "northeast";
  };
  if (d > 56.25 && d < 78.75) {
    direct = "east northeast";
  };
  if (d < 78.75 && d < 101.25) {
    direct = "east";
  };
  if (d < 101.25 && d < 123.75) {
    direct = "east southeast";
  };
  if (d < 123.75 && d < 146.25) {
    direct = "southeast";
  };
  if (d < 146.25 && d < 168.75) {
    direct = "south southeast";
  };
  if (d < 168.75 && d < 191.25) {
    direct = "south";
  };
  if (d < 191.25 && d < 213.75) {
    direct = "south southwest";
  };
  if (d < 213.25 && d < 236.25) {
    direct = "southwest";
  };
  if (d < 236.25 && d < 258.75) {
    direct = "west southwest";
  };
  if (d < 258.75 && d < 281.25) {
    direct = "west";
  };
  if (d < 281.25 && d < 303.75) {
    direct = "west northwest";
  };
  if (d < 303.75 && d < 326.25) {
    direct = "south southeast";
  };
  if (d < 326.25 && d < 348.75) {
    direct = "north northwest";
  };
  return direct;
}
