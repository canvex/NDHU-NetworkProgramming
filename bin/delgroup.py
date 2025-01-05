import mysql.connector
from mysql.connector import Error
import sys
from mysqlConn import create_connection


def delete_group(username, group_name):
    try:
        # 建立資料庫連線
        connection = create_connection()
        if connection is None:
            return

        cursor = connection.cursor()

        # 確認群組是否存在
        check_group_query = "SELECT COUNT(*) FROM grouplist WHERE group_name = %s"
        cursor.execute(check_group_query, (group_name,))
        group_result = cursor.fetchone()

        if group_result[0] == 0:
            print("Group not found!")
            return

        # 確認使用者是否為群組擁有者
        check_owner_query = "SELECT owner_name FROM grouplist WHERE group_name = %s"
        cursor.execute(check_owner_query, (group_name,))
        result = cursor.fetchone()

        owner_name = result[0]
        if username != owner_name:
            print(f"Only the group owner '{owner_name}' can delete the group!")
            return

        # 刪除群組成員
        delete_members_query = "DELETE FROM group_member WHERE group_name = %s"
        cursor.execute(delete_members_query, (group_name,))

        # 刪除群組
        delete_group_query = "DELETE FROM grouplist WHERE group_name = %s"
        cursor.execute(delete_group_query, (group_name,))

        # 提交更改
        connection.commit()

        print("Group delete success!")

    except Error as e:
        print(f"資料庫連線失敗，錯誤: {e}")

    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()


# 主程式，根據參數傳入
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("使用方法: python delgroup.py <username> <group_name>")
    else:
        username = sys.argv[1]
        group_name = sys.argv[2]

        delete_group(username, group_name)
