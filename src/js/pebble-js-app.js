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
});

Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received!');
    var dict = {"KEY_COUNT": 100};
    Pebble.sendAppMessage(dict);
});