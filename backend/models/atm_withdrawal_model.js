const db = require('../database');

function atmWithdrawal(data, callback) {
    const ATM_ID = 1;
    
    // MUUTOS: Luetaan 'idaccount' (ilman alaviivaa)
    const accountId = Number(data.idaccount);
    const amount = Number(data.amount);

    console.log(`--- Aloitetaan nosto: Tili ${accountId}, Summa ${amount} ---`);

    // Tarkistetaan ettei tullut NaN tai 0
    if (!accountId || isNaN(accountId) || isNaN(amount) || amount <= 0) {
        return callback({ message: "Virheellinen tili-ID tai summa (NaN)" });
    }

    db.getConnection(function (err, conn) {
        if (err) return callback(err);

        conn.beginTransaction(function (err) {
            if (err) { conn.release(); return callback(err); }

            // 1. Haetaan ja lukitaan ATM-setelit
            conn.query('SELECT * FROM atm WHERE idatm = ? FOR UPDATE', [ATM_ID], function (err, rows) {
                if (err || !rows || rows.length === 0) {
                    return rollback(conn, {message: 'ATM-tietoja ei löytynyt'}, callback);
                }

                const atm = rows[0];
                let remaining = amount;
                
                // Setelilaskenta (yksinkertaistettu tässä)
                const take = (value, field) => {
                    const count = Math.min(Math.floor(remaining / value), atm[field] || 0);
                    remaining -= count * value;
                    atm[field] -= count;
                };

                take(200, 'atm200'); take(100, 'atm100'); take(50, 'atm50');
                take(20, 'atm20'); take(10, 'atm10');

                if (remaining > 0) {
                    return rollback(conn, {message: 'Automaatissa ei ole sopivia seteleitä'}, callback);
                }

                // 2. Päivitetään automaatin saldo ja kutsutaan tilinostoproseduuria
                conn.query(
                    'UPDATE atm SET atm200=?, atm100=?, atm50=?, atm20=?, atm10=? WHERE idatm=?',
                    [atm.atm200, atm.atm100, atm.atm50, atm.atm20, atm.atm10, ATM_ID],
                    function (err) {
                        if (err) return rollback(conn, err, callback);

                        // Kutsutaan proseduuria (huomaa: idaccount on nyt oikea luku)
                        conn.query(
                            "CALL credit_withdrawal(?, ?, 'ATM Withdrawal')",
                            [accountId, amount],
                            function (err, procResult) {
                                if (err) return rollback(conn, err, callback);
                                
                                if (procResult && procResult[0] && procResult[0][0]) {
                                    const dbMessage = procResult[0][0].result;
                                    if (dbMessage === 'Success') {
                                        conn.commit(function (err) {
                                            if (err) return rollback(conn, err, callback);
                                            conn.release();
                                            callback(null, { affectedRows: 1 });
                                        });
                                    } else {
                                        rollback(conn, { message: dbMessage }, callback);
                                    }
                                } else {
                                    rollback(conn, { message: "Tietokantavirhe" }, callback);
                                }
                            }
                        );
                    }
                );
            });
        });
    });
}

function rollback(conn, err, callback) {
    conn.rollback(function () {
        conn.release();
        callback(err);
    });
}

module.exports = { atmWithdrawal };