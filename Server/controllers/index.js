/**
 * Get Request Object and return a string with the request fields
 * @param {Object} request
 */
function getRequestFields(req, res) {
  return `${req._startTime.toUTCString()} ${req.method} ${req.hostname}${req.url} ${res.statusCode}`;
}

module.exports = { getRequestFields };
