import mysql.connector
from mysql.connector import Error

# 設定資料庫連接
def create_connection():
    try:
        connection = mysql.connector.connect(
            host="127.0.0.1",      # 資料庫主機
            database="networkprogramming",    # 資料庫名稱
            user="root",  # 資料庫使用者名稱
            password="0123456789",  # 資料庫密碼
            charset='utf8mb4'
        )
        if connection.is_connected():
            # print("成功連接到資料庫")
            return connection
    except Error as e:
        print(f"資料庫連接失敗: {e}")
        return None