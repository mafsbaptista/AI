const express = require('express');

const router = express.Router();
const indexController = require('../controllers');
const stateController = require('../controllers').state;

const logs = [];

/* GET home page. */
router.get('/', (req, res, next) => {
  logs.unshift(indexController.getRequestFields(req, res));
  res.render('index', { title: 'Home Control Server', log: logs });
});

router.get('/:division/:type/:object', stateController.requestState);

module.exports = router;
