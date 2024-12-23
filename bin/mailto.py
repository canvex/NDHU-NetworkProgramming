import mysql.connector
import sys
from mysql.connector import Error
from mysqlConn import create_connection

# 儲存郵件到資料庫的函數
def save_mail_to_db(sender, receiver, message):
    try:
        connection = create_connection()
        if connection is None:
            return

        cursor = connection.cursor()

        # 檢查接收者是否存在
        query_check_receiver = "SELECT 1 FROM users WHERE username = %s"
        cursor.execute(query_check_receiver, (receiver,))
        receiver_exists = cursor.fetchone()

        if not receiver_exists:
            print("User not found!")
            return

        # 插入郵件資料
        query_insert_mail = "INSERT INTO mailbox (sender, receiver, message, datetime) VALUES (%s, %s, %s, NOW())"
        cursor.execute(query_insert_mail, (sender, receiver, message))
        connection.commit()

        # print(f"成功儲存郵件到資料庫！")
        print("Send Accept!")
    except Error as e:
        print(f"儲存郵件到資料庫失敗: {e}")
    finally:
        if connection:
            connection.close()

# 印出所有的命令列參數
def print_args():
    print("在PYTHON中接收到的命令列參數:")
    for i, arg in enumerate(sys.argv):
        print(f"args[{i}]: {arg}")

# 主程式，根據參數傳入receiver
if __name__ == "__main__":
    # print_args()

    message=""
    sender_name = sys.argv[1]
    receiver_name = sys.argv[2]
    for i in range(3,len(sys.argv)):
        message+=f"{sys.argv[i]} "

    # 將郵件資料儲存到資料庫
    save_mail_to_db(sender_name, receiver_name, message)

    