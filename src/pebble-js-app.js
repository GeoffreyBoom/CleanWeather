// Function to send a message to the Pebble using AppMessage API
function send_weather(weather) {
  Pebble.sendAppMessage({
    "WEATHER_CITY_KEY":weather.city,
    "WEATHER_TEMPERATURE_KEY":weather.temperature
    }
  );
}
function get_location(){
  navigator.geolocation.getCurrentPosition(
    function locationSuccess(pos) {
      var location = {
        latitude:   pos.coords.latitude,
        longitude:  pos.coords.longitude,
        recent:     true
      };
      get_weather(location);
    },
    function locationError(err) {
      var location;
      location.latitude  = "null";
      location.longitude = "null";
      location.recent    = false;
      get_weather(location);
    },
    {
      enableHighAccuracy: true, 
      maximumAge: 10000, 
      timeout: 10000
    }
  );
}


function get_weather(location){

  var URL = "";
  if(location.latitude!== null && location.longitude !== null){
    URL = 'http://api.openweathermap.org/data/2.5/weather'+
      '?lat=' + location.latitude +
      '&lon=' + location.longitude +
      '&units=metric&mode=json';
  }
  else{
    URL = 'http://api.openweathermap.org/data/2.5/weather?q=Montreal&units=metric&mode=json'; 
  }

  
  var html = new XMLHttpRequest();
  html.onreadystatechange  = function(e){
    if (html.readyState == 4) {
      if(html.status == 200) {
        var response = JSON.parse(html.responseText);
        console.log(response.name);
        var weather = {
          city: response.name,
          condition: response.weather[0].description,
          temperature: Math.round(response.main.temp).toString(),
          recent:true
        };
        send_weather(weather);
      }
    }
  };
  html.open("GET", URL, true);
  html.send();
}

// Called when JS is ready
Pebble.addEventListener("ready",
  function(e) {
    get_location();
  }
);
												
// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage",
  function(e) {
    get_location();
  }
);