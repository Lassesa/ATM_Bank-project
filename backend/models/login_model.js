const db = require('../database'); 

const login = {
  // Funktio hakee kortin tiedot tietokannasta kortin numeron perusteella
  checkPin: function(card_number, callback) {
    return db.query(
      'SELECT card_pin FROM card WHERE card_number = ?', 
      [card_number], 
      callback
    );
  }
};

module.exports = login;