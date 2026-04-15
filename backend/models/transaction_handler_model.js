const db = require('../database');

const transactionHandler = {
    // nosto (withdrawal)
withdrawal: function(data, callback) {
    return db.query(
        "CALL credit_withdrawal(?, ?, ?)", 
        [data.id_account, data.amount, data.description], // Lisää description tähän
        callback
    );
},

    // tilisiirto (transfer)
transfer: function(data, callback) {
    return db.query(
        'CALL transfer_money(?, ?, ?, ?)',
        [data.source_id, data.target_id, data.amount, data.description],
        callback
    );
},
// luotto
    creditWithdrawal: function(data, callback) {
    return db.query(
        'CALL credit_withdrawal(?, ?)', 
        [data.id_account, data.amount], 
        callback
    );
}


};

module.exports = transactionHandler;