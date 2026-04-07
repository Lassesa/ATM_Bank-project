const db = require('../database'); 

const login = {
  // Funktio hakee nyt sekä PIN-koodin että siihen liitetyn tilin ID:n
  checkPin: function(card_number, callback) {
    return db.query(
      // Lisätään card_account hakuun ja annetaan sille alias 'idaccount'
      'SELECT card_pin, card_account AS idaccount FROM card WHERE card_number = ?', 
      [card_number], 
      callback
    );
  }
};

module.exports = login;