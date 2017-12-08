var fs = require('fs');

module.exports.loadJSON = function (path) {
  var result = null,
    fileData = null;

  try {
    fileData = fs.readFileSync(path);
    result = JSON.parse(fileData);
  } catch (err) {
    // supress exception, just log
    console.error(err + ' ' + path);
  }

  return result;
};

module.exports.typeCast = function (param) {
  if (param === 'true' || param === 'false') {
    return param === 'true';
  }

  if (!isNaN(parseFloat(param))) {
    return parseFloat(param);
  }

  return param;
};

module.exports.genID = function(num) {
  var a = 'abcdefghijklmnoprstuwxyz',
    l = a.length,
    ret = '';

  while (--num >= 0) {
    ret += a[(Math.random() * l) | 0];
  }
  return ret;
};
