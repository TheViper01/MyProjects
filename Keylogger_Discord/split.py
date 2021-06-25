import os.path
import sys


def read_from_file(path):
    if not os.path.isfile(path):
        return False
    str = None
    try:
        f = open(path, 'rb')
        str = f.read()
        f.close()
    except:
        pass
    return str


def write_to_file(path, data):
    try:
        f = open(path, 'ab+')
        f.write(data)
        f.close()
    except:
        pass


filename = "D:\\Programming\\program_py\\keylogger_\\dist\\svchost.exe"
write_dir = "D:\\Programming\\program_py\\keylogger_\\dist\\split_files\\"
data = read_from_file(filename)
max_chunk = 6000000
written = 0
i = 1
while True:
    if len(data) - written > 0:
        to_write = min(max_chunk, (len(data) - written)) + written
        write_to_file(write_dir+str(i)+".file", data[written:to_write])
        written += to_write
        i += 1
    else:
        break

"""j = 50000
while j < 50378:
    os.remove(str(j)+".file")
    j += 1"""



