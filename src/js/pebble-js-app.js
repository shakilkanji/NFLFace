//
//  pebble-js-app.js
//  NFLFace
//
//  Created by Shakil Kanji


// Create a global variable for user's preferences
var desired_team;
var twenty_four_hour_format;

// Construct URL to retrieve scores data
var url = "http://www.nfl.com/liveupdate/scores/scores.json";

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getScores(pos) {
  // Send request to nfl.com
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with scores info
      var scores = JSON.parse(responseText);

      // Find desired game
      var desired_game_string = findGame(scores);
      
      // GAME DETAILS
      // Home Team and Score
      var home = scores[desired_game_string].home.abbr;
      var home_score = scores[desired_game_string].home.score.T;

      // Away Team and Score
      var away = scores[desired_game_string].away.abbr;      
      var away_score = scores[desired_game_string].away.score.T;
      
      // Network showing the desired game
      var network = scores[desired_game_string].media.tv;
      
      // Date of game
      var month = desired_game_string.substring(4,6);
      var day = desired_game_string.substring(6,8);
      var date = month + "/" + day;
      
      // In-game info
      var quarter = scores[desired_game_string].qtr;
      var qtr_string = "Q" + quarter;

      var time_left = scores[desired_game_string].clock;
      var time_string = qtr_string + " " + time_left;
      
      var down = scores[desired_game_string].down;
      var togo = scores[desired_game_string].togo;
      var down_togo = down + "&" + togo;
      
      var yard_line = scores[desired_game_string].yl;
      var posteam = scores[desired_game_string].posteam;

      var info_string = posteam + " " + down_togo + " " + yard_line;
      
      // Assemble full strings to send
      var home_info;
      var away_info;
      var network_info;
      var date_info;
      
      // Conditions for Pregame, Ingame, Postgame
      if (desired_game_string == "BYE WEEK") {
        home_info = "";
        away_info = "";
        network_info = desired_game_string;
        date_info = "";
      }
      else if (quarter === null) {
        home_info = home;
        away_info = away;
        network_info = network;
        date_info = date;
      }
      else if (quarter === "Pregame") {
        home_info = home;
        away_info = away;
        network_info = network;
        date_info = "Pregame";
      }
      else if (quarter === "1" || quarter === "2" || quarter === "3" || quarter === "4") {
        home_info = home + " " + home_score;
        away_info = away + " " + away_score;
        network_info = info_string;
        date_info = time_string;
      }
      else if (quarter === "Halftime") {
        home_info = home + " " + home_score;
        away_info = away + " " + away_score;
        network_info = "Halftime";
        date_info = "";
      }
      else {
        home_info = home + " " + home_score;
        away_info = away + " " + away_score;
        network_info = "FINAL";
        date_info = "";
      }
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_HOME_INFO": home_info,
        "KEY_AWAY_INFO": away_info,
        "KEY_NETWORK_INFO": network_info,
        "KEY_DATE_INFO": date_info,
        "KEY_DESIRED_TEAM": desired_team,
        "KEY_TWENTY_FOUR_HOUR_FORMAT": twenty_four_hour_format
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Scores sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending scores to Pebble!");
        }
      );
    }      
  );
}

//---------------------- HELPER FUNCTIONS ------------------------------

function findGame(scores) {
  for (var i =0; i < 16; i++) {
    var game = Object.keys(scores)[i];
    var game_string = String(game);
    var home_team = scores[game_string].home.abbr;
    if (String(home_team) == desired_team) {
      return game_string;
    }
    var away_team = scores[game_string].away.abbr;
    if (String(away_team) == desired_team) {
      return game_string;
    } else {
      return "BYE WEEK";
    }
  }
}

function send_empty_dictionary() {
  var dictionary = {
    "KEY_DESIRED_TEAM": null
  };
  Pebble.sendAppMessage(dictionary,
    function(e) {
      console.log("Sent empty dictionary to prompt initial get_data");
    },
    function(e) {
      console.log("Empty dictionary not sent!");
    }
  );
}

//---------------------- EVENT LISTENERS ------------------------------

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
    send_empty_dictionary();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("Received message:" + JSON.stringify(e.payload));
    desired_team = e.payload.KEY_DESIRED_TEAM;
    getScores();
  }                     
);

// Listen for when the config page is opened
Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://shakilkanji.github.io/NFLFace/';
  console.log('Showing configuration page: ' + url);

  Pebble.openURL(url);
});

// Listen for when the config page is closed
Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('Configuration page returned: ' + JSON.stringify(configData));

  desired_team = configData.faveTeam;
  twenty_four_hour_format = configData.twentyFourHourFormat;
  getScores();
});
