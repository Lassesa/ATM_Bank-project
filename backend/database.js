const db=require('mysql2');

const dotenv=require('dotenv');
dotenv.config();



const conn=db.createPool(process.env.MYSQL_SERVER);



module.exports=conn;