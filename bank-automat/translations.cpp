#include "translations.h"

UiTexts getTexts(const QString &lang)
{
    UiTexts t;

    if (lang == "PL") {
        t.welcomeTitle = "Witamy w S/R Banku";
        t.welcomeInstruction = "Włóż kartę, aby rozpocząć";

        t.pinTitle = "Proszę wprowadzić PIN";
        t.pinInstruction = "Proszę zasłonić klawiaturę podczas wpisywania PIN-u";

        t.mainTitle = "Wybierz transakcję";
        t.mainInstruction = "Wybierz usługę, aby kontynuować";

        t.withdrawTitle = "Wypłata gotówki";
        t.withdrawInstruction = "Wpisz kwotę i naciśnij OK";

        t.balanceTitle = "Saldo konta";
        t.balanceInstruction = "Sprawdź aktualne dostępne saldo";

        t.transferTitle = "Przelew";
        t.transferInstruction = "Wyślij pieniądze na inne konto";

        t.donationTitle = "Darowizna";
        t.donationInstruction = "Wesprzyj wybraną organizację";

        t.exitTitle = "Dziękujemy!";
        t.exitInstruction = "Pamiętaj, aby zabrać kartę";

        t.otherTitle = "Inne usługi";
        t.otherInstruction = "Dostęp do dodatkowych usług bankowych";

        t.mainChoice1 = "50 €";
        t.mainChoice2 = "100 €";
        t.mainChoice3 = "3 Inna kwota";
        t.mainChoice4 = "4 Saldo";
        t.mainChoice5 = "5 Przelew";
        t.mainChoice6 = "6 Darowizna";
        t.mainChoice7 = "7 Wyjście";
        t.mainChoice8 = "8 Więcej";

        t.msgInvalidAmount = "BŁĄD: Kwota musi być wielokrotnością 10.";
        t.msgWithdrawSuccess = "Sukces! Proszę odebrać gotówkę.";
        t.msgNetError = "Błąd połączenia z bankiem.";
        t.msgAtmError = "Błąd techniczny. Spróbuj innego bankomatu.";

        t.balanceMainAccount = "Konto główne";
        t.balanceRecentTransactions = "Ostatnie 5 transakcji";
        t.balanceOtherAccounts = "Inne konta";
        t.balanceMoreTransactions = "Więcej transakcji";

        t.donationChoice1 = "Czerwony Krzyż";
        t.donationChoice2 = "Fundacja Onkologiczna";
        t.donationChoice3 = "UNICEF";
        t.donationChoice4 = "Przeszczep włosów dla Arttu";

        t.errorTitle = "Przerwa techniczna";
        t.errorInstruction = "Prosimy skorzystać z innego bankomatu.";

        t.timeTitle = "Czy nadal jesteś tam?";
        t.timeInstruction = "Twoja sesja wkrótce wygaśnie.";

        t.accountsTitle = "Dostępne konta";
        t.accountsInstruction = "Twoje konta";

        t.transactionsTitle = "Transakcje";
        t.transactionsInstruction = "Ostatnie operacje";

        t.accountsMainTitle = "Konto główne";
        t.accountsBalanceTitle = "Saldo:";
        t.accountsAvailableTitle = "Dostępne środki:";
        t.accountsLimitTitle = "Limit debetowy:";
        t.accountsLimitUsedTitle = "Wykorzystany debet:";

        t.accountsCreditTitle = "Konto kredytowe";
        t.accountsCreditLimitTitle = "Limit kredytowy:";
        t.accountsCreditUsedTitle = "Wykorzystany kredyt:";
        t.accountsAvailableCreditTitle = "Dostępny kredyt:";

        t.msgPinCover = "Proszę zasłonić klawiaturę podczas wpisywania PIN-u";
        t.msgCardDetected = "Wykryto kartę";
        t.msgDonationSuccess = "Darowizna została wykonana!";
        t.msgDonationFailed = "Nie udało się wykonać darowizny.";
        t.msgDonationInvalidAmount = "Nieprawidłowa kwota.";
        t.msgAccountDataFailed = "Nie udało się wczytać danych konta.";

        t.transactionsNextButton = "Dalej";
        t.transactionsPreviousButton = "Wstecz";

        t.transferPhonePlaceholder = "Numer telefonu";
        t.transferAmountPlaceholder = "Wpisz kwotę";

        t.msgWrongPinRemaining = "Błędny PIN! Pozostało prób: %1";

        t.msgCardLocked = "Karta została zablokowana. Skontaktuj się z bankiem.";

        t.msgGenericError = "Błąd";

        t.msgNoAccountSelected = "Nie wybrano konta.";
        t.msgNoTransactions = "Brak wcześniejszych transakcji.";
        t.msgTransactionsLoadError = "Błąd podczas ładowania transakcji.";
        t.defaultTransactionDescription = "WYPŁATA Z BANKOMATU";
        t.debitCardLabel = "Karta Debetowa";
        t.creditCardLabel = "Karta Kredytowa";

        t.msgTransferSuccess = "Przelew został wykonany pomyślnie.";
        t.msgTransferFailed = "Nie udało się wykonać przelewu. Spróbuj ponownie.";
        t.msgTransferMissingPhone = "Wprowadź numer telefonu.";
        t.msgTransferMissingAmount = "Wprowadź kwotę.";
        t.msgTransferInvalidAmount = "Nieprawidłowa kwota przelewu.";


    }
    else if (lang == "FI") {
        // ...Finnish
        t.welcomeTitle = "Tervetuloa S/R Pankkiin";
        t.welcomeInstruction = "Aseta kortti aloittaaksesi";

        t.pinTitle = "Anna PIN-koodi";
        t.pinInstruction = "Suojaa näppäimistö PIN-koodia syöttäessäsi";

        t.mainTitle = "Valitse tapahtuma";
        t.mainInstruction = "Valitse palvelu jatkaaksesi";

        t.withdrawTitle = "Nosta käteistä";
        t.withdrawInstruction = "Syötä summa ja paina OK";

        t.balanceTitle = "Tilin saldo";
        t.balanceInstruction = "Näet tilisi tämänhetkisen saldon";

        t.transferTitle = "Tilisiirto";
        t.transferInstruction = "Lähetä rahaa toiselle tilille";

        t.donationTitle = "Lahjoitus";
        t.donationInstruction = "Tue valitsemaasi kohdetta";

        t.exitTitle = "Kiitos!";
        t.exitInstruction = "Muista ottaa korttisi";

        t.otherTitle = "Muut palvelut";
        t.otherInstruction = "Käytä muita pankkipalveluita";

        t.mainChoice1 = "50 €";
        t.mainChoice2 = "100 €";
        t.mainChoice3 = "3 Muu summa";
        t.mainChoice4 = "4 Saldo";
        t.mainChoice5 = "5 Siirto";
        t.mainChoice6 = "6 Lahjoitus";
        t.mainChoice7 = "7 Poistu";
        t.mainChoice8 = "8 Lisää";

        t.msgInvalidAmount = "VIRHE: Summan on oltava 10, 20, 50...";
        t.msgWithdrawSuccess = "Nosto onnistui! Otathan rahat.";
        t.msgNetError = "Yhteysvirhe pankkiin.";
        t.msgAtmError = "Tekninen häiriö, kokeile toista automaattia.";

        t.balanceMainAccount = "Päätili";
        t.balanceRecentTransactions = "Viimeiset 5 tapahtumaa";
        t.balanceOtherAccounts = "Muut tilit";
        t.balanceMoreTransactions = "Lisää tapahtumia";

        t.donationChoice1 = "Punainen Risti";
        t.donationChoice2 = "Syöpäsäätiö";
        t.donationChoice3 = "UNICEF";
        t.donationChoice4 = "Artulle hiussiirto";

        t.errorTitle = "Tilapäinen häiriö";
        t.errorInstruction = "Käytä toista pankkiautomaattia.";

        t.timeTitle = "Oletko vielä paikalla?";
        t.timeInstruction = "Istuntosi päättyy pian.";

        t.accountsTitle = "Saatavilla olevat tilit";
        t.accountsInstruction = "Omat tilit";

        t.transactionsTitle = "Tapahtumat";
        t.transactionsInstruction = "Viimeisimmät tapahtumat";

        t.accountsMainTitle = "Päätili";
        t.accountsBalanceTitle = "Saldo:";
        t.accountsAvailableTitle = "Käytettävissä:";
        t.accountsLimitTitle = "Tilin käyttöraja:";
        t.accountsLimitUsedTitle = "Käytetty tilinylitys:";

        t.accountsCreditTitle = "Luottotili";
        t.accountsCreditLimitTitle = "Luottoraja:";
        t.accountsCreditUsedTitle = "Käytetty luotto:";
        t.accountsAvailableCreditTitle = "Vapaa luotto:";

        t.msgPinCover = "Suojaa näppäimistö PIN-koodia syöttäessäsi";
        t.msgCardDetected = "Kortti tunnistettu";
        t.msgDonationSuccess = "Lahjoitus onnistui!";
        t.msgDonationFailed = "Lahjoitus epäonnistui.";
        t.msgDonationInvalidAmount = "Virheellinen summa.";
        t.msgAccountDataFailed = "Tilitietoja ei voitu ladata.";

        t.transactionsNextButton = "Seuraava";
        t.transactionsPreviousButton = "Edellinen";

        t.transferPhonePlaceholder = "Puhelinnumero";
        t.transferAmountPlaceholder = "Syötä summa";

        t.msgWrongPinRemaining = "Väärä PIN! Yrityksiä jäljellä: %1";

        t.msgCardLocked = "Kortti on lukittu. Ota yhteys pankkiin.";

        t.msgGenericError = "Virhe";

        t.msgNoAccountSelected = "Tiliä ei ole valittu.";
        t.msgNoTransactions = "Ei aiempia tapahtumia.";
        t.msgTransactionsLoadError = "Virhe ladattaessa tapahtumia.";
        t.defaultTransactionDescription = "AUTOMAATTINOSTO";
        t.debitCardLabel = "Debitkortti";
        t.creditCardLabel = "Luottokortti";

        t.msgTransferSuccess = "Siirto onnistui.";
        t.msgTransferFailed = "Siirto epäonnistui. Yritä uudelleen.";
        t.msgTransferMissingPhone = "Syötä puhelinnumero.";
        t.msgTransferMissingAmount = "Syötä summa.";
        t.msgTransferInvalidAmount = "Virheellinen siirtosumma.";

    }
    else {
        // ...English

        t.welcomeTitle = "Welcome to S/R Bank";
        t.welcomeInstruction = "Insert your card to begin";

        t.pinTitle = "Please enter your PIN";
        t.pinInstruction = "Please cover the keypad while entering your PIN";

        t.mainTitle = "Choose a transaction";
        t.mainInstruction = "Select a service to continue";

        t.withdrawTitle = "Withdraw Cash";
        t.withdrawInstruction = "Enter amount and press OK";

        t.balanceTitle = "Account balance";
        t.balanceInstruction = "View your current available balance";

        t.transferTitle = "Transfer Money";
        t.transferInstruction = "Send money to another account";

        t.donationTitle = "Donation";
        t.donationInstruction = "Support a cause or organization";

        t.exitTitle = "Thank you!";
        t.exitInstruction = "Please remember to take your card";

        t.otherTitle = "Other";
        t.otherInstruction = "Access more features and settings";

        t.mainChoice1 = "50 €";
        t.mainChoice2 = "100 €";
        t.mainChoice3 = "3 Other amount";
        t.mainChoice4 = "4 Balance";
        t.mainChoice5 = "5 Transfer";
        t.mainChoice6 = "6 Donation";
        t.mainChoice7 = "7 Exit";
        t.mainChoice8 = "8 More";

        t.msgInvalidAmount = "ERROR: Amount must be multiples of 10.";
        t.msgWithdrawSuccess = "Success! Please take your cash.";
        t.msgNetError = "Connection error to bank.";
        t.msgAtmError = "Technical error. Please try another ATM.";

        t.balanceMainAccount = "Main account";
        t.balanceRecentTransactions = "Last 5 transactions";
        t.balanceOtherAccounts = "Other Accounts";
        t.balanceMoreTransactions = "More Transactions";

        t.donationChoice1 = "Red Cross";
        t.donationChoice2 = "Cancer Foundation";
        t.donationChoice3 = "UNICEF";
        t.donationChoice4 = "Hair transplant for Arttu";

        t.errorTitle = "Out of Service";
        t.errorInstruction = "Please use another ATM.";

        t.timeTitle = "Are you still there?";
        t.timeInstruction = "Your session will end soon.";

        t.accountsTitle = "Available accounts";
        t.accountsInstruction = "Accounts you can use";

        t.transactionsTitle = "Transactions";
        t.transactionsInstruction = "Recent transactions";

        t.accountsMainTitle = "Main Account";
        t.accountsBalanceTitle = "Balance:";
        t.accountsAvailableTitle = "Available Funds:";
        t.accountsLimitTitle = "Overdraft Limit:";
        t.accountsLimitUsedTitle = "Used Overdraft:";

        t.accountsCreditTitle = "Credit Account";
        t.accountsCreditLimitTitle = "Credit Limit:";
        t.accountsCreditUsedTitle = "Used Credit:";
        t.accountsAvailableCreditTitle = "Available Credit:";

        t.msgPinCover = "Please cover the keypad while entering your PIN";
        t.msgCardDetected = "Card detected";
        t.msgDonationSuccess = "Donation successful!";
        t.msgDonationFailed = "Donation failed.";
        t.msgDonationInvalidAmount = "Invalid amount.";
        t.msgAccountDataFailed = "Failed to load account data.";

        t.transactionsNextButton = "Next";
        t.transactionsPreviousButton = "Previous";

        t.transferPhonePlaceholder = "Phone Number";
        t.transferAmountPlaceholder = "Enter amount";

        t.msgWrongPinRemaining = "Wrong PIN! Attempts remaining: %1";

        t.msgCardLocked = "Card has been locked. Please contact your bank.";


        t.msgGenericError = "Error";

        t.msgNoAccountSelected = "No account selected.";
        t.msgNoTransactions = "No previous transactions.";
        t.msgTransactionsLoadError = "Error loading transactions.";
        t.defaultTransactionDescription = "ATM WITHDRAW";
        t.debitCardLabel = "Debit Card";
        t.creditCardLabel = "Credit Card";

        t.msgTransferSuccess = "Transfer completed successfully.";
        t.msgTransferFailed = "Transfer failed. Please try again.";
        t.msgTransferMissingPhone = "Please enter phone number.";
        t.msgTransferMissingAmount = "Please enter amount.";
        t.msgTransferInvalidAmount = "Invalid transfer amount.";
    }

    return t;
}