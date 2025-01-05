import mysql.connector
from mysql.connector import Error
import sys
from mysqlConn import create_connection

def add_members_to_group(inviter, group_name, invitees):
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
            print(f"群組 '{group_name}' 不存在！")
            return

        # 確認邀請人是否是群組的擁有者
        check_owner_query = "SELECT owner_name FROM grouplist WHERE group_name = %s"
        cursor.execute(check_owner_query, (group_name,))
        result = cursor.fetchone()

        owner_name = result[0]
        if inviter != owner_name:
            print(f"只有群組 '{group_name}' 的擁有者 '{owner_name}' 才能邀請成員！")
            return

        # 處理每個被邀請的使用者
        users_to_add = []
        users_not_found = []
        users_already_in_group = []

        for invitee in invitees:
            # 確認被邀請人是否存在
            check_user_query = "SELECT COUNT(*) FROM users WHERE username = %s"
            cursor.execute(check_user_query, (invitee,))
            user_result = cursor.fetchone()

            if user_result[0] == 0:
                users_not_found.append(invitee)
                continue

            # 確認被邀請人是否已在群組中
            check_member_query = "SELECT COUNT(*) FROM group_member WHERE group_name = %s AND username = %s"
            cursor.execute(check_member_query, (group_name, invitee))
            member_result = cursor.fetchone()

            if member_result[0] > 0:
                users_already_in_group.append(invitee)
            else:
                users_to_add.append(invitee)

        # 將符合條件的使用者加入群組
        if users_to_add:
            insert_member_query = "INSERT INTO group_member (group_name, username) VALUES (%s, %s)"
            for user in users_to_add:
                cursor.execute(insert_member_query, (group_name, user))
            connection.commit()

            print(f"{' '.join(users_to_add)} add success !")

        # 顯示錯誤訊息
        if users_not_found:
            print(f"{' '.join(users_not_found)} not found !")

        if users_already_in_group:
            print(f"{' '.join(users_already_in_group)} already in group !")

    except Error as e:
        print(f"資料庫連線失敗，錯誤: {e}")

    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()


# 主程式，根據參數傳入
if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("使用方法: python addto.py <inviter> <group_name> <user_name1> <user_name2> ...")
    else:
        inviter = sys.argv[1]
        group_name = sys.argv[2]
        invitees = sys.argv[3:]

        add_members_to_group(inviter, group_name, invitees)
