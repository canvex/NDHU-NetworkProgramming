import mysql.connector
import sys
from mysql.connector import Error
from mysqlConn import create_connection
import os

loginname = ""  # 全域變數
# 獲取目前進程的 PID
current_pid = os.getpid()

# 獲取父進程的 PID
parent_pid = os.getppid()

# print(f"目前的 PID 是: {current_pid}")
# print(f"父進程的 PID 是: {parent_pid}")

def update_name_if_pid_match(login_name):
    file_path = "/tmp/userlist"
    try:
        # 指定編碼來讀取檔案
        with open(file_path, 'r', encoding='utf-8') as file:
            lines = [line.strip() for line in file.readlines() if line.strip()]  # 過濾掉空行並去除每行的換行符

        # 取得最後一行資料並分析
        if lines:
            last_line = lines[-1].split()  # 使用split()來分隔各欄位，默認會以空白作為分隔符
            last_pid = last_line[-1]  # 假設PID在最後一個欄位

            # 獲取父進程的 PID
            parent_pid = str(os.getppid())  # 確保轉換為字串

            # 如果最後一個 PID 與父進程的 PID 一樣，更新名字
            if last_pid == parent_pid:
                # 替換名字為 login_name
                last_line[1] = login_name  # 假設名字是第二欄位
                lines[-1] = " ".join(last_line)  # 更新最後一行，這裡不加換行符

                # 移除所有空行
                lines = [line for line in lines if line.strip()]  # 過濾掉空行

                # 寫回更新後的內容
                with open(file_path, 'w', encoding='utf-8') as file:
                    # 使用writelines時，已經在lines中保留了換行符，因此不需要額外添加
                    file.writelines([line + "\n" for line in lines])  # 加回換行符

                print(f"Updated name to '{login_name}' for PID {parent_pid}")
            else:
                print(f"PID {parent_pid} does not match the last entry's PID.")
        else:
            print("The file is empty.")
    except Exception as e:
        print(f"An error occurred: {e}")



# 註冊功能
def register(connection):
    print("執行註冊")
    cursor = connection.cursor()
    username = input("username: ").strip()  # 清理輸入的多餘空格
    password = input("password: ").strip()

    # 查詢使用者名稱是否已存在
    query = "SELECT * FROM users WHERE username = %s"
    cursor.execute(query, (username,))
    existing_user = cursor.fetchone()

    if existing_user:
        print("使用者名稱已經存在，請選擇其他名稱。")
    else:
        # 插入新使用者資料
        insert_query = "INSERT INTO users (username, password) VALUES (%s, %s)"
        cursor.execute(insert_query, (username, password))
        connection.commit()  # 提交更改
        print("註冊成功！")
        return 0


# 登入驗證
def login(connection):
    global loginname  # 使用全域變數 loginname
    print("執行登入")
    cursor = connection.cursor()
    username = input("username: ").strip()  # 清理輸入的多餘空格
    password = input("password: ").strip()

    # 查詢是否存在該使用者名稱
    query = "SELECT * FROM users WHERE username = %s"
    cursor.execute(query, (username,))
    user = cursor.fetchone()

    if user is None:
        return 2
    else:
        # 檢查密碼是否正確
        if user[2] == password:  # 假設密碼在資料表的第3個欄位
            loginname = user[1]  # 假設使用者名稱在資料表的第2個欄位
            # 執行更新
            update_name_if_pid_match(loginname)
            return 0
        else:
            return 1


# 把登入名稱寫入/tmp/loginname
def write_loginname_to_file():
    global loginname
    if loginname:  # 如果 loginname 已經被賦值
        try:
            with open(f"/tmp/loginname.{parent_pid}", "w") as file:
                file.write(loginname)
            print(f"登入名稱 '{loginname}' 已寫入 /tmp/loginname.{parent_pid}")
        except Exception as e:
            print(f"寫入檔案時發生錯誤: {e}")
    else:
        print("沒有登入名稱，無法寫入檔案。")


# 主程式
def main():
    # 建立資料庫連線
    try:
        connection = create_connection()
    except Error as e:
        print(f"資料庫連線錯誤: {e}")
        sys.exit(1)

    while True:
        # 先執行登入
        status = login(connection)

        # 根據登入結果進行相應處理
        if status == 1:  # 密碼錯誤
            print("密碼錯誤，請重新嘗試。")
        elif status == 2:  # 使用者名稱不存在
            retry = input("使用者名稱不存在！您想註冊新帳號(1)還是重新嘗試登入(2)? <1/2>: ").strip()
            if retry == "1":
                register(connection)
            elif retry == "2":
                continue  # 重新嘗試登入
        elif status == 0:  # 登入成功
            print(f"歡迎光臨 {loginname} 已成功登入")
            write_loginname_to_file()  # 將登入名稱寫入檔案
            connection.close()  # 關閉資料庫連線
            break  # 結束程式


if __name__ == "__main__":
    main()
