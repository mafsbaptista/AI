var fs = require('fs');
var path = require('path');

var filePath = path.join(__dirname, '..', 'data', 'server_state.json');

var requestState = function(req, res) {
    //Get info about the object
    const { division, type, object }= req.params;

    const content = fs.readFileSync(filePath);
    const serverState = JSON.parse(content);

    const requestedState = serverState[division][type][object];

    res.status(200).send(requestedState);
};

module.exports = {
    requestState
};