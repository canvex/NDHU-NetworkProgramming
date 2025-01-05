import mysql.connector
from mysql.connector import Error
import sys
from mysqlConn import create_connection 

def create_group(group_name, owner_name):
    try:
        connection = create_connection()
        if connection is None:
            return

        cursor = connection.cursor()

        # 檢查群組是否已存在
        check_query = "SELECT COUNT(*) FROM grouplist WHERE group_name = %s"
        cursor.execute(check_query, (group_name,))
        result = cursor.fetchone()

        if result[0] > 0:
            print(f"群組 '{group_name}' 已存在！")
            return

        # 插入新群組
        insert_query = "INSERT INTO grouplist (group_name, owner_name) VALUES (%s, %s)"
        cursor.execute(insert_query, (group_name, owner_name))
        connection.commit()

        # 紀錄創建人的資訊到 group_member
        insert_member_query = "INSERT INTO group_member (group_name, username) VALUES (%s, %s)"
        cursor.execute(insert_member_query, (group_name, owner_name))
        connection.commit()

        print(f"成功創建群組 '{group_name}'，擁有者為 '{owner_name}'，並已將擁有者加入群組成員！")

    except Error as e:
        print(f"資料庫連線失敗，錯誤: {e}")

    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()


# 主程式，根據參數傳入receiver
if __name__ == "__main__":
    # print_args()
    
    owner_name = sys.argv[1]
    group_name = sys.argv[2]
    create_group(group_name, owner_name)

