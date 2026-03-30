const db = require('../database');

const transaction = {
    // Haetaan tietyn tilin tapahtumat (uusin ensin)
    getByAccountId: function(id, callback) {
        return db.query(
            "SELECT * FROM transaction WHERE transaction_source = ? ORDER BY transaction_date DESC", 
            [id], 
            callback
        );
    },

    // Adminille kaikki tapahtumat
    getAll: function(callback) {
        return db.query("SELECT * FROM transaction ORDER BY transaction_date DESC", callback);
    }
};

module.exports = transaction;