const express = require('express');
const router = express.Router();
const customer = require('../models/customer_model');
const card = require('../models/card_model');

// HAE KAIKKI (admin)
router.get('/', function(request, response) {
    if (request.user.role !== 'admin') {
        return response.status(403).json("Vain admin voi listata kaikki.");
    }
    customer.getAllCustomers((err, result) => {
        if (err) response.status(500).json(err);
        else response.json(result);
    });
});

// HAE YKSI (oma tai admin)
router.get('/:id', function(request, response) {
    const requestedId = request.params.id;
    const user = request.user;

    if (user.role === 'admin') {
        customer.getOneCustomer(requestedId, (err, result) => {
            if (err) response.status(500).json(err);
            else response.json(result[0]);
        });
    } else {
        // kuka omistaa kortin, jolla on kirjauduttu
        card.getById(user.card_number, function(err, cardResult) {
            if (err || !cardResult || cardResult.length === 0) {
                console.log("VIRHE: Korttia ei löydy DB:stä:", user.card_number);
                return response.status(403).json("Korttitietoja ei löytynyt.");
            }

            const realOwnerId = cardResult[0].card_owner;
            console.log(`VERTAILU: Omistaja ${realOwnerId} vs Haettu ${requestedId}`);

            if (realOwnerId.toString() === requestedId.toString()) {
                customer.getOneCustomer(requestedId, (err, result) => {
                    if (err) response.status(500).json(err);
                    else response.json(result[0]);
                });
            } else {
                response.status(403).json("Forbidden: Et voi katsoa muiden tietoja.");
            }
        });
    }
});

// LISÄÄ (admin)
router.post('/', (req, res) => {
    if (req.user.role !== 'admin') return res.status(403).json("Vain admin.");
    customer.add(req.body, (err, result) => {
        if (err) res.status(500).json(err);
        else res.json(result);
    });
});

// PÄIVITÄ (admin)
router.put('/:id', (req, res) => {
    if (req.user.role !== 'admin') return res.status(403).json("Vain admin.");
    customer.update(req.params.id, req.body, (err, result) => {
        if (err) res.status(500).json(err);
        else res.json(result);
    });
});

// POISTA (admin)
router.delete('/:id', (req, res) => {
    if (req.user.role !== 'admin') return res.status(403).json("Vain admin.");
    customer.delete(req.params.id, (err, result) => {
        if (err) res.status(500).json(err);
        else res.json(result);
    });
});

module.exports = router;