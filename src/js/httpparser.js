function HTTPParser(incoming) {
	this.incoming = incoming;
}

HTTPParser.prototype = {
	OnHeaders: function (headers, url) {
		var parser = this;
		parser.incoming.addHeaders(headers);
	},
	OnHeadersComplete: function (info) {
		var parser = this;
		var headers = info.headers;

		if (!headers) {
			headers = parser._headers;
			parser.incoming.addHeaders(headers);
			parser._headers = {};
		} else {
			parser.incoming.addHeaders(headers);
		}

		// add header fields of headers to incoming.headers
		parser.incoming.addHeaders(headers);
		parser.incoming.statusCode = info.status;
		parser.incoming.statusMessage = info.status_msg;
		parser.incoming.started = true;

		// For client side, if response to 'HEAD' request, we will skip parsing body
		if (parser.incoming.statusCode == 100) {
			return false;
		}
		return parser.incoming.clientRequest.headersComplete();
	},
	OnBody: function (buf, start, len) {
		var parser = this;
		var incoming = parser.incoming;

		if (!incoming) {
			return;
		}

		// Push body part into incoming stream, which will emit 'data' event
		var body = buf.slice(start, start + len);
		incoming.push(body);
	},
	OnMessageComplete: function () {
		var parser = this;
		var incoming = parser.incoming;

		if (incoming) {
			if (incoming.statusCode == 100) {
				incoming.headers = {};
				incoming.statusCode = null;
				incoming.statusMessage = null;
				incoming.started = false;
			} else {
				incoming.completed = true;
				// no more data from incoming, stream will emit 'end' event
				incoming.push(null);
			}
		}
	}
};

module.exports = {HTTPParser: HTTPParser};
