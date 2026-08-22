import os
import subprocess

class fcolors:
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    WHITE = '\033[97m'
    ENDC = '\033[0m'

class bcolors:
    GREEN = '\033[102m'
    YELLOW = '\033[103m'
    RED = '\033[101m'
    WHITE = '\033[107m'
    ENDC = '\033[0m'


# Original source - https://stackoverflow.com/a/59803793
# Modified to remove ext requirement
def scandir(dir):
    subfolders, files = [], []
    for f in os.scandir(dir):
        if f.is_dir():
            subfolders.append(f.path)
        if f.is_file() and os.path.splitext(f)[1] != ".c": # skip c files
            files.append(f.path)
    for dir in list(subfolders):
        sf, f = scandir(dir)
        subfolders.extend(sf)
        files.extend(f)
    return subfolders, files

def get_files(dir):
    _, files = scandir(dir)
    return files

def parse_test(name):
    test_case = ""
    expected = ""
    with open(name) as f:
        test_case += f.readline().rstrip("\r\n")
        f.readline() # skip expect
        expected += f.readline().rstrip("\r\n")
    return test_case, expected

def run_lexer_test(f):
    return subprocess.run(['./build/lexer', f], stdout=subprocess.PIPE)

def clean_output(str):
    str = str.decode('UTF-8')
    str = str.split()
    return ''.join(str)

def text_lexer():
    files = get_files("./tests/lexer")
    passed = 0
    for f in files:
        t, e = parse_test(f)
        o = run_lexer_test(t)
        if o.returncode == 0:
            out = clean_output(o.stdout)
            if (out == e):
                passed += 1
                print(f"{t}: {bcolors.GREEN + fcolors.WHITE}PASS{bcolors.ENDC}")
            else:
                print(f"{t}: {bcolors.RED + fcolors.WHITE}FAIL{bcolors.ENDC}, EXPECTED: {e}, GOT: {out}")
        else:
            print(f"{bcolors.RED + fcolors.WHITE}Program returned with {o.returncode}.{bcolors.ENDC}")
    print(f"Tests: {passed}/{len(files)}")

text_lexer()