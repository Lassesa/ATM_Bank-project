const express = require('express');
const router = express.Router();
const transaction = require('../models/transaction_model');
const transactionHandler = require('../models/transaction_handler_model');
const card = require('../models/card_model');

/**
 * 1. HAE TILIIN LIITTYVÄT TAPAHTUMAT
 */
router.get('/:id', function(request, response) {
    const requestedAccountId = request.params.id;
    const user = request.user;

    card.getById(user.card_number, function(err, cardResult) {
        if (err || !cardResult || cardResult.length === 0) {
            return response.status(403).json({ error: "Korttia ei löytynyt." });
        }

        const authorizedAccountId = cardResult[0].card_account;

        if (user.role === 'admin' || authorizedAccountId.toString() === requestedAccountId.toString()) {
            transaction.getByAccountId(requestedAccountId, function(err, result) {
                if (err) response.status(500).json(err);
                else response.json(result);
            });
        } else {
            response.status(403).json({ error: "Ei oikeutta tämän tilin tapahtumiin." });
        }
    });
});

/**
 * 2. RAHAN NOSTO (DEBIT)
 */
router.post('/withdrawal', function(request, response) {
    const { id_account, amount } = request.body;
    const user = request.user;

    card.getById(user.card_number, function(err, cardResult) {
        if (err || !cardResult || cardResult.length === 0) return response.sendStatus(403);

        const authorizedAccountId = cardResult[0].card_account;

        if (user.role !== 'admin' && authorizedAccountId.toString() !== id_account.toString()) {
            return response.status(403).json({ error: "Voit nostaa rahaa vain omalta tililtäsi." });
        }

        transactionHandler.withdrawal({ id_account, amount }, function(err, dbResult) {
            if (err) response.status(500).json(err);
            else {
                const result = dbResult[0][0];
                if (result.result === 'Success') response.json({ message: "Nosto onnistui!" });
                else response.status(400).json({ message: "Nosto epäonnistui: Kate ei riitä." });
            }
        });
    });
});

/**
 * 3. RAHAN NOSTO (CREDIT) - UUSI!
 */
router.post('/credit-withdrawal', function(request, response) {
    const { id_account, amount } = request.body;
    const user = request.user;

    card.getById(user.card_number, function(err, cardResult) {
        if (err || !cardResult || cardResult.length === 0) return response.sendStatus(403);

        const authorizedAccountId = cardResult[0].card_account;
        const cardType = cardResult[0].card_type; // 'debit' tai 'credit'

        // Tarkistetaan oikeus tiliin
        if (user.role !== 'admin' && authorizedAccountId.toString() !== id_account.toString()) {
            return response.status(403).json({ error: "Voit nostaa rahaa vain omalta tililtäsi." });
        }

        // Tarkistetaan, että kortti on tyyppiä credit
        if (user.role !== 'admin' && cardType !== 'credit') {
            return response.status(403).json({ error: "Korttisi ei salli credit-nostoja." });
        }

        transactionHandler.creditWithdrawal({ id_account, amount }, function(err, dbResult) {
            if (err) response.status(500).json(err);
            else {
                const result = dbResult[0][0];
                if (result.result === 'Success') response.json({ message: "Credit-nosto onnistui!" });
                else response.status(400).json({ message: "Credit-nosto epäonnistui: Luottoraja ylittyi." });
            }
        });
    });
});

/**
 * 4. TILISIIRTO (TRANSFER)
 */
router.post('/transfer', function(request, response) {
    const { source_id, target_id, amount } = request.body;
    const user = request.user;

    card.getById(user.card_number, function(err, cardResult) {
        if (err || !cardResult || cardResult.length === 0) return response.sendStatus(403);

        const authorizedAccountId = cardResult[0].card_account;

        if (user.role !== 'admin' && authorizedAccountId.toString() !== source_id.toString()) {
            return response.status(403).json({ error: "Voit siirtää rahaa vain omalta tililtäsi." });
        }

        transactionHandler.transfer({ source_id, target_id, amount }, function(err, dbResult) {
            if (err) response.status(500).json(err);
            else {
                const result = dbResult[0][0];
                if (result.result === 'Success') response.json({ message: "Siirto onnistui!" });
                else response.status(400).json({ message: "Siirto epäonnistui: " + result.result });
            }
        });
    });
});

module.exports = router;