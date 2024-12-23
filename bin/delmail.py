import mysql.connector
import sys
from mysql.connector import Error
from mysqlConn import create_connection

# 刪除信件的函數
def delete_mail(uid, current_user):
    try:
        connection = create_connection()
        if connection is None:
            return

        cursor = connection.cursor()

        # 檢查信件是否存在且屬於當前使用者
        query_check_mail = "SELECT 1 FROM mailbox WHERE uid = %s AND receiver = %s"
        cursor.execute(query_check_mail, (uid, current_user))
        mail_exists = cursor.fetchone()

        if not mail_exists:
            print("mail ID unexist! ")
            return

        # 刪除信件
        query_delete_mail = "DELETE FROM mailbox WHERE uid = %s AND receiver = %s"
        cursor.execute(query_delete_mail, (uid, current_user))
        connection.commit()

        print(f"信件 UID {uid} 已成功刪除！")
    except Error as e:
        print(f"刪除信件失敗: {e}")
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
    

    current_user = sys.argv[1]
    mail_uid = sys.argv[2]
    # message = sys.argv[3]

    # 執行刪除
    delete_mail(mail_uid, current_user)