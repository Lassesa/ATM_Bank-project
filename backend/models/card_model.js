const db = require('../database');
const bcrypt = require('bcryptjs');

const saltRounds = 10; // Määrittää, kuinka vahva salaus on

const card = {
    getAll(callback) {
        return db.query("SELECT * FROM card", callback);
    },
    getById(id, callback) {
        return db.query("SELECT * FROM card WHERE idcard = ?", [id], callback);
    },
    add(newCard, callback) {
        // Salataan PIN-koodi ennen tallennusta
        bcrypt.hash(newCard.card_pin, saltRounds, function(err, hashedPin) {
            if (err) return callback(err);

            return db.query(
                "INSERT INTO card (card_number, card_pin, card_type, card_status, card_owner, card_account) VALUES(?,?,?,?,?,?)",
                [
                    newCard.card_number,
                    hashedPin, // Tänne menee nyt se sotkettu versio
                    newCard.card_type,
                    newCard.card_status,
                    newCard.card_owner,
                    newCard.card_account
                ],
                callback
            );
        });
    },
    update(id, c, callback) {
        // Myös päivityksessä pitää hashata, jos PIN muuttuu
        bcrypt.hash(c.card_pin, saltRounds, function(err, hashedPin) {
            if (err) return callback(err);

            return db.query(
                "UPDATE card SET card_number=?, card_pin=?, card_type=?, card_status=? WHERE idcard=?",
                [c.card_number, hashedPin, c.card_type, c.card_status, id],
                callback
            );
        });
    },
    delete(id, callback) {
        return db.query("DELETE FROM card WHERE idcard = ?", [id], callback);
    }
};

module.exports = card;