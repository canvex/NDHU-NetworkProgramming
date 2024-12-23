import mysql.connector
from mysql.connector import Error
import sys
from mysqlConn import create_connection  # 假設你已經有 create_connection 函數

def remove_users_from_group(group_name, inviter, users_to_remove):
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

        # 確認群組擁有者
        check_owner_query = "SELECT owner_name FROM grouplist WHERE group_name = %s"
        cursor.execute(check_owner_query, (group_name,))
        result = cursor.fetchone()

        owner_name = result[0]
        if inviter != owner_name:
            print(f"Only the group owner '{owner_name}' can remove members!")
            return

        users_not_in_group = []
        users_not_found = []
        users_removed = []

        for user in users_to_remove:
            # 確認使用者是否存在
            check_user_query = "SELECT COUNT(*) FROM users WHERE username = %s"
            cursor.execute(check_user_query, (user,))
            user_result = cursor.fetchone()

            if user_result[0] == 0:
                users_not_found.append(user)
                continue

            # 確認使用者是否在該群組中
            check_member_query = "SELECT COUNT(*) FROM group_member WHERE group_name = %s AND username = %s"
            cursor.execute(check_member_query, (group_name, user))
            member_result = cursor.fetchone()

            if member_result[0] == 0:
                users_not_in_group.append(user)
            else:
                # 移除使用者
                remove_user_query = "DELETE FROM group_member WHERE group_name = %s AND username = %s"
                cursor.execute(remove_user_query, (group_name, user))
                users_removed.append(user)

        connection.commit()

        # 顯示結果
        if users_not_in_group:
            print(f"{' '.join(users_not_in_group)} is not in group.")
        if users_removed:
            print(f"{' '.join(users_removed)} remove success !")
        if users_not_found:
            print(f"{' '.join(users_not_found)} not found !")

    except Error as e:
        print(f"資料庫連線失敗，錯誤: {e}")

    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()


# 主程式，根據參數傳入
if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("使用方法: python remove.py <inviter> <group_name> <user_name1> <user_name2> ...")
    else:
        group_name = sys.argv[2]
        inviter = sys.argv[1]
        users_to_remove = sys.argv[3:]

        remove_users_from_group(group_name, inviter, users_to_remove)
