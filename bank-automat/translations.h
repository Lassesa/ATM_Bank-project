// translations.h
#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#include <QString>

struct UiTexts {
    QString welcomeTitle;
    QString welcomeInstruction;

    QString pinTitle;
    QString pinInstruction;

    QString mainTitle;
    QString mainInstruction;

    QString withdrawTitle;
    QString withdrawInstruction;

    QString balanceTitle;
    QString balanceInstruction;

    QString transferTitle;
    QString transferInstruction;

    QString donationTitle;
    QString donationInstruction;

    QString exitTitle;
    QString exitInstruction;

    QString otherTitle;
    QString otherInstruction;

    QString errorTitle;
    QString errorInstruction;

    QString timeTitle;
    QString timeInstruction;

    QString accountsTitle;
    QString accountsInstruction;

    QString transactionsTitle;
    QString transactionsInstruction;

    QString mainChoice1;
    QString mainChoice2;
    QString mainChoice3;
    QString mainChoice4;
    QString mainChoice5;
    QString mainChoice6;
    QString mainChoice7;
    QString mainChoice8;

    QString balanceMainAccount;
    QString balanceRecentTransactions;
    QString balanceOtherAccounts;
    QString balanceMoreTransactions;

    QString donationChoice1;
    QString donationChoice2;
    QString donationChoice3;
    QString donationChoice4;

    QString accountsMainTitle;
    QString accountsBalanceTitle;
    QString accountsAvailableTitle;
    QString accountsLimitTitle;
    QString accountsLimitUsedTitle;

    QString accountsCreditTitle;
    QString accountsCreditLimitTitle;
    QString accountsCreditUsedTitle;
    QString accountsAvailableCreditTitle;

    QString msgInvalidAmount;
    QString msgWithdrawSuccess;
    QString msgNetError;
    QString msgAtmError;

    QString msgPinCover;
    QString msgCardDetected;
    QString msgDonationSuccess;
    QString msgDonationFailed;
    QString msgDonationInvalidAmount;
    QString msgAccountDataFailed;

    QString transactionsNextButton;
    QString transactionsPreviousButton;

    QString transferPhonePlaceholder;
    QString transferAmountPlaceholder;

    QString msgWrongPinRemaining;

    QString msgCardLocked;

    QString msgGenericError;

    QString msgNoAccountSelected;
    QString msgNoTransactions;
    QString msgTransactionsLoadError;

    QString defaultTransactionDescription;

    QString debitCardLabel;
    QString creditCardLabel;

};

UiTexts getTexts(const QString &lang);

#endif