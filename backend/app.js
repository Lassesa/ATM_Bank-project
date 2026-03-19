
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');
var customersRouter=require('./routes/customer');
const accountRouter = require('./routes/account');
const cardRouter = require('./routes/card');
const transactionRouter = require('./routes/transaction');

var app = express();
const PORT = process.env.PORT || 3000;

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
app.use('/users', usersRouter);
app.use('/customer',customersRouter);
app.use('/account', accountRouter);
app.use('/card', cardRouter);
app.use('/transaction', transactionRouter);

app.listen(PORT, function(){
    console.log("Palvelin kuuntelee porttia: " + PORT);
});

module.exports = app;
