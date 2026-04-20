const db = require('../database');

function findCombination(amount, atm) {
    for (let n200 = Math.min(Math.floor(amount / 200), atm.atm200 || 0); n200 >= 0; n200--) {
        for (let n100 = Math.min(Math.floor((amount - n200 * 200) / 100), atm.atm100 || 0); n100 >= 0; n100--) {
            for (let n50 = Math.min(Math.floor((amount - n200 * 200 - n100 * 100) / 50), atm.atm50 || 0); n50 >= 0; n50--) {
                for (let n20 = Math.min(Math.floor((amount - n200 * 200 - n100 * 100 - n50 * 50) / 20), atm.atm20 || 0); n20 >= 0; n20--) {
                    const used = n200 * 200 + n100 * 100 + n50 * 50 + n20 * 20;
                    const remaining = amount - used;

                    if (remaining < 0) continue;
                    if (remaining % 10 !== 0) continue;

                    const n10 = remaining / 10;

                    if (n10 <= (atm.atm10 || 0)) {
                        return {
                            atm200: n200,
                            atm100: n100,
                            atm50: n50,
                            atm20: n20,
                            atm10: n10
                        };
                    }
                }
            }
        }
    }

    return null;
}

function atmWithdrawal(data, callback) {
    const ATM_ID = 1;

    const accountId = Number(data.idaccount);
    const amount = Number(data.amount);

    console.log(`--- Aloitetaan nosto: Tili ${accountId}, Summa ${amount} ---`);

    if (!accountId || isNaN(accountId) || isNaN(amount) || amount <= 0) {
        return callback({ message: "Virheellinen tili-ID tai summa (NaN)" });
    }

    db.getConnection(function (err, conn) {
        if (err) return callback(err);

        conn.beginTransaction(function (err) {
            if (err) {
                conn.release();
                return callback(err);
            }

            conn.query('SELECT * FROM atm WHERE idatm = ? FOR UPDATE', [ATM_ID], function (err, rows) {
                if (err || !rows || rows.length === 0) {
                    return rollback(conn, { message: 'ATM-tietoja ei löytynyt' }, callback);
                }

                const atm = rows[0];

                const combination = findCombination(amount, atm);

                if (!combination) {
                    return rollback(conn, { message: 'Automaatissa ei ole sopivia seteleitä' }, callback);
                }

                atm.atm200 -= combination.atm200;
                atm.atm100 -= combination.atm100;
                atm.atm50 -= combination.atm50;
                atm.atm20 -= combination.atm20;
                atm.atm10 -= combination.atm10;

                console.log("Chosen combination:", combination);

                conn.query(
                    'UPDATE atm SET atm200=?, atm100=?, atm50=?, atm20=?, atm10=? WHERE idatm=?',
                    [atm.atm200, atm.atm100, atm.atm50, atm.atm20, atm.atm10, ATM_ID],
                    function (err) {
                        if (err) return rollback(conn, err, callback);

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
                                            callback(null, {
                                                affectedRows: 1,
                                                combination
                                            });
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