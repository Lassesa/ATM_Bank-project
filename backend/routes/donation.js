const express = require('express');
const router = express.Router();
const transactionHandler = require('../models/transaction_handler_model');

router.post('/', function(req, res) {
    // Luetaan idaccount (vastaa Qt:n lähetystä)
    const { idaccount, amount, description } = req.body;

    if (!idaccount || !amount) {
        return res.status(400).json({ message: "Tiedot puuttuvat" });
    }

    // Käytetään transactionHandler.withdrawal, EI atmWithdrawal
    // Tämä kutsuu suoraan SQL-proseduuria ilman setelitarkistuksia.
    transactionHandler.withdrawal({
        id_account: idaccount,
        amount: amount,
        description: description || "Donation" //lokiin tulee oikea teksti
    }, function(err, result) {
        if (err) {
            return res.status(400).json({ message: err.message || "Lahjoitus epäonnistui" });
        }
        res.json({ message: "Success", affectedRows: 1 });
    });
});

module.exports = router;