const express = require('express');
const path = require('path');
const cookieParser = require('cookie-parser');
const logger = require('morgan');
const jwt = require('jsonwebtoken');
const dotenv = require('dotenv');


dotenv.config();

// Tuodaan reitit
const indexRouter = require('./routes/index');
const customerRouter = require('./routes/customer');
const cardRouter = require('./routes/card');
const accountRouter = require('./routes/account');
const transactionRouter = require('./routes/transaction');
const loginRouter = require('./routes/login');
const donationRouter = require('./routes/donation');

const app = express();


app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

//JULKISET REITIT (Ei vaadi tokenia)
app.use('/login', loginRouter);
app.use('/card', cardRouter);

// Kaikki tämän alapuolella olevat reitit vaativat tokenin
app.use(authenticateToken);

function authenticateToken(req, res, next) {
  const authHeader = req.headers['authorization'];
  const token = authHeader && authHeader.split(' ')[1];

  if (token == null) return res.sendStatus(401);

  jwt.verify(token, process.env.MY_TOKEN, (err, user) => {
    if (err) return res.sendStatus(403);

    // Tallennetaan käyttäjän tiedot (card_number, role) pyyntöön
    req.user = user; 
    next();
  });
}

// SUOJATUT REITIT 
app.use('/', indexRouter);
app.use('/customer', customerRouter);
app.use('/account', accountRouter);
app.use('/transaction', transactionRouter);
app.use('/donation', donationRouter);


//PALVELIMEN KÄYNNISTYS

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {

  console.log(`Palvelin kuuntelee porttia: ${PORT}`);

});

module.exports = app;