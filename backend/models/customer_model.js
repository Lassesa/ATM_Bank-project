const db=require('../database');


const customer={
    getAllCustomers(callback){
        return db.query("SELECT * FROM customer", callback);
    },

    getOneCustomer(id,callback){
        return db.query("SELECT * FROM customer WHERE idcustomer = ?",[id],callback);
    },

    add(newC, callback) {
        return db.query(
        "INSERT INTO customer (idcustomer, fname, lname, phonenumber, email, citizenship1, citizenship2, politicalstatus, socialsecnum, birthdate) VALUES(?,?,?,?,?,?,?,?,?,?)",
        [
            newC.idcustomer,
            newC.fname,
            newC.lname,
            newC.phonenumber,
            newC.email,
            newC.citizenship1,
            newC.citizenship2,
            newC.politicalstatus,
            newC.socialsecnum,
            newC.birthdate
        ],
        callback);
    },

    update(id, newC, callback) {
        return db.query(
        "UPDATE customer SET fname=?, lname=?, phonenumber=?, email=?, citizenship1=?, citizenship2=?, politicalstatus=?, socialsecnum=?, birthdate=? WHERE idcustomer=?",
        [
            newC.fname,
            newC.lname,
            newC.phonenumber,
            newC.email,
            newC.citizenship1,
            newC.citizenship2,
            newC.politicalstatus,
            newC.socialsecnum,
            newC.birthdate,
            id // Tämä menee viimeisen kysymysmerkin kohdalle 
        ],
        callback);
    },

    delete(id, callback) {
    return db.query("DELETE FROM customer WHERE idcustomer = ?", [id], callback);
    }
    

    
    
}

module.exports = customer;