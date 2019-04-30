const express = require('express');
const path = require('path');
const morgan = require('morgan');
const rfs = require('rotating-file-stream');

const indexRouter = require('./routes/index');

const app = express();
var server = require('http').createServer(app);
var io = require('socket.io')(server);

// Logger settings
const accessLogStream = rfs('access.log', {
    interval: '1d', // rotate daily
    path: path.join(__dirname, 'log'),
});

app.use(morgan('<p> :date[web] :remote-addr'
    + '<br> :method :url HTTP/:http-version :status </p>', {
    stream: accessLogStream,
    skip(req, res) {
        return res.statusCode === 304;
    },
}));

app.use(function (req, res, next) {
    res.io = io;
    next();
});

// View engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

// Takes the raw requests and turns them into usable properties on req.body
app.use(express.json());
app.use(express.urlencoded({extended: true}));

// Static file declaration
app.use(express.static(path.join(__dirname, 'public')));
app.use(express.static(path.join(__dirname, 'home-control-client/build')));

app.use('/', indexRouter);

module.exports = {app, server};
