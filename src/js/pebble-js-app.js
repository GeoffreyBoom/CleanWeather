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
      console.log(location.latitude + ", " + location.longitude);
      
      get_weather(location);
      get_forecast(location);
    },
    function locationError(err) {
      switch(err.code){
        case err.POSITION_UNAVAILABLE:
        case err.TIMEOUT:
          console.log("position unavailable or timeout");
          setTimeout(get_location, 60000);
          break;
        case err.PERMISSION_DENIED:
          console.log("position permission denied");
          var location = {
            latitude  : null,
            longitude : null,
            recent    : false
          };
          get_weather(location);
          get_forecast(location);
      }
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

function parse_forecast(j){  
  var list = j.list;
  var to_return = [
    {"dt": list[0].dt_txt, "temp": list[0].main.temp},
    {"dt": list[1].dt_txt, "temp": list[1].main.temp},
    {"dt": list[2].dt_txt, "temp": list[2].main.temp},
    {"dt": list[3].dt_txt, "temp": list[3].main.temp}
  ];
  return to_return;
}

function get_forecast(location){
  var URL = "";
  if(location.latitude!== null && location.longitude !== null){
    URL = 'http://api.openweathermap.org/data/2.5/forecast'+
      '?lat=' + location.latitude +
      '&lon=' + location.longitude +
      '&units=metric&mode=json';
  }
  else{
    console.log("long/lat was null");
    URL = 'http://api.openweathermap.org/data/2.5/forecast?q=Montreal&units=metric&mode=json'; 
  }
  var html = new XMLHttpRequest();
  
  html.onreadystatechange  = function(e){
    if (html.readyState == 4 && html.status == 200){
      var response = JSON.parse(html.responseText);
      var weather = parse_forecast(response);
      send_forecast(weather);
    }
  };
  html.open("GET", URL, true);
  html.send();
}
var numSends = 0;
function send_forecast(forecast){
  var forecastString = "";
  var forecast_array = [];
  for(var i = 0; i<4;i++){
    var date = new Date(forecast[i].dt);
    var minute = date.getMinutes();
    var hour = date.getHours();
    minute = (minute < 10 ? "0" : "") + minute;
    hour = (hour < 10 ? "0" : "") + hour;
    forecastString += ("" + hour + ":" + minute + " -> ");
    forecastString += ("" + forecast[i].temp + "°C");
    forecast_array[i] = forecastString;
    forecastString = "";
  }
  console.log("forecast" + forecastString);
  var dict = {
    "FORECAST_KEY_1": forecast_array[0],
    "FORECAST_KEY_2": forecast_array[1],
    "FORECAST_KEY_3": forecast_array[2],
    "FORECAST_KEY_4": forecast_array[3]
  };
  console.log(JSON.stringify(dict));
  Pebble.sendAppMessage(dict,
    function(e){
      console.out("Forecast send successful");
    },
    function(e){
      if(++numSends < 10){
        var time = Math.floor((Math.random() * 1000) + 1);
        setTimeout(send_forecast(forecast), time);
      }
    });
}

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
    console.log('Configuration window returned: ' + dict.WEATHER_TIME_KEY);
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