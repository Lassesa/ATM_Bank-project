const db = require('../database');

const account = {
    // Hae kaikki tilit
    getByCustomerId(customerId, callback) {
        return db.query("SELECT * FROM account WHERE account_customerid = ?", [customerId], callback);
    },
    
    getAll(callback) {
        return db.query("SELECT * FROM account", callback);
    },
    // Hae tili ID:n perusteella
    getById(id, callback) {
        return db.query("SELECT * FROM account WHERE idaccount = ?", [id], callback);
    },
    // Lisää uusi tili 
    add(newAcc, callback) {
        return db.query(
            "INSERT INTO account (account_number, account_type, account_balance, account_limit, account_currency, account_customerid) VALUES(?,?,?,?,?,?)",
            [
                newAcc.account_number,
                newAcc.account_type,
                newAcc.account_balance,
                newAcc.account_limit,
                newAcc.account_currency,
                newAcc.account_customerid
            ],
            callback
        );
    },
    // Päivitä tilin tiedot (esim. saldo tai tyyppi)
    update(id, acc, callback) {
        return db.query(
            "UPDATE account SET account_number=?, account_type=?, account_balance=?, account_limit=?, account_currency=? WHERE idaccount=?",
            [acc.account_number, acc.account_type, acc.account_balance, acc.account_limit, acc.account_currency, id],
            callback
        );
    },
    // Poista tili
    delete(id, callback) {
        return db.query("DELETE FROM account WHERE idaccount = ?", [id], callback);
    },

    getBalance: function(id, callback) {
    return db.query('select * from account where idaccount = ?', [id], callback);
}
};

module.exports = account;