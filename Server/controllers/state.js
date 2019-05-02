var fs = require('fs');
var path = require('path');

var filePath = path.join(__dirname, '..', 'data', 'server_state.json');

var requestState = function (req, res) {
    //Get info about the object
    const {division, type, object} = req.params;

    const content = fs.readFileSync(filePath);
    const serverState = JSON.parse(content);

    const requestedState = serverState[division][type][object];

    res.status(200).send(requestedState);
};

var changeState = function (req, res) {
    //Get info about the object
    const {division, type, object} = req.params;
    const newState = req.body.state;

    const content = fs.readFileSync(filePath);
    const serverState = JSON.parse(content);

    serverState[division][type][object] = newState;

    fs.writeFile(filePath, JSON.stringify(serverState, null, 2),
        function (err) {
            if (err) {
                res.status(400);
            }
            res.status(200);
        });
    res.io
    .emit('newState', {
        state: {
            division: division,
            type: type,
            object: object,
            state: newState
        }
    });
    res.send();
};

module.exports = {
    requestState, changeState
};
