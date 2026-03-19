const db = require('../database');

const transaction = {
    // Haetaan tapahtumat, joissa tili on ollut lähettäjänä (source)
    getByAccountId(id, callback) {
        return db.query(
            "SELECT * FROM transaction WHERE transaction_source = ? ORDER BY transaction_date DESC", 
            [id], 
            callback
        );
    },
    
    add(newTrans, callback) {
    // 1. Lisätään tapahtuma lokiin
    return db.query(
        "INSERT INTO transaction (transaction_date, transaction_amount, transaction_source, transaction_type) VALUES(NOW(), ?, ?, ?)",
        [newTrans.transaction_amount, newTrans.transaction_source, newTrans.transaction_type],
        function(err, result) {
            if (err) {
                return callback(err);
            }
            
            // 2. Päivitetään tilin saldo account-tauluun
            // Jos transaction_amount on negatiivinen (esim. -20), se vähennetään saldosta
           
            return db.query(
            "UPDATE account SET account_balance = account_balance + ? WHERE idaccount = ? AND (account_balance + ? >= 0 OR account_type = 'Credit')",
            [newTrans.transaction_amount, newTrans.transaction_source, newTrans.transaction_amount],
            callback
            );
        }
    );
}
};

module.exports = transaction;