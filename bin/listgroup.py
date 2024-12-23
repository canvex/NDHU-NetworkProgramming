import mysql.connector
from mysql.connector import Error
import sys
from mysqlConn import create_connection  # 假設你已經有 create_connection 函數

def list_groups(user_name):
    try:
        connection = create_connection()
        if connection is None:
            return

        cursor = connection.cursor()

        # 查詢使用者所屬的所有群組和其群組擁有者
        query = """
            SELECT gm.group_name, gl.owner_name
            FROM group_member gm
            JOIN grouplist gl ON gm.group_name = gl.group_name
            WHERE gm.username = %s
        """
        cursor.execute(query, (user_name,))
        groups = cursor.fetchall()

        if not groups:
            print("Empty !")
        else:
            print("<owner>\t<group>")
            for group, owner in groups:
                print(f"{owner}\t{group}")

    except Error as e:
        print(f"資料庫連線失敗，錯誤: {e}")

    finally:
        if connection.is_connected():
            cursor.close()
            connection.close()


# 主程式，根據參數傳入
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("使用方法: python script_name.py <user_name>")
    else:
        user_name = sys.argv[1]
        list_groups(user_name)
