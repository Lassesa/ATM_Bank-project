const db = require('../database');

const transactionHandler = {
    // withdrawal / donation
    withdrawal: function(data, callback) {
        return db.query(
            "CALL credit_withdrawal(?, ?, ?)",
            [data.id_account, data.amount, data.description],
            callback
        );
    },

    // transfer
    transfer: function(data, callback) {
        return db.query(
            'CALL credit_transfer(?, ?, ?)',
            [data.source_id, data.target_id, data.amount],
            callback
        );
    },

    // credit withdrawal
    creditWithdrawal: function(data, callback) {
        return db.query(
            'CALL credit_withdrawal(?, ?)',
            [data.id_account, data.amount],
            callback
        );
    }
};

module.exports = transactionHandler;