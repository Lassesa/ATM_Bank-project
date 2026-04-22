-- MySQL dump 10.13  Distrib 8.0.44, for Win64 (x86_64)
--
-- Host: 127.0.0.1    Database: bank
-- ------------------------------------------------------
-- Server version	8.2.0

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `account`
--

DROP TABLE IF EXISTS `account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `account` (
  `idaccount` int NOT NULL AUTO_INCREMENT,
  `account_number` varchar(45) DEFAULT NULL,
  `account_type` varchar(20) DEFAULT NULL,
  `account_balance` decimal(10,2) DEFAULT NULL,
  `account_limit` decimal(10,2) DEFAULT NULL,
  `account_currency` varchar(20) DEFAULT NULL,
  `account_opened` datetime DEFAULT NULL,
  `account_closed` datetime DEFAULT NULL,
  `account_status` tinyint DEFAULT NULL,
  `account_customerid` int DEFAULT NULL,
  `kajcoin_balance` decimal(15,2) DEFAULT '0.00',
  PRIMARY KEY (`idaccount`),
  KEY `account_customer_idx` (`account_customerid`),
  CONSTRAINT `account_customer` FOREIGN KEY (`account_customerid`) REFERENCES `customer` (`idcustomer`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8mb3;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `account`
--

LOCK TABLES `account` WRITE;
/*!40000 ALTER TABLE `account` DISABLE KEYS */;
INSERT INTO `account` VALUES (2,'FI12345678901234','Debit',-500.00,500.00,'EUR',NULL,NULL,NULL,1,0.00),(3,'FI12345678901234','Debit',1000.50,0.00,'EUR',NULL,NULL,NULL,NULL,0.00),(4,'FI12345678901234','Debit',783.50,0.00,'EUR',NULL,NULL,NULL,1,0.00),(5,'FI12345678901234','Debit',1000.50,0.00,'EUR',NULL,NULL,NULL,1,0.00),(6,'admin','Debit',12741602.69,0.00,'EUR',NULL,NULL,NULL,4,0.36),(7,'123456','dual',-9987.00,10000.00,'EUR',NULL,NULL,NULL,5,0.00),(8,'123456','Debit',1024024.00,10000.00,'EUR',NULL,NULL,NULL,7,0.00),(9,'666','Debit',80025.00,1000.00,'EUR',NULL,NULL,NULL,8,2.89),(10,'555','Debit',54745.00,1000.00,'EUR',NULL,NULL,NULL,9,0.00),(11,'8545','Debit',6665452.00,1000.00,'EUR',NULL,NULL,NULL,10,0.00),(12,'CR-777','Credit',-800.00,5000.00,'EUR',NULL,NULL,NULL,7,0.00),(13,'CR-888','Credit',-10070.00,5000.00,'EUR',NULL,NULL,NULL,8,0.00),(14,'ADMIN-CREDIT','Credit',-106394.00,1000000.00,'EUR',NULL,NULL,NULL,4,0.00);
/*!40000 ALTER TABLE `account` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-04-22 10:49:34
