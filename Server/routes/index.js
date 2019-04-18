const express = require('express');

const router = express.Router();
const indexController = require('../controllers');

const logs = [];

/* GET home page. */
router.get('/', (req, res, next) => {
  logs.unshift(indexController.getRequestFields(req, res));
  res.render('index', { title: 'Home Control Server', log: logs });
});

module.exports = router;
