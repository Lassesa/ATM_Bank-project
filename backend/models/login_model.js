const db = require('../database'); 

const login = {
checkPin: function(card_number, callback) {
    return db.query(
      // LISÄTTY card_type hakuun
      'SELECT card_pin, card_account AS idaccount, card_status, card_type, card_owner, pin_attempts FROM card WHERE card_number = ?', 
      [card_number], 
      callback
    );
  },

  // Uusi funktio, joka kasvattaa laskuria ja lukitsee kortin jos 3 yritystä täyttyy
  incrementAttempts: function(card_number, callback) {
    // 1. Kasvatetaan laskuria
    db.query('UPDATE card SET pin_attempts = pin_attempts + 1 WHERE card_number = ?', [card_number], (err, res) => {
      if (err) return callback(err);
      
      // 2. Tarkistetaan tilanne päivityksen jälkeen
      db.query('SELECT pin_attempts FROM card WHERE card_number = ?', [card_number], (err, res) => {
        if (err) return callback(err);
        const attempts = res[0].pin_attempts;
        
        // 3. Jos yrityksiä on 3 tai enemmän, lukitaan kortti
        if (attempts >= 3) {
          db.query('UPDATE card SET card_status = 0 WHERE card_number = ?', [card_number], (err) => {
             callback(err, { attempts: attempts, locked: true });
          });
        } else {
          callback(null, { attempts: attempts, locked: false });
        }
      });
    });
  },

  // Nollataan laskuri onnistuneen kirjautumisen yhteydessä
  resetAttempts: function(card_number, callback) {
    return db.query('UPDATE card SET pin_attempts = 0 WHERE card_number = ?', [card_number], callback);
  },

  lockCard: function(card_number, callback) {
    return db.query('UPDATE card SET card_status = 0 WHERE card_number = ?', [card_number], callback);
  }
};

module.exports = login;