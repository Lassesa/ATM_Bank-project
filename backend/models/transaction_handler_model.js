const db = require('../database');

const transactionHandler = {
    // 1. Perusnosto
    withdrawal: function(data, callback) {
        return db.query(
            "CALL credit_withdrawal(?, ?, ?)", 
            [data.id_account, data.amount, data.description],
            callback
        );
    },

    // 2. Tilisiirto
    transfer: function(data, callback) {
        return db.query(
            'CALL transfer_money(?, ?, ?, ?)',
            [data.source_id, data.target_id, data.amount, data.description],
            callback
        );
    },

    // 3. Luoton nosto
    creditWithdrawal: function(data, callback) {
        return db.query(
            'CALL credit_withdrawal(?, ?)', 
            [data.id_account, data.amount], 
            callback
        );
    },

    // 4. ATM-nosto (seteliautomatiikka)
    atmWithdrawal: function (data, callback) {
        const ATM_ID = 1;

        db.getConnection(function (err, conn) {
            if (err) return callback(err);

            conn.beginTransaction(function (err) {
                if (err) {
                    conn.release();
                    return callback(err);
                }

                // 1️⃣ Lukitse ATM ja hae setelit
                conn.query(
                    `SELECT atm10, atm20, atm50, atm100, atm200
                     FROM atm
                     WHERE idatm = ?
                     FOR UPDATE`,
                    [ATM_ID],
                    function (err, rows) {
                        if (err) return rollback(err);

                        const atm = rows[0];
                        let remaining = data.amount;

                        const take = (value, field) => {
                            const count = Math.min(
                                Math.floor(remaining / value),
                                atm[field]
                            );
                            remaining -= count * value;
                            atm[field] -= count;
                        };

                        take(200, 'atm200');
                        take(100, 'atm100');
                        take(50,  'atm50');
                        take(20,  'atm20');
                        take(10,  'atm10');

                        if (remaining !== 0) {
                            return rollback({ message: 'ATM ei voi antaa tätä summaa seteleillä' });
                        }

                        // 2️⃣ Päivitä ATM-setelit
                        conn.query(
                            `UPDATE atm SET
                             atm10 = ?, atm20 = ?, atm50 = ?, atm100 = ?, atm200 = ?
                             WHERE idatm = ?`,
                            [atm.atm10, atm.atm20, atm.atm50, atm.atm100, atm.atm200, ATM_ID],
                            function (err) {
                                if (err) return rollback(err);

                                // 3️⃣ Vähennä tilin saldo
                                conn.query(
                                    `UPDATE account
                                     SET account_balance = account_balance - ?
                                     WHERE idaccount = ?`,
                                    [data.amount, data.id_account],
                                    function (err) {
                                        if (err) return rollback(err);

                                        conn.commit(function (err) {
                                            if (err) return rollback(err);
                                            conn.release();
                                            callback(null, { result: 'Success' });
                                        });
                                    }
                                );
                            }
                        );
                    }
                );

                function rollback(error) {
                    conn.rollback(function () {
                        conn.release();
                        callback(error);
                    });
                }
            });
        });
    }
};

// Viedään koko objekti, jotta kaikki metodit ovat käytettävissä
module.exports = transactionHandler;