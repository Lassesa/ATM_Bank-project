# Pankkiautomaatti-projekti (Ryhmä 8)

Tämä on Node.js-pohjainen REST API, joka toimii pankkiautomaattijärjestelmän taustapalveluna. Sovellus hallinnoi asiakastietoja, pankkikortteja, tilejä ja tilitapahtumia.

## Videoesittely
[![Pankkiautomaatti-projekti esittely](https://img.youtube.com/vi/7f2bgE1Fowo/0.jpg)](https://www.youtube.com/watch?v=7f2bgE1Fowo)

## Toteutetut ominaisuudet

* **Tietokanta ja CRUD:** Täydet hallintatyökalut asiakkaille, korteille ja tileille.
* **Tietoturva:** Korttien PIN-koodit tallennetaan tietokantaan suojattuina bcrypt-algoritmilla.
* **Automaattinen korttilukitus:** Järjestelmä seuraa virheellisiä PIN-koodin syöttöyrityksiä tietokantatasolla. Kolmen peräkkäisen virheen jälkeen kortti lukittuu automaattisesti, mikä estää luvattoman käytön. Onnistunut kirjautuminen nollaa yrityslaskurin.
* **Autentikaatio:** Järjestelmä käyttää korttinumeroa ja PIN-koodia kirjautumiseen, mistä palautetaan JSON Web Token (JWT).
* **Pankkilogiikka:** Kaikki kriittiset rahaoperaatiot, kuten nostot ja tilisiirrot, on toteutettu tietokantatason proseduureilla. Tämä varmistaa transaktioiden atomisuuden, katesuojan ja tietojen eheyden.
* **Tilitapahtumien selaus:** Käyttöliittymä tarjoaa selkeän listanäkymän tilin historiasta (pvm, tapahtumatyyppi, summa). Haku on rajattu hakemaan 50 viimeisintä tapahtumaa joko Debit- tai Credit-puolelta valitun tilin mukaan.
* **Älykäs pankkiautomaattilogiikka (ATM Vault):** Järjestelmä seuraa automaatin fyysistä rahamäärää reaaliajassa omasta tietokantataulustaan. 
    * Kone tarkistaa ennen nostoa, onko automaatissa riittävästi seteleitä ja pystytäänkö summa muodostamaan olemassa olevilla nimellisarvoilla (esim. 150€ -> 1x100€ + 1x50€). 
    * Noston yhteydessä automaatin saldo päivittyy automaattisesti, ja järjestelmä estää nostot, mikäli tarvittavia seteleitä ei ole saatavilla.
* **Dual-Card -tuki:** Järjestelmä tukee yhdistelmäkortteja, joissa on sekä Debit- että Credit-ominaisuudet. Käyttäjä voi vaihtaa tilityyppiä lennosta saman istunnon aikana.
* **Puhelinnumeropohjaiset siirrot:** Perinteisten tili-ID-siirtojen lisäksi varoja voidaan siirtää vastaanottajan puhelinnumeron perusteella, mikä tekee palvelusta käyttäjäystävällisemmän.
* **Älykäs omistajuuden varmistus:** Backend tarkistaa dynaamisesti, että käyttäjällä on oikeus hallinnoida useita eri tilejä saman kortin kautta (esim. oikeus siirtää rahaa sekä Debit- että Credit-puolelta).
* **Lahjoitustoiminto:** Järjestelmään on integroitu tuki kohdistetuille lahjoituksille, jotka hyödyntävät olemassa olevaa turvallista siirtolaitosta.

## Helppokäyttöisyys ja käyttöliittymä

Käyttöliittymä on suunniteltu huomioimaan erilaiset käyttäjätarpeet ja tarjoamaan miellyttävä käyttökokemus:

* **Monikielisyys:** Sovelluksessa on tuki kolmelle kielelle: Suomi, Englanti ja Puola. Kieli on vaihdettavissa yhdellä painalluksella jokaisella sivulla.
* **Kontrastitilat:** Käyttäjä voi valita visuaalisen teeman tarpeen mukaan. Saatavilla on selkeä vaalea teema sekä silmiä säästävä tumma tila.
* **Interaktiiviset näppäinäänet:** Käyttökokemusta on parannettu äänipalautteella. Eri toiminnoille (kuten näppäinpainallukset, onnistuneet tapahtumat ja virhetilanteet) on omat tunnistettavat ääniefektinsä, mikä helpottaa laitteen käyttöä.

## Rakenne

Sovellus noudattaa MVC-mallia (Model-View-Controller):

* **Routes:** Reitit (esim. account.js, transaction.js) ottavat vastaan pyynnöt ja tarkistavat käyttäjän oikeudet.
* **Models:** Tiedostot (esim. card_model.js, transaction_handler_model.js) sisältävät SQL-rajapinnan ja kutsuvat tietokannan proseduureja.
* **Middleware:** Reittien yhteyteen toteutettu tarkistuslogiikka varmistaa, että vain kirjautuneet käyttäjät, joilla on voimassa oleva JWT-token, pääsevät tekemään tilitapahtumia.

  ## Tietokannan ER-malli
<img width="933" height="923" alt="bank_model" src="https://github.com/user-attachments/assets/b7d1e28c-a1fe-4a46-bd33-d9b873cbb60c" />


 ## Järjestelmän toimintavirta
 <img width="1173" height="745" alt="Screenshot 2026-04-21 at 16 54 27" src="https://github.com/user-attachments/assets/3e3c877e-4866-49b7-932a-f890e9e47568" />
 <img width="1166" height="726" alt="Screenshot 2026-04-21 at 16 55 21" src="https://github.com/user-attachments/assets/e46ece42-1ac4-41cb-a267-43b3765dc1db" />
 <img width="1160" height="1208" alt="Screenshot 2026-04-21 at 16 56 28" src="https://github.com/user-attachments/assets/a9a92b3b-bc3e-465a-bdb5-735b0024e0cc" />
<img width="949" height="1048" alt="Screenshot 2026-04-21 at 16 57 29" src="https://github.com/user-attachments/assets/f023b8e8-f454-4d8f-b45f-ad5bc8a4306f" />
 <img width="954" height="747" alt="Screenshot 2026-04-21 at 16 58 05" src="https://github.com/user-attachments/assets/4e76dfcc-c5f5-41bd-8179-1d6256a16880" />



