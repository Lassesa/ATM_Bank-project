const db = require('../database');
const bcrypt = require('bcryptjs');

const saltRounds = 10; 

const card = {
    // Hakee kaikki kortit
    getAll(callback) {
        return db.query("SELECT * FROM card", callback);
    },

    // Hakee kortin numerolla TAI idllä
    
    getById(id, callback) {
        return db.query("SELECT * FROM card WHERE card_number = ? OR idcard = ?", [id, id], callback);
    },

    // Lisää uuden kortin ja hashaa pinin
    add(newCard, callback) {
        bcrypt.hash(newCard.card_pin, saltRounds, function(err, hashedPin) {
            if (err) return callback(err);

            return db.query(
                "INSERT INTO card (card_number, card_pin, card_type, card_status, card_owner, card_account) VALUES(?,?,?,?,?,?)",
                [
                    newCard.card_number,
                    hashedPin, 
                    newCard.card_type,
                    newCard.card_status,
                    newCard.card_owner,
                    newCard.card_account
                ],
                callback
            );
        });
    },

    // Päivittää kortin tiedot ja hashaa uuden pinin
    update(id, c, callback) {
        bcrypt.hash(c.card_pin, saltRounds, function(err, hashedPin) {
            if (err) return callback(err);

            return db.query(
                "UPDATE card SET card_number=?, card_pin=?, card_type=?, card_status=?, card_owner=?, card_account=? WHERE idcard=?",
                [c.card_number, hashedPin, c.card_type, c.card_status, c.card_owner, c.card_account, id],
                callback
            );
        });
    },

    // Poistaa kortin idn perusteella
    delete(id, callback) {
        return db.query("DELETE FROM card WHERE idcard = ?", [id], callback);
    }
};

module.exports = card;