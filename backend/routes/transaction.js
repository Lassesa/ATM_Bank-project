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

        // DEBUG: Tulostetaan bäckärin konsoliin molemmat arvot
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
    const { id_account, amount } = request.body;
    const user = request.user;

    card.getById(user.card_number, function(err, cardResult) {
        if (err || !cardResult || cardResult.length === 0) {
            return response.status(403).json({ error: "Korttia ei löytynyt." });
        }

        const authorizedAccountId = cardResult[0].card_account;
        const cardType = cardResult[0].card_type;

        if (user.role !== 'admin' && Number(authorizedAccountId) !== Number(id_account)) {
            return response.status(403).json({ error: "Voit nostaa rahaa vain omalta tililtäsi." });
        }

        if (user.role !== 'admin' && cardType !== 'credit') {
            return response.status(403).json({ error: "Korttisi ei salli credit-nostoja." });
        }

        transactionHandler.creditWithdrawal({ id_account, amount }, function(err, dbResult) {
            if (err) response.status(500).json(err);
            else {
                const result = dbResult[0][0];
                if (result.result === 'Success') {
                    response.json({ message: "Credit-nosto onnistui!" });
                } else {
                    response.status(400).json({ message: "Credit-nosto epäonnistui: Luottoraja ylittyi." });
                }
            }
        });
    });
});


/**
 * 4. TILISIIRTO (TRANSFER) - PUHELINNUMEROTUKI
 */
router.post('/transfer', function(request, response) {
    // Otetaan vastaan joko target_id (tili) tai phonenumber (puhelin)
    const { source_id, target_id, phonenumber, amount } = request.body;
    const user = request.user;

    card.getById(user.card_number, function(err, cardResult) {
        if (err || !cardResult || cardResult.length === 0) {
            return response.status(403).json({ error: "Korttia ei löytynyt." });
        }

        const authorizedAccountId = cardResult[0].card_account;

        if (user.role !== 'admin' && Number(authorizedAccountId) !== Number(source_id)) {
            return response.status(403).json({ error: "Voit siirtää rahaa vain omalta tililtäsi." });
        }

//PUHELINNUMERO HAKU
if (!target_id && phonenumber) {
    const db = require('../database');
    
   
    const query = `
        SELECT account.idaccount 
        FROM account 
        JOIN customer ON account.account_customerid = customer.idcustomer 
        WHERE customer.phonenumber = ?`;
    
    db.query(query, [phonenumber], function(err, result) {
        if (err || result.length === 0) {
            return response.status(404).json({ message: "Siirto epäonnistui: Puhelinnumeroa ei löytynyt tai tiliä ei ole." });
        }
        
      
        const real_target_id = result[0].idaccount; 
        console.log("Siirretään tilille ID:", real_target_id); // Hyvä debuggausta varten
        executeActualTransfer(source_id, real_target_id, amount, response);
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
    const { id_account, amount } = request.body;
    const user = request.user;

    atmWithdrawal.atmWithdrawal({ id_account, amount }, function(err, result) {
        if (err) {
            response.status(400).json({ message: err.message });
        } else {
            response.json({ message: 'ATM-nosto onnistui' });
        }
    });
});


module.exports = router;
