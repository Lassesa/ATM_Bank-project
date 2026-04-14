# AGGREGATE FUNCTIONS eli koostefunktiot
# AVG, SUM, COUNT, GROUP_CONCAT(mysql)

SELECT AVG(salary) as "Keskipalkka" FROM person;
SELECT AVG(birth_year) as "Keski-ikä" FROM person;
# kahdella desimaalilla
SELECT ROUND(AVG(salary),2) AS "Keskipalkka" FROM person;
SELECT ROUND(3.143321321321,2);

#Palkkojen keskiarvo, min ja max
SELECT AVG(salary), MIN(salary),MAX(salary) FROM person;

#GROUP BY tarvitaan jos SELECT sisältää koostefunktioita ja normi kenttiä

#Tulostetaan palkkojen keskiarvot kunnittain
SELECT city,ROUND(AVG(salary),2) FROM PERSON GROUP BY city;

#Tulostetaan henkilöiden lukumäärät kunnittain
SELECT city, count(city)
FROM person GROUP BY city

#HAVING ehtoa käytetään GROUP BY:n kanssa

#Tulostetaan palkkojen summat kunnittain
#Palkkojen summa suurempi kuin 90

SELECT city, SUM(salary)
FROM person GROUP BY city HAVING SUM(salary) > 70;

SELECT city, SUM(salary) FROM person WHERE salary < 40 GROUP BY city HAVING SUM(salary) >70;

#Minkä nimisiä henkilöitä eri kaupungeissa asuu
# siis listaa sukunimet kaupungeittain

SELECT city as Kaupunki, GROUP_CONCAT(CONCAT(firstname, " ", lastname)) as "Sukunimet kaupungeissa"
FROM person
GROUP BY city;

SELECT lastname as "sukunimi" FROM person;

# AS ei ole pakollinen, mutta kannattaa käyttää selkeyden vuoksi

SELECT city, birth_year, COUNT(*)
FROM person GROUP BY city, birth_year;

ALTER TABLE account DROP COLUMN account_ownership;



