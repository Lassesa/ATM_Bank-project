const express = require('express');
const router = express.Router();
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const login = require('../models/login_model');
const account = require('../models/account_model'); // TARVITAAN TÄMÄ

router.post('/', function(request, response) {
  const { card_number, card_pin } = request.body;

  if (card_number && card_pin) {
    login.checkPin(card_number, function(dbError, dbResult) {
      if (dbError) return response.status(500).json(dbError);

      if (dbResult.length > 0) {
        const dbCard = dbResult[0];

        if (dbCard.card_status === 0) {
          return response.status(403).send("Kortti on lukittu.");
        }

        bcrypt.compare(card_pin, dbCard.card_pin, function(err, compareResult) {
          if (compareResult) {
            // PIN OIKEIN
            login.resetAttempts(card_number, (err) => {
              
              const cardType = dbCard.card_type || 'debit';
              const token = jwt.sign(
                { card_number: card_number, role: cardType === 'admin' ? 'admin' : 'user' }, 
                process.env.MY_TOKEN, { expiresIn: '1h' }
              );

              // HAETAAN TILIT, jotta Qt saa ID:t (activeAccountId ei jää nollaksi)
              account.getByCustomerId(dbCard.card_owner, (err, accounts) => {
                if (err || !accounts || accounts.length === 0) {
                  // Jos haku epäonnistuu, palautetaan edes korttiin suoraan kytketty tili
                  response.json({ 
                    token: token, 
                    card_type: cardType, 
                    idaccount: dbCard.idaccount 
                  });
                } else if (cardType === 'dual' || cardType === 'admin') {
                  // DUAL-KORTTI: Lähetetään molemmat ID:t
                  // accounts[0] = debit, accounts[1] = credit (olettaen että ne on tässä järjestyksessä)
                  response.json({
                    token: token,
                    card_type: cardType,
                    id_debit: accounts[0] ? accounts[0].idaccount : dbCard.idaccount,
                    id_credit: accounts[1] ? accounts[1].idaccount : null
                  });
                } else {
                  // TAVALLINEN KORTTI
                  response.json({ 
                    token: token, 
                    card_type: cardType, 
                    idaccount: accounts[0].idaccount 
                  });
                }
              });
            });

          } else {
            // PIN VÄÄRIN
            login.incrementAttempts(card_number, function(err, result) {
              if (err) return response.status(500).json(err);
              if (result.locked) {
                response.status(403).json({ message: "Kortti lukittiin!", attempts: result.attempts });
              } else {
                response.status(401).json({ message: "Väärä PIN", remaining: 3 - result.attempts });
              }
            });
          }
        });
      } else {
        response.status(404).send("Korttia ei löytynyt.");
      }
    });
  } else {
    response.status(400).send("Tietoja puuttuu.");
  }
});

module.exports = router;