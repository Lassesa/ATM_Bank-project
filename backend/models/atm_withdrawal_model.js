
const db = require('../database');

/**
 * ATM withdrawal: vähentää sekä setelit että tilin saldon
 */
function atmWithdrawal(data, callback) {
    const ATM_ID = 1;

    db.getConnection(function (err, conn) {
        if (err) return callback(err);

        conn.beginTransaction(function (err) {
            if (err) {
                conn.release();
                return callback(err);
            }

            // Lukitaan ATM ja haetaan setelit
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

                    // Jaetaan setelit isoimmasta pienimpään
                    take(200, 'atm200');
                    take(100, 'atm100');
                    take(50,  'atm50');
                    take(20,  'atm20');
                    take(10,  'atm10');

                    if (remaining !== 0) {
                        return rollback({ message: 'Automaatissa ei ole sopivia seteleitä' });
                    }

                    // Päivitetään ATM-setelit
                    conn.query(
                        `UPDATE atm SET
                         atm10 = ?, atm20 = ?, atm50 = ?, atm100 = ?, atm200 = ?
                         WHERE idatm = ?`,
                        [
                            atm.atm10,
                            atm.atm20,
                            atm.atm50,
                            atm.atm100,
                            atm.atm200,
                            ATM_ID
                        ],
                        function (err) {
                            if (err) return rollback(err);

                            // 3. Vähennetään tilin saldo
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

module.exports = {
    atmWithdrawal
};
