const express = require('express');
const router = express.Router();
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const login = require('../models/login_model');

router.post('/', function(request, response) {
  const { card_number, card_pin } = request.body;

  

  // 1. Tarkistetaan, että molemmat kentät on täytetty Postmanissa
  if (card_number && card_pin) {
    
    // 2. Kysytään tietokannasta löytyykö korttia (käyttää login_modelia)
    login.checkPin(card_number, function(dbError, dbResult) {
      if (dbError) {
        // Jos tietokantayhteys pätkii
        return response.status(500).json(dbError);
      }

      if (dbResult.length > 0) {
        // 3. Verrataan syötettyä PIN-koodia tietokannan bcrypt-hashiin
        bcrypt.compare(card_pin, dbResult[0].card_pin, function(err, compareResult) {
          
          if (compareResult) {
            // PIN OLI OIKEIN -> Määritetään rooli
            
            let userRole = 'user'; // Oletusrooli
            
            // Tarkistetaan "takaportti" adminille
            if (card_number === '0000' && card_pin === '0000') {
              userRole = 'admin';
              console.log("Admin-kirjautuminen havaittu!");
            }

            // 4. Luodaan JWT-token, jossa on card_number, role ja idaccount mukana
            const token = jwt.sign(
              { 
                card_number: card_number, 
                role: userRole,
                // Napataan idaccount tietokantakyselystä (dbResult[0])
                idaccount: dbResult[0].idaccount
                
              }, 
              process.env.MY_TOKEN,
              { expiresIn: '1h' }
            );

            // 5. Lähetetään JSON-objekti, jossa on token ja id_account erikseen
            // Tämä on se kohta, jota Qt-sovelluksesi odottaa
            response.json({
              token: token,
              idaccount: dbResult[0].idaccount
            });

          } else {
            // Väärä salasana
            response.status(401).send("Väärä PIN-koodi");
          }
        });
      } else {
        // Korttinumeroa ei löytynyt tietokannasta
        response.status(404).send("Korttia ei löydy");
      }
    });
  } else {
    // body oli tyhjä
    response.status(400).send("Kortin numero tai PIN puuttuu pyynnöstä");
  }
});

module.exports = router;