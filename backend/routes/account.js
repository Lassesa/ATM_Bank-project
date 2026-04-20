const express = require('express');
const router = express.Router();
const account = require('../models/account_model');
const card = require('../models/card_model');


 // 1. HAE KAIKKI TILIT (Vain Admin)

router.get('/', function(request, response) {
    // Tarkistetaan, onko käyttäjä admin
    if (request.user.role !== 'admin') {
        return response.status(403).json({ error: "Vain admin voi hakea listan kaikista tileistä." });
    }

    account.getAll(function(err, dbResult) {
        if (err) {
            response.status(500).json(err);
        } else {
            response.json(dbResult);
        }
    });
});


 //2. HAE YKSI TILI (Oma tili tai Admin)

router.get('/:id', function(request, response) {
    const requestedAccountId = request.params.id;
    const user = request.user;

    // Admin saa hakea minkä tahansa tilin
    if (user.role === 'admin') {
        account.getById(requestedAccountId, (err, result) => {
            if (err) response.status(500).json(err);
            else response.json(result[0]);
        });
    } else {
        //  tarkistetaan kuuluuko tili hänen korttiinsa
        card.getById(user.card_number, function(err, cardResult) {
            if (err || !cardResult || cardResult.length === 0) {
                return response.status(403).json({ error: "Korttia ei löytynyt." });
            }

            const authorizedAccountId = cardResult[0].card_account;

            if (authorizedAccountId.toString() === requestedAccountId.toString()) {
                account.getById(requestedAccountId, (err, result) => {
                    if (err) response.status(500).json(err);
                    else response.json(result[0]);
                });
            } else {
                response.status(403).json({ error: "Tämä tili ei kuulu korttiisi." });
            }
        });
    }
});


 // 3. LISÄÄ UUSI TILI (Vain Admin)

router.post('/', (req, res) => {
    
    if (req.user.role !== 'admin') return res.sendStatus(403);
    
    account.add(req.body, (err, result) => {
        if (err) res.status(500).json(err);
        else res.json(result);
    });
});


 //4. PÄIVITÄ TILI (Vain Admin)

router.put('/:id', (req, res) => {
    if (req.user.role !== 'admin') return res.sendStatus(403);
    
    account.update(req.params.id, req.body, (err, result) => {
        if (err) res.status(500).json(err);
        else res.json(result);
    });
});


 //5. POISTA TILI (Vain Admin)

router.delete('/:id', (req, res) => {
    if (req.user.role !== 'admin') return res.sendStatus(403);
    
    account.delete(req.params.id, (err, result) => {
        if (err) res.status(500).json(err);
        else res.json(result);
    });
});

// HAE KIRJAUTUNEEN KORTIN SALDO
router.get('/balance/me', function(request, response) {
    const cardNumber = request.user.card_number;

    card.getById(cardNumber, function(err, cardResult) {
        if (err || !cardResult || cardResult.length === 0) {
            return response.status(404).json({ error: "Korttia ei löytynyt." });
        }

        
        const customerId = cardResult[0].card_owner; 
        
        console.log("Haetaan tilit asiakkaalle ID:", customerId);

        account.getByCustomerId(customerId, function(err, accountResults) {
            if (err || !accountResults || accountResults.length === 0) {
                return response.status(404).json({ error: "Asiakkaalla ei ole tilejä." });
            }

            // Palautetaan ensimmäinen tili
            response.json(accountResults[0]); 
        });
    });
});

router.get('/balance/me/:id', function(request, response) {
    const requestedAccountId = request.params.id;
    
    
    account.getById(requestedAccountId, function(err, dbResult) {
        if (err) {
            response.status(500).json(err);
        } else if (dbResult && dbResult.length > 0) {
            response.json(dbResult[0]);
        } else {
            response.status(404).json({ error: "Tiliä ei löytynyt" });
        }
    });
});

module.exports = router;