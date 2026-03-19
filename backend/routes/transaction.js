const express = require('express');
const router = express.Router();
const transaction = require('../models/transaction_model');

// Hae tilin tapahtumat: GET localhost:3000/transaction/account/1
router.get('/account/:id', function(request, response) {
    transaction.getByAccountId(request.params.id, function(err, result) {
        if (err) {
            response.json(err);
        } else {
            response.json(result);
        }
    });
});

// Lisää uusi tapahtuma: POST localhost:3000/transaction
router.post('/', function(request, response) {
    transaction.add(request.body, function(err, result) {
        if (err) {
            response.json(err);
        } else {
            response.json(result);
        }
    });
});

module.exports = router;