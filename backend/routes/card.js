const express = require('express');
const router = express.Router();
const card = require('../models/card_model');

// 1. Hae kaikki kortit (admin)
router.get('/', function(request, response) {
    if (request.user.role !== 'admin') {
        return response.status(403).json({ viesti: "Vain järjestelmänvalvoja voi listata kortit." });
    }
    card.getAll(function(err, result) {
        if (err) response.status(500).json(err);
        else response.json(result);
    });
});

// 2. Hae kortti ID:llä (Vain oma kortti tai admin)
router.get('/:id', function(request, response) {
    card.getById(request.params.id, function(err, result) {
        if (err) return response.status(500).json(err);
        if (result.length === 0) return response.status(404).json("Korttia ei löydy");

        // result[0].card_number on tietokannasta tuleva kortin numero
        if (request.user.role === 'admin' || request.user.card_number == result[0].card_number) {
            response.json(result[0]);
        } else {
            response.status(403).json("Ei oikeutta tämän kortin tietoihin.");
        }
    });
});

// 3. LISÄTÄÄN UUSI KORTTI
router.post('/', function(request, response) {
    // Tarkistetaan ensin, onko käyttäjä admin
    if (request.user.role !== 'admin') {
        return response.status(403).json({ viesti: "Vain admin voi luoda uusia kortteja." });
    }

    // Jos on admin, käytetään modelia lisäykseen
    card.add(request.body, function(err, result) {
        if (err) {
            response.status(500).json(err);
        } else {
            response.json(result);
        }
    });
});

// 4. Päivitä kortti
router.put('/:id', function(request, response) {
    if (request.user.role !== 'admin') {
        return response.status(403).json("Vain admin voi muokata kortteja.");
    }
    card.update(request.params.id, request.body, function(err, result) {
        if (err) response.status(500).json(err);
        else response.json(result);
    });
});

// 5. Poista kortti
router.delete('/:id', function(request, response) {
    if (request.user.role !== 'admin') {
        return response.status(403).json("Vain admin voi poistaa kortteja.");
    }
    card.delete(request.params.id, function(err, result) {
        if (err) response.status(500).json(err);
        else response.json(result);
    });
});

module.exports = router;