var fs = require('fs');
var path = require('path');

var filePath = path.join(__dirname, '..', 'data', 'server_state.json');

var requestState = function (req, res) {
    //Get info about the object
    const {division, type, object} = req.params;

    const content = fs.readFileSync(filePath);
    const serverState = JSON.parse(content);

    const requestedState = serverState[division][type][object];

    res.io
    .emit('arduinoState', {
        state: {
            division: division,
            type: type,
            object: object,
            state: requestedState
        }
    });

    res.status(200).send(requestedState);
};

var requestStateAllEqual = function (req, res) {
    //Get info about the object
    const {division, type} = req.params;

    const content = fs.readFileSync(filePath);
    const serverState = JSON.parse(content);

    const objects = serverState[division][type];

    const first_value = Object.entries(objects)[0][1];
    let common_value;
    for (const key of Object.keys(objects)) {
        if (objects[key] !== first_value)
            common_value = "-1";
    }
    if(common_value !== "-1") common_value = first_value;
    res.status(200).send(common_value);
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

var changeStateAll = function (req, res) {
    //Get info about the object
    const {division, type} = req.params;
    const newState = req.body.state;

    const content = fs.readFileSync(filePath);
    const serverState = JSON.parse(content);

    const objects = serverState[division][type];
    for (const key of Object.keys(objects)) {
        objects[key] = newState;
    }

    fs.writeFile(filePath, JSON.stringify(serverState, null, 2),
      function (err) {
          if (err) {
              res.status(400);
          }
          res.status(200);
      });

    for (const key of Object.keys(objects)) {
        res.io
        .emit('newState', {
            state: {
                division: division,
                type: type,
                object: key,
                state: newState
            }
        });
    }
    res.send();
};

module.exports = {
    requestState, changeState, requestStateAllEqual, changeStateAll,
};
