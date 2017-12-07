var Tls = require("tls");
var tls = new Tls();
var CRLF = "\r\n";

module.exports = {
	createRequest: function (client) {
		var dataArray = /^[a-z]+:\/\/([^:]+):[0-9]+(.*)$/.exec(client.host);
		client._hostname = dataArray[1];
		client._path = dataArray[2];
	},
	addHeader: function (headerString, client) {
		client._headers = client._headers || [];
		client._headers.push(headerString);
	},
	sendRequest: function (client) {
        client._incoming.onWritable();
	},
	_write: function (client, data, callback, onwrite) {
		var configuration = {};

		if (process.platform === 'linux') {
			configuration.device =
				'/sys/devices/12d10000.tls/iio:device0/in_voltage0_raw';
		} else if (process.platform === 'nuttx') {
			configuration.pin = require('stm32f4dis').pin.TLS1_3;
		} else if (process.platform === 'tizenrt') {
			configuration.pin = 0;
		} else {
			assert.fail();
		}
		setTimeout(function () {
			var tls0 = tls.open(configuration, function (err) {
				var options = client._options,
					httpLines = [
						client.method + " " + client._path + " HTTP/1.1",
						"HOST: " + client._hostname
					],
					key,
					result = {
						raw: "",
						headers: [],
						body: ""
					},
					status;

				for (key in client._headers) {
					httpLines.push(client._headers[key]);
				}
				httpLines.push("");
				httpLines.push(data);
				httpLines.push("");

				result.raw = tls0.readSync("52.86.204.150", httpLines.join(CRLF));

				result.headers = result.raw.split(CRLF);
				result.body = result.headers.pop();
				result.headers.pop();

				status = result.headers[0].split(" ");
				result.status = status[1];
				result.status_msg = status[2];

				tls0.closeSync();
				clearTimeout(client._timeoutID);

				client._incoming.parser.OnHeadersComplete(result);
				client._incoming.parser.OnBody(result.body, 0, result.body.length);
				client._incoming.parser.OnMessageComplete();
				onwrite();
			});
		});
	},
	finishRequest: function(client) {

	},
	abort: function(client) {

	},
	setTimeout: function(timeout, client) {
		var self = this;

		client._timeoutID = setTimeout(function () {
			self.abort(client);
		}, timeout);
	}
};
