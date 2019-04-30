const express = require('express');

const router = express.Router();
const stateController = require('../controllers').state;

router.get('/:division/:type/:object', stateController.requestState);

router.post('/:division/:type/:object', stateController.changeState);

/* GET home page. */
router.get('/', (req, res) => {
    res.render('index', {title: 'Home Control Server'});
});

module.exports = router;
