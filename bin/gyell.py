import mysql.connector
import sys
import os
from mysqlConn import create_connection  # 假設你已經有 create_connection 函數


SERVER_FIFO_TEMPLATE = "/tmp/np_sv.{pid}"
user_file = "/tmp/userlist"


def gyell(sender, group_name, message):
    """查找群組成員並發送訊息"""
    # 連接 MySQL 資料庫
    connection = create_connection()
    cursor = connection.cursor()

    # 查詢該群組的所有成員名稱
    query = "SELECT username FROM group_member WHERE group_name = %s"
    cursor.execute(query, (group_name,))
    group_members = cursor.fetchall()

    # 如果群組不存在
    if not group_members:
        print("Group not found !")
        cursor.close()
        connection.close()
        return

    # 構造訊息格式
    formatted_message = f"\n<{group_name}:{sender}>: {message}"

    # 查找並發送訊息給每個群組成員
    for member in group_members:
        username = member[0]

        # 查找該用戶的 PID
        with open(user_file, "r") as file:
            for line in file:
                # 跳過空行
                if not line.strip():
                    continue

                try:
                    uid, name, ip, port, pid = line.strip().split()
                    if name == username:
                        pid = int(pid)
                        print(f"Sending message to {username} (PID: {pid})")
                        if not send_message_to_pid(pid, formatted_message):
                            print(f"Failed to send message to {username}")
                        break
                except ValueError:
                    print(f"Skipping invalid line: {line.strip()}")
                    continue

    # 關閉 MySQL 連線
    cursor.close()
    connection.close()

def send_message_to_pid(pid, message):
    """發送訊息到指定 PID 的 FIFO"""
    target_fifo = f"/tmp/np_sv.{pid}"

    try:
        # 嘗試以非阻塞模式打開 FIFO
        fd = open(target_fifo, 'w')
        fd.write(message)
        fd.close()
        print(f"Message sent to PID {pid}")
        return True
    except Exception as e:
        print(f"Error sending message to PID {pid}: {e}")
        return False

# 主程式，從命令行參數讀取發送者、群組名和訊息
if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: gyell <sender> <group_name> <message>")
        sys.exit(1)

    sender = sys.argv[1]
    group_name = sys.argv[2]
    message = " ".join(sys.argv[3:])

    gyell(sender, group_name, message)
