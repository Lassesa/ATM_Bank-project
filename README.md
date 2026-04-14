# Pankkiautomaatti-projekti (Ryhmä SPO-08)

Tämä on Node.js-pohjainen **REST API**, joka toimii pankkiautomaattijärjestelmän taustapalveluna. Sovellus hallinnoi asiakastietoja, pankkikortteja, tilejä ja tilitapahtumia tarjoten turvallisen rajapinnan Qt-pohjaiselle asiakasohjelmalle.

##  Toteutetut ominaisuudet

* **Tietokanta ja CRUD:** Täydet hallintatyökalut asiakkaille, korteille ja tileille.
* **Tietoturva:**
    * Korttien PIN-koodit tallennetaan tietokantaan suojattuina **bcrypt**-algoritmilla.
    * Suojatut reitit vaativat voimassa olevan **JWT (JSON Web Token)** -tunnisteen.
* **Autentikaatio:** Järjestelmä käyttää korttinumeroa ja PIN-koodia kirjautumiseen.
* **Pankkilogiikka:**
    * Rahansiirrot, kuten nostot ja lahjoitukset, on toteutettu tietokantatason proseduureilla (Stored Procedures).
    * Hyödyntää transaktioita ja katesuojaa: tili ei mene luvatta miinukselle.
* **Erityisominaisuudet:**
    * **Kortin lukitus:** Rajapinta lukitsee kortin automaattisesti kolmen virheellisen PIN-syötön jälkeen.
    * **Monikielisyys ja esteettömyys:** Tuki kielen vaihdolle (FI/EN/PL) ja korkean kontrastin tilalle.
    * **Lahjoitukset:** Mahdollisuus tehdä lahjoituksia hyväntekeväisyyteen noston yhteydessä.

##  Tekninen rakenne (MVC-malli)

Sovelluksen arkkitehtuuri on jaettu loogisiin kerroksiin ylläpidettävyyden ja tietoturvan varmistamiseksi:

* **Routes:** Reitit (esim. `account.js`, `transaction.js`) ottavat vastaan HTTP-pyynnöt.
* **Models:** Mallit (esim. `card_model.js`) sisältävät SQL-kyselyt ja kommunikoivat MySQL-tietokannan kanssa.
* **Middleware:** `authenticateToken.js` tarkistaa jokaisen pyynnön otsikosta JWT-tokenin oikeellisuuden.

 ## Tietokannan ER-malli
<img width="786" height="1193" alt="bank_model" src="https://github.com/user-attachments/assets/4139010b-c342-4a0d-ac71-67a00c607e52" />
