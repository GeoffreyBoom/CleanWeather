
Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');
  get_location();
});

Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received!');
  get_location();
});




function send_city(city){
  var dict = {"WEATHER_CITY_KEY": city};
  Pebble.sendAppMessage(dict,
    function(e){
      console.out("send successful");
    },
    function(e){
      var time = Math.floor((Math.random() * 1000) + 1);
      setTimeout(send_city(city), time);
    });
}

function send_temperature(temperature){
  var dict = {"WEATHER_TEMPERATURE_KEY": temperature + "°C"};
  Pebble.sendAppMessage(dict,
    function(e){
      console.out("send successful");
    },
    function(e){
      var time = Math.floor((Math.random() * 1000) + 1);
      setTimeout(send_temperature(temperature),time);
    }
  );
}

function send_condition(condition){
  var dict = {"WEATHER_CONDITION_KEY": condition};
  Pebble.sendAppMessage(dict,
    function(e){
      console.out("send successful");
    },
    function(e){
      var time = Math.floor((Math.random() * 1000) + 1);
      setTimeout(send_condition(condition), time);
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
      console.log(location.latitude);
      console.log(location.longitude);
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
    if (html.readyState == 4 && html.status == 200) {
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
  };
  html.open("GET", URL, true);
  html.send();
}

function send_weather(weather){
  send_city(weather.city);
  send_condition(weather.condition);
  send_temperature(weather.temperature);
}