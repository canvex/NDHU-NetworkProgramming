import mysql.connector
from mysql.connector import Error
import sys
from mysqlConn import create_connection  # 假設你已經有 create_connection 函數

def leave_group(group_name, user_name):
    try:
        connection = create_connection()
        if connection is None:
            return

        cursor = connection.cursor()

        # 確認群組是否存在
        check_group_query = "SELECT COUNT(*) FROM grouplist WHERE group_name = %s"
        cursor.execute(check_group_query, (group_name,))
        group_result = cursor.fetchone()

        if group_result[0] == 0:
            print("Group not found !")
            return

        # 確認群組的擁有者
        check_owner_query = "SELECT owner_name FROM grouplist WHERE group_name = %s"
        cursor.execute(check_owner_query, (group_name,))
        owner_result = cursor.fetchone()

        owner_name = owner_result[0]
        if user_name == owner_name:
            print("You are the owner of the group, you CAN NOT leave")
            return

        # 確認使用者是否在該群組中
        check_member_query = "SELECT COUNT(*) FROM group_member WHERE group_name = %s AND username = %s"
        cursor.execute(check_member_query, (group_name, user_name))
        member_result = cursor.fetchone()

        if member_result[0] == 0:
            print("Leave fault !")
            return

        # 移除使用者
        remove_user_query = "DELETE FROM group_member WHERE group_name = %s AND username = %s"
        cursor.execute(remove_user_query, (group_name, user_name))
        connection.commit()

        print(f"Successfully left the group '{group_name}'.")

    except Error as e:
        print(f"資料庫連線失敗，錯誤: {e}")

    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()


# 主程式，根據參數傳入
if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("使用方法: python script_name.py <user_name> <group_name>")
    else:
        group_name = sys.argv[2]
        user_name = sys.argv[1]
        leave_group(group_name, user_name)
