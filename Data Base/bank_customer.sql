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
-- Table structure for table `customer`
--

DROP TABLE IF EXISTS `customer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `customer` (
  `idcustomer` int NOT NULL AUTO_INCREMENT,
  `fname` varchar(20) DEFAULT NULL,
  `lname` varchar(20) DEFAULT NULL,
  `phonenumber` varchar(20) DEFAULT NULL,
  `email` varchar(45) DEFAULT NULL,
  `citizenship1` varchar(20) DEFAULT NULL,
  `citizenship2` varchar(20) DEFAULT NULL,
  `politicalstatus` tinyint DEFAULT NULL,
  `socialsecnum` varchar(20) DEFAULT NULL,
  `birthdate` date DEFAULT NULL,
  `customercreated` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `customerclosed` datetime DEFAULT NULL,
  `customerstatus` tinyint DEFAULT '1',
  PRIMARY KEY (`idcustomer`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8mb3;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `customer`
--

LOCK TABLES `customer` WRITE;
/*!40000 ALTER TABLE `customer` DISABLE KEYS */;
INSERT INTO `customer` VALUES (1,'Ville','Vallaton','0401234567','ville.vallaton@esimerkki.fi','Suomi',NULL,0,'121298-456X','1998-12-12',NULL,NULL,NULL),(2,'arttu','Vallaton','0401234567','ville.vallaton@esimerkki.fi','Suomi',NULL,0,'121298-456X','1998-12-12',NULL,NULL,NULL),(3,'arttu','Vallaton','0401234567','ville.vallaton@esimerkki.fi','Suomi',NULL,0,'121298-456X','1998-12-12',NULL,NULL,NULL),(4,'admin','admin','0401234567','admin@admin.com','Suomi',NULL,0,'admin','1998-12-12',NULL,NULL,NULL),(5,'ara','jäbä','0401234567','ara@jäbä.com','Suomi',NULL,0,'190397-221X','1998-12-12','2026-03-23 08:17:18',NULL,1),(6,'Arttu','Tölli','0401234567','arttu.tolli@gmail.com','Suomi','Ruotsi',1,'100596-221X','1996-05-10','2026-03-30 06:08:32',NULL,1),(7,'Kajetan','Zelech-Alatarvas','0000910639','kajetan@gmail.com','Suomi','Puola',1,'100596-221X','1996-05-10','2026-04-07 07:45:07',NULL,1),(8,'Arttu','Tölli','0000412110','ara@gmail.com','Suomi','Ruotsi',1,'100596-221X','1996-05-10','2026-04-07 08:02:08',NULL,1),(9,'Nhi','Luong','0000410119','nhi@gmail.com','Suomi','Ruotsi',1,'100596-221X','1996-05-10','2026-04-07 08:07:42',NULL,1),(10,'Lasse','Sarja','0000887224','lasse@gmail.com','Suomi','Ruotsi',0,'100596-221X','1996-05-10','2026-04-07 08:08:58',NULL,1);
/*!40000 ALTER TABLE `customer` ENABLE KEYS */;
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
