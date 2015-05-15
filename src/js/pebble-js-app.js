var CONFIG_WEBSITE = 'http://geoffreyboom.github.io/WeatherNeat';

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');
  get_location();
});

Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received!');
  get_location();
});


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
      var location = {
        latitude  : null,
        longitude : null,
        recent    : false
      };
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
    console.log("long/lat was null");
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

var numSends = 0;
function send_weather(weather){
  console.log("sending weather");
  var dict = {"WEATHER_CITY_KEY": weather.city,
              "WEATHER_CONDITION_KEY": weather.condition,
              "WEATHER_TEMPERATURE_KEY": weather.temperature + "°C",
              "WEATHER_REQUEST_KEY": 1
  };
  Pebble.sendAppMessage(dict,
    function(e){
      console.out("send successful");
    },
    function(e){
      if(++numSends < 10){
        var time = Math.floor((Math.random() * 1000) + 1);
        setTimeout(send_weather(weather), time);
      }
    });
}

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL(CONFIG_WEBSITE);
});

Pebble.addEventListener('webviewclosed',
  function(e) {
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log(decodeURIComponent(e.response));
    console.log(configuration.light_on);
    console.log(configuration.weather_interval);
    var dict={
      "LIGHT_TIME_KEY": parseInt(configuration.light_on),
      "WEATHER_TIME_KEY": parseInt(configuration.weather_interval)
    };
    console.log('Configuration window returned: ' + dict.LIGHT_TIME_KEY);
    console.log('Configuration window returned: ' + dict.WEATHER_INTERVAL);
    send_configuration(dict);

  }
);

function send_configuration(dict){
  Pebble.sendAppMessage(dict,
    function(e){
      console.out("send successful");
    },
    function(e){
      var time = Math.floor((Math.random() * 1000) + 1);
      setTimeout(send_configuration(dict), time);
    }
  );
}


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