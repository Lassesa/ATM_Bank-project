const express = require('express');
const router = express.Router();
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const login = require('../models/login_model');

router.post('/', function(request, response) {
  const { card_number, card_pin } = request.body;

  if (card_number && card_pin) {
    login.checkPin(card_number, function(dbError, dbResult) {
      if (dbError) return response.status(500).json(dbError);

      if (dbResult.length > 0) {
        const dbCard = dbResult[0];

        // Tarkistus: Onko jo lukittu?
        if (dbCard.card_status === 0) {
          return response.status(403).send("Kortti on lukittu.");
        }

        bcrypt.compare(card_pin, dbCard.card_pin, function(err, compareResult) {
          if (compareResult) {
            // PIN OIKEIN -> Nollataan yritykset tietokannassa
            login.resetAttempts(card_number, (err) => {
              const token = jwt.sign(
                { card_number: card_number, role: 'user', idaccount: dbCard.idaccount }, 
                process.env.MY_TOKEN, { expiresIn: '1h' }
              );
              response.json({ token: token, idaccount: dbCard.idaccount });
            });

          } else {
            // PIN VÄÄRIN -> Kasvatetaan laskuria tietokannassa
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
        response.status(404).send("Korttia ei löydy");
      }
    });
  } else {
    response.status(400).send("Kortin numero tai PIN puuttuu");
  }
});

module.exports = router;