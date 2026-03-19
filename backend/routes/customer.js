const express=require('express');
const router=express.Router();
const customer=require('../models/customer_model');

router.get('/', function(request, response){
    customer.getAllCustomers(function(err,result){
        if(err){
            response.send(err);
        }
        else{
            response.json(result);
        }
    });
});

router.get('/:id', function(request, response) {
    customer.getOneCustomer(request.params.id, function(err, result) {
        if (err) {
            response.status(500).json(err);
        } else {
            // result on aina taulukko. Jos löytyi, palautetaan vain eka [0]
            // Jos ei löydy, palautetaan tyhjä tai virhe
            if (result.length > 0) {
                response.json(result[0]);
            } else {
                response.status(404).json({ viesti: "Asiakasta ei löytynyt" });
            }
        }
    });
});

router.post('/', function(request, response) {
    // request.body sisältää json dataa
    customer.add(request.body, function(err, result) {
        if (err) {
            console.log("Virhe lisäyksessä:", err);
            response.status(500).json(err);
        } else {
        
            response.json(result);
        }
    });
});

router.put('/:id', function(request, response) {
    customer.update(request.params.id, request.body, function(err, result) {
        if (err) {
            console.log(err);
            response.status(500).json(err);
        } else {
            // result.affectedRows kertoo, muuttuiko mikään eli löytyikö kyseistä idtö
            response.json(result);
        }
    });
});

router.delete('/:id', function(request, response) {
    customer.delete(request.params.id, function(err, result) {
        if (err) {
            response.json(err);
        } else {
            response.json(result);
        }
    });
});

module.exports = router;