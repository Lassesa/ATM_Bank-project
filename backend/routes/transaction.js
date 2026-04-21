const express = require('express');
const router = express.Router();
const transaction = require('../models/transaction_model');
const transactionHandler = require('../models/transaction_handler_model');
const card = require('../models/card_model');
const atmWithdrawal = require('../models/atm_withdrawal_model');

/**
 * 1. HAE TILIIN LIITTYVÄT TAPAHTUMAT
 */
router.get('/:id', function(request, response) {
    const requestedAccountId = request.params.id;
    const user = request.user;

    card.getById(user.card_number, function(err, cardResult) {
        if (err || !cardResult || cardResult.length === 0) {
            return response.status(403).json({ error: "Korttia ei löytynyt." });
        }

        const authorizedAccountId = cardResult[0].card_account;

        // Käytetään Number-muunnosta, jotta vertailu ei kaadu tyyppieroihin
        if (user.role === 'admin' || Number(authorizedAccountId) === Number(requestedAccountId)) {
            transaction.getByAccountId(requestedAccountId, function(err, result) {
                if (err) response.status(500).json(err);
                else response.json(result);
            });
        } else {
            response.status(403).json({ error: "Ei oikeutta tämän tilin tapahtumiin." });
        }
    });
});

/**
 * 2. RAHAN NOSTO (DEBIT)
 */
router.post('/withdrawal', function(request, response) {
    const { id_account, amount, description } = request.body;
    const user = request.user;



    card.getById(user.card_number, function(err, cardResult) {
        if (err || !cardResult || cardResult.length === 0) {
            return response.status(403).json({ error: "Korttia ei löytynyt." });
        }

        const authorizedAccountId = cardResult[0].card_account;

        //Tulostetaan bäckärin konsoliin molemmat arvot
        console.log("NOSTO-VERTAILU -> Kortin tili:", authorizedAccountId, "Pyydetty tili:", id_account);

        // Pakotetaan molemmat numeroiksi Number()-funktiolla
        if (user.role !== 'admin' && Number(authorizedAccountId) !== Number(id_account)) {
            return response.status(403).json({ 
                error: "Voit nostaa rahaa vain omalta tililtäsi.",
                debug: { requested: id_account, authorized: authorizedAccountId }
            });
        }

        transactionHandler.withdrawal({ id_account, amount, description }, function(err, dbResult) {
            if (err) response.status(500).json(err);
            else {
                const result = dbResult[0][0];
                if (result.result === 'Success') {
                    response.json({ message: "Nosto onnistui!" });
                } else {
                    response.status(400).json({ message: "Nosto epäonnistui: Kate ei riitä." });
                }
            }
        });
    });
});

/**
 * 3. RAHAN NOSTO (CREDIT)
 */
router.post('/credit-withdrawal', function(request, response) {
    // Luetaan idaccount (Qt lähettää tämän)
    const { idaccount, amount } = request.body;
    const user = request.user;

    transactionHandler.creditWithdrawal({ 
        id_account: idaccount, 
        amount: amount 
    }, function(err, dbResult) {
        if (err) return response.status(500).json(err);
        
        if (dbResult && dbResult[0] && dbResult[0][0]) {
            const result = dbResult[0][0];
            if (result.result === 'Success') {
                response.json({ message: "Credit-nosto onnistui!" });
            } else {
                response.status(400).json({ message: "Credit-nosto epäonnistui: " + result.result });
            }
        } else {
            response.status(500).json({ error: "Tietokantavirhe" });
        }
    });
});


/**
 * 4. TILISIIRTO (TRANSFER) - PUHELINNUMEROTUKI
 */
router.post('/transfer', function(request, response) {
    const { source_id, target_id, phonenumber, amount } = request.body;
    const user = request.user; // Tokenista saatu käyttäjä

    const db = require('../database');

    // 1. Haetaan KAIKKI tilit, jotka kuuluvat tämän kortin omistajalle
    const findOwnerAccounts = `
        SELECT idaccount FROM account 
        WHERE account_customerid = (
            SELECT account_customerid FROM account WHERE idaccount = (
                SELECT card_account FROM card WHERE card_number = ?
            )
        )`;

    db.query(findOwnerAccounts, [user.card_number], function(err, results) {
        if (err || results.length === 0) {
            return response.status(403).json({ error: "Korttia tai tiliä ei löytynyt." });
        }

        // Tehdään lista sallituista ID numeroista
        const allowedIds = results.map(row => row.idaccount);

        // 2. TARKISTUS, onko Qtn lähettämä source_id sallittujen listalla?
        if (user.role !== 'admin' && !allowedIds.includes(Number(source_id))) {
            console.log("ESTETTY: Käyttäjän sallitut tilit:", allowedIds, "Yritetty tili:", source_id);
            return response.status(403).json({ error: "Voit siirtää rahaa vain omalta tililtäsi." });
        }

        // 3. JOS OK, JATKETAAN SIIRTOON
        if (!target_id && phonenumber) {
            // Haetaan vastaanottaja puhelinnumerolla
            const phoneQuery = `
                SELECT a.idaccount FROM account a
                JOIN customer c ON a.account_customerid = c.idcustomer 
                WHERE c.phonenumber = ?`;
            
            db.query(phoneQuery, [phonenumber], function(err, result) {
                if (err || result.length === 0) {
                    return response.status(404).json({ message: "Puhelinnumeroa ei löytynyt." });
                }
                executeActualTransfer(source_id, result[0].idaccount, amount, response);
            });
        } else {
            executeActualTransfer(source_id, target_id, amount, response);
        }
    });
});


function executeActualTransfer(source_id, target_id, amount, response) {
    transactionHandler.transfer({ source_id, target_id, amount, description: 'transfer' }, function(err, dbResult) {
        if (err) {
            console.log("Tietokantavirhe:", err);
            return response.status(500).json(err);
        }

      
        if (dbResult && dbResult[0] && dbResult[0][0]) {
            const result = dbResult[0][0];
            if (result.result === 'Success') {
                return response.json({ message: "Siirto onnistui!" });
            } else {
                return response.status(400).json({ message: "Siirto epäonnistui: " + result.result });
            }
        } else {
           
            console.log("Proseduuri ei palauttanut tulosta. Tarkista MySQL-proseduurin loppu (SELECT 'Success' as result).");
            return response.status(500).json({ error: "Tietokanta ei vastannut odotetusti." });
        }
    });
}

router.post('/atm-withdrawal', function(request, response) {
    //Otetaan vastaan idaccount
    const { idaccount, amount } = request.body;
    
    console.log("Backend vastaanotti noston:", { idaccount, amount });

    // Välitetään data mallille
    atmWithdrawal.atmWithdrawal({ idaccount, amount }, function(err, result) {
        if (err) {
            return response.status(400).json({ message: err.message });
        }
        response.json({ message: 'Success', affectedRows: 1 });
    });
});

router.get('/me/:id', function(request, response) {
    const id = request.params.id;
    
    // Tässä kutsutaan transaction_modelin funktiota, joka hakee tapahtumat idllä
    transaction.getByAccountId(id, function(err, dbResult) {
        if (err) {
            response.status(500).json(err);
        } else {
            response.json(dbResult);
        }
    });
});

router.post('/trade-kajcoin', function(request, response) {
    // 1. Luetaan data bodysta
    const { id_account, euro_change, kaj_change } = request.body;
    
    // DEBUG-tulostus bäkkerin konsoliin:
    console.log("KajCoin-kutsu vastaanotettu:", { id_account, euro_change, kaj_change });

    if (!id_account) {
        console.log("VIRHE: id_account puuttuu pyynnöstä!");
        return response.status(400).json({ error: "id_account missing" });
    }

    // 2. Kutsutaan handleria
    transactionHandler.tradeKajCoin({ id_account, euro_change, kaj_change }, function(err, dbResult) {
        if (err) {
            console.log("Tietokantavirhe KajCoin-kaupassa:", err);
            response.status(500).json(err);
        } else {
            console.log("Tietokanta vastasi:", dbResult);
            // Tarkistetaan palauttiko proseduuri Success-viestin
            if (dbResult && dbResult[0] && dbResult[0][0]) {
                response.json(dbResult[0][0]);
            } else {
                response.json({ message: "Success" }); 
            }
        }
    });
});


module.exports = router;
