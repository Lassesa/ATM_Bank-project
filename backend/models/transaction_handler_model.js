const db = require('../database');

const transactionHandler = {
    // 1. Perusnosto (käyttää proseduuria, joka tarkistaa saldon/luoton)
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

    // 3. Luoton nosto (sama proseduuri kuin perusnostossa)
    creditWithdrawal: function(data, callback) {
        return db.query(
            'CALL credit_withdrawal(?, ?, ?)', 
            [data.id_account, data.amount, 'Luoton nosto'], 
            callback
        );
    },
/* kommentoitu pois testin vuoks
    // 4. ATM-nosto (seteliautomatiikka + saldon vähennys proseduurilla)
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

                                // 3️⃣ Vähennä tilin saldo PROSEDUURILLA (tämä mahdollistaa credit-miinuksen)
                                // Käytetään samaa credit_withdrawal-proseduuria kuin kohdassa 1.
                                conn.query(
                                    "CALL credit_withdrawal(?, ?, 'ATM withdrawal')",
                                    [data.id_account, data.amount],
                                    function (err) {
                                        if (err) {
                                            // Jos proseduuri hylkää noston (ei katetta/luottoa), 
                                            // rollback palauttaa myös ATM-setelitilanteen.
                                            return rollback(err);
                                        }

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
    }*/
};

module.exports = transactionHandler;