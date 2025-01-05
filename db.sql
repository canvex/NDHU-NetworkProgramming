-- --------------------------------------------------------
-- 主機:                           152.70.110.2
-- 伺服器版本:                        8.0.24 - Source distribution
-- 伺服器作業系統:                      Linux
-- HeidiSQL 版本:                  12.8.0.6908
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;


-- 傾印 networkprogramming 的資料庫結構
CREATE DATABASE IF NOT EXISTS `networkprogramming` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci */ /*!80016 DEFAULT ENCRYPTION='N' */;
USE `networkprogramming`;

-- 傾印  資料表 networkprogramming.grouplist 結構
CREATE TABLE IF NOT EXISTS `grouplist` (
  `group_id` int NOT NULL AUTO_INCREMENT,
  `group_name` varchar(50) COLLATE utf8mb4_general_ci NOT NULL,
  `owner_name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`group_id`),
  UNIQUE KEY `group_name` (`group_name`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- 正在傾印表格  networkprogramming.grouplist 的資料：~5 rows (近似值)
INSERT INTO `grouplist` (`group_id`, `group_name`, `owner_name`) VALUES
	(1, '測試', 'brian'),
	(5, 'minecraft', 'kevin'),
	(6, 'hahah', 'ndhu'),
	(8, 'Hello', 'test2'),
	(9, '888', 'brian');

-- 傾印  資料表 networkprogramming.group_member 結構
CREATE TABLE IF NOT EXISTS `group_member` (
  `uid` int NOT NULL AUTO_INCREMENT,
  `group_name` varchar(50) COLLATE utf8mb4_general_ci NOT NULL,
  `username` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`uid`)
) ENGINE=InnoDB AUTO_INCREMENT=26 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- 正在傾印表格  networkprogramming.group_member 的資料：~4 rows (近似值)
INSERT INTO `group_member` (`uid`, `group_name`, `username`) VALUES
	(4, 'minecraft', 'kevin'),
	(5, 'hahah', 'ndhu'),
	(17, 'Hello', 'test2'),
	(23, 'minecraft', 'brian');

-- 傾印  資料表 networkprogramming.mailbox 結構
CREATE TABLE IF NOT EXISTS `mailbox` (
  `uid` int NOT NULL AUTO_INCREMENT,
  `message` char(50) COLLATE utf8mb4_general_ci DEFAULT NULL,
  `sender` varchar(50) COLLATE utf8mb4_general_ci DEFAULT NULL,
  `receiver` varchar(50) COLLATE utf8mb4_general_ci DEFAULT NULL,
  `datetime` datetime DEFAULT NULL,
  PRIMARY KEY (`uid`)
) ENGINE=InnoDB AUTO_INCREMENT=27 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- 正在傾印表格  networkprogramming.mailbox 的資料：~20 rows (近似值)
INSERT INTO `mailbox` (`uid`, `message`, `sender`, `receiver`, `datetime`) VALUES
	(1, 'Hello, this is Brian.', 'brian', 'ndhu', '2024-12-21 08:00:00'),
	(2, 'nice try', 'ndhu', 'kevin', '2024-12-21 09:15:00'),
	(3, 'Project update, Brian.', 'kevin', 'brian', '2024-12-21 10:30:00'),
	(4, 'Meeting at 3 PM.', 'ndhu', 'brian', '2024-12-21 11:45:00'),
	(5, 'see you~', 'kevin', 'ndhu', '2024-12-21 12:00:00'),
	(6, 'iloveyou', 'brian', 'kevin', '2024-12-21 16:04:52'),
	(7, 'jerk', 'ndhu', 'kevin', '2024-12-21 16:06:47'),
	(8, 'fuckyou', 'brian', 'kevin', '2024-12-21 16:08:14'),
	(9, 'testMSG', 'ndhu', 'kevin', '2024-12-21 16:10:06'),
	(10, 'helloNDHU', 'brian', 'ndhu', '2024-12-21 16:19:54'),
	(11, 'MoonFestival', 'brian', 'ndhu', '2024-12-21 16:22:47'),
	(12, 'hellomessage', 'brian', 'kevin', '2024-12-21 16:25:35'),
	(13, 'hellomessage this is brian', 'brian', 'kevin', '2024-12-21 16:27:44'),
	(17, 'test youtube netflix', 'brian', 'kevin', '2024-12-21 16:56:29'),
	(18, 'youtube', 'brian', 'ndhu', '2024-12-21 16:56:49'),
	(19, 'youtube youtube', 'brian', 'ndhu', '2024-12-21 16:56:55'),
	(21, 'hello', 'brian', 'ndhu', '2024-12-21 16:59:04'),
	(22, 'hey new friends', 'kevin', 'csie', '2024-12-21 17:27:17'),
	(23, 'i love you boy', 'ndhu', 'brian', '2024-12-23 13:05:10'),
	(25, 'second mail', 'ndhu', 'test2', '2024-12-23 21:37:57');

-- 傾印  資料表 networkprogramming.users 結構
CREATE TABLE IF NOT EXISTS `users` (
  `uid` int NOT NULL AUTO_INCREMENT,
  `username` varchar(50) COLLATE utf8mb4_general_ci NOT NULL,
  `password` varchar(50) COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`uid`),
  UNIQUE KEY `username` (`username`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- 正在傾印表格  networkprogramming.users 的資料：~7 rows (近似值)
INSERT INTO `users` (`uid`, `username`, `password`) VALUES
	(1, 'brian', 'bb'),
	(2, 'ndhu', '2024'),
	(3, 'kevin', 'bb889'),
	(4, 'csie', '2024'),
	(5, 'test', 'test'),
	(6, 'test2', 'test'),
	(7, 'ndhucsie', 'abcd');

/*!40103 SET TIME_ZONE=IFNULL(@OLD_TIME_ZONE, 'system') */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
