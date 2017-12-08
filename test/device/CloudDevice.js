var https = require('https'),
  querystring = require('querystring');

var CloudDevice = function (options) {
    this.deviceId = options.deviceId;
    this.deviceToken = options.deviceToken;
    this.cloudHostName = options.cloudHostName;
  },
  proto = CloudDevice.prototype;

function request(options, data, callback) {
  console.log(options);
  var req = https.request(options, function (res) {
    var responseData = '';
    res.on('data', function (incomming) {
      responseData += incomming.toString();
    });

    res.on('end', function () {
      console.log(responseData);
      callback(null, responseData);
    });

    res.on('error', function (err) {
      callback(err);
    });
  });

    console.log("data: ", data);
    req.write(new Buffer(data || ""));


  req.end();
};

proto.getActions = function (options, callback) {
  var query = {
    startDate: options.startDate || 0,
    ddid: this.deviceId
  };

  if (options.endDate) {
    query.endDate = options.endDate;
  }

  if (options.order) {
    query.order = options.order;
  }

  if (options.count) {
    query.count = options.count;
  }
  console.log('getActions');
  request({
    hostname: this.cloudHostName,
    method: 'GET',
    path: '/v1.1/actions?' + querystring.encode(query),
    headers: {
      'Authorization': 'Bearer ' + this.deviceToken,
      'Content-Type': 'application/json',
      'Accept': 'application/json',
      'User-Agent': 'gardener/1.1'
    }
  },
  null,
  function (err, data) {
    console.log('getActions2', err, data);
    var result = [];
    if (err) {
      callback(err);
    } else if (!data) {
      callback('no data');
    } else {
      var parsed = JSON.parse(data.toString());
      if (parsed.error) {
        callback(parsed.error.message);
      } else {
        if (data.size > 0) {
          result = data.data;
        }
        callback(null, result);
      }
    }
  });
};

proto.postMessage = function (data, callback) {
  var message = JSON.stringify({
    sdid: this.deviceId,
    data: data
  });

  request({
    hostname: this.cloudHostName,
    method: 'POST',
    path: '/v1.1/messages',
    headers: {
      'Content-Length': message.length,
      'Authorization': 'Bearer ' + this.deviceToken,
      'Content-Type': 'application/json',
      'Accept': 'application/json',
      'User-Agent': 'gardener/1.1'
    }
  },
  message,
  function (err, data) {
    if (err) {
      callback(err);
    } else if (!data) {
      callback('no data');
    } else {
      var parsed = JSON.parse(data.toString());
      if (parsed.error) {
        callback(parsed.error.message);
      } else {
        callback(null, parsed);
      }
    }
  });
};

module.exports = CloudDevice;
