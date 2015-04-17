var count = 0;

var emit = function() {
  count += 1;

  var dict = {"KEY_COUNT": count};

  Pebble.sendAppMessage(dict);
  
};

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');

  // Send periodic updates every 3 seconds
  setInterval(emit, 3000);
  send_city();
  send_weather();
  send_condition();
});

Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received!');
  var dict = {"KEY_COUNT": 100};
  Pebble.sendAppMessage(dict);
  send_city();
  send_weather();
  send_condition();
});

function send_city(){
  var dict = {"WEATHER_CITY_KEY": "kansas"};
  Pebble.sendAppMessage(dict,
    function(e){
      console.out("send successful");
    },
    function(e){
      send_city();
    });
}

function send_weather(){
  var dict = {"WEATHER_TEMPERATURE_KEY": "100C"};
  Pebble.sendAppMessage(dict,
    function(e){
      console.out("send successful");
    },
    function(e){
      send_weather();
    }
  );
}

function send_condition(){
  var dict = {"WEATHER_CONDITION_KEY": "chance of meatballs"};
  Pebble.sendAppMessage(dict,
    function(e){
      console.out("send successful");
    },
    function(e){
      send_condition();
    }
  );
}