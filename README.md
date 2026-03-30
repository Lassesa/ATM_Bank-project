# Pankkiautomaatti-projekti (Ryhmä 8)

Tämä on Node.js-pohjainen REST API, joka toimii pankkiautomaattijärjestelmän taustapalveluna. Sovellus hallinnoi asiakastietoja, pankkikortteja, tilejä ja tilitapahtumia.

## Toteutetut ominaisuudet

* **Tietokanta ja CRUD:** Täydet hallintatyökalut asiakkaille, korteille ja tileille.
* **Tietoturva:** Korttien PIN-koodit tallennetaan tietokantaan suojattuina bcrypt-algoritmilla.
* **Autentikaatio:** Järjestelmä käyttää korttinumeroa ja PIN-koodia kirjautumiseen, mistä palautetaan JSON Web Token (JWT).
* **Pankkilogiikka:** Rahansiirrot, kuten nostot ja tilisiirrot, on toteutettu tietokantatason proseduureilla (Stored Procedures). Ne hyödyntävät transaktioita ja katesuojaa, mikä varmistaa, että tili ei mene luvatta miinukselle ja tiedot pysyvät ehyinä virhetilanteissa.

## Rakenne

Sovellus noudattaa MVC-mallia (Model-View-Controller):

* **Routes:** Reitit (esim. account.js, transaction.js) ottavat vastaan pyynnöt ja tarkistavat käyttäjän oikeudet.
* **Models:** Tiedostot (esim. card_model.js) sisältävät varsinaiset SQL-kyselyt.
* **Middleware:** authenticateToken.js tarkistaa jokaisen suojatun pyynnön yhteydessä, että mukana on voimassa oleva JWT-token.

