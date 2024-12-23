import mysql.connector
import sys
from mysql.connector import Error
from mysqlConn import create_connection

# 查詢郵件資料
def get_mail_by_receiver(receiver):
    try:
        connection = create_connection()
        if connection is None:
            return None
        
        cursor = connection.cursor(dictionary=True)
        query = "SELECT uid, sender, message, datetime FROM mailbox WHERE receiver = %s"
        
        # 執行查詢並傳入receiver參數
        cursor.execute(query, (receiver,))
        results = cursor.fetchall()

        # 檢查是否有結果
        if results:
            print("<id>\t<message>\t<sender>\t<date>")
            for row in results:
                print(f"{row['uid']}\t{row['message']}\t{row['sender']}\t{row['datetime']}")
        else:
            print("Empty!")
        
        # 關閉連接
        cursor.close()
        connection.close()

    except Error as e:
        print(f"查詢郵件時發生錯誤: {e}")

# 主程式，根據參數傳入receiver
if __name__ == "__main__":
    # receiver = input("請輸入接收者名稱: ")  # 使用者輸入接收者名稱
    get_mail_by_receiver(sys.argv[1])