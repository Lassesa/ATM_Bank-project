/*const express = require('express');
const router = express.Router();
const db = require('../db');
const auth = require('../middleware/auth');

router.post('/', auth, async (req, res) => {
    const { amount } = req.body;
    const cardId = req.user.cardId;

    if (!amount) {
        return res.status(400).json({ error: "Amount missing" });
    }

    try {
        // Haetaan tilin saldo
        const [account] = await db.query(
            "SELECT balance FROM account WHERE card_id = ?",
            [cardId]
        );

        if (account.length === 0) {
            return res.status(404).json({ error: "Account not found" });
        }

        const balance = account[0].balance;

        if (balance < amount) {
            return res.status(400).json({ error: "Insufficient funds" });
        }

        // Päivitetään saldo
        await db.query(
            "UPDATE account SET balance = balance - ? WHERE card_id = ?",
            [amount, cardId]
        );

        // Lisätään transaction
        await db.query(
            "INSERT INTO transaction (card_id, type, amount) VALUES (?, 'donation', ?)",
            [cardId, amount]
        );

        res.json({ message: "Donation successful", donated: amount });

    } catch (err) {
        console.error(err);
        res.status(500).json({ error: "Server error" });
    }
});

module.exports = router;*/