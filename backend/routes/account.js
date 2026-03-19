const express = require('express');
const router = express.Router();
const account = require('../models/account_model');

// Hae kaikki tilit (GET http://localhost:3000/account)
router.get('/', function(request, response) {
    account.getAll(function(err, result) {
        if (err) {
            response.json(err);
        } else {
            response.json(result);
        }
    });
});

// Hae tietyn asiakkaan tilit (GET http://localhost:3000/account/customer/1)
router.get('/customer/:id', function(request, response) {
    account.getByCustomer(request.params.id, function(err, result) {
        if (err) {
            response.json(err);
        } else {
            response.json(result);
        }
    });
});

router.get('/:id', function(request, response) {
    account.getById(request.params.id, function(err, result) {
        if (err) {
            response.json(err);
        } else {
            // Jos tulos on tyhjä, palautetaan virheviesti
            if (result.length > 0) {
                response.json(result[0]);
            } else {
                response.status(404).json({message: "Tiliä ei löytynyt"});
            }
        }
    });
});

// Lisää uusi tili (POST http://localhost:3000/account)
router.post('/', function(request, response) {
    account.add(request.body, function(err, result) {
        if (err) {
            response.json(err);
        } else {
            response.json(result);
        }
    });
});

// Päivitä tili (PUT http://localhost:3000/account/5)
router.put('/:id', function(request, response) {
    account.update(request.params.id, request.body, function(err, result) {
        if (err) {
            response.json(err);
        } else {
            response.json(result);
        }
    });
});

// Poista tili (DELETE http://localhost:3000/account/5)
router.delete('/:id', function(request, response) {
    account.delete(request.params.id, function(err, result) {
        if (err) {
            response.json(err);
        } else {
            response.json(result);
        }
    });
});

module.exports = router;