const db = require('../database');

const transactionHandler = {
    // 1. Perusnosto, käyttää proseduuria, joka tarkistaa saldon/luoton
    withdrawal: function(data, callback) {
        return db.query(
            "CALL credit_withdrawal(?, ?, ?)", 
            [data.id_account, data.amount, data.description || 'Nosto'],
            callback
        );
    },

    // 2. Tilisiirto
    transfer: function(data, callback) {
        return db.query(
            'CALL transfer_money(?, ?, ?, ?)',
            [data.source_id, data.target_id, data.amount, data.description || 'Tilisiirto'],
            callback
        );
    },

    // 3. Luoton nosto, sama proseduuri kuin perusnostossa
    creditWithdrawal: function(data, callback) {
        return db.query(
            'CALL credit_withdrawal(?, ?, ?)', 
            [data.id_account, data.amount, 'Luoton nosto'], 
            callback
        );
    },

};

module.exports = transactionHandler;