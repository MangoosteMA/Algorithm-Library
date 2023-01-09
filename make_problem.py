#!/usr/bin/python3

'''
Parameters:
dir=X                  where X is the name of the directory
[optional] path=X      where X is the path where you want to create a problem
[optional] main=X      where X is the name of created solution. X=a by default
[optional] simple      it means that you don't need to create input/output data from some url
[optional] multitest   it means that the problem should contains multitest

If you didn't set simple:
url=X     where X is the link to the CODEFORCES problem. Other options will be avaliable soon
outfile   it means that you need to create output file for every test if it exists
'''

from sys import argv, exit
from os import system
import urllib.request

parameters = {}
simple_initialize = False
multitest = False
create_ans_file = False

for i in range(1, len(argv)):
    if argv[i] == 'simple':
        simple_initialize = True
        continue

    if argv[i] == 'outfile':
        create_ans_file = True
        continue

    if argv[i] == 'multitest':
        multitest = True
        continue

    pos = argv[i].index('=')
    if pos == len(argv[i]):
        print(f'Wrong parameter: {argv[i]}.')
        exit(1)

    key = argv[i][:pos]
    value = argv[i][pos + 1:]
    if key in parameters:
        print(f'Duplicate of {key}.')
        exit(2)

    parameters[key] = value

if not 'dir' in parameters:
    print('Didn\'t find \'dir\' in parameters.')
    exit(3)

dir_name = parameters['dir']
path = '.'
if 'path' in parameters:
    path = parameters['path']

solver_name = 'a'
if 'main' in parameters:
    solver_name = parameters['main']

system(f'rm -r {path}/{dir_name} 2> /dev/null')
system(f'mkdir {path}/{dir_name}')
if not multitest:
    system(f'cp ~/Programming/Templates/template.cpp {path}/{dir_name}/{solver_name}.cpp')
else:
    system(f'cp ~/Programming/Templates/template_multitest.cpp {path}/{dir_name}/{solver_name}.cpp')
system(f'cp ~/Programming/Templates/debug.h {path}/{dir_name}/.')

system(f'mkdir {path}/{dir_name}/.vscode')
system(f'cp ~/Library/Application\ Support/Code/User/settings.json {path}/{dir_name}/.vscode/settings.json')
# system(f'cp ~/Library/Application\ Support/Code/User/tasks.json {path}/{dir_name}/.vscode/tasks.json')
system(f'cp ~/Library/Application\ Support/Code/User/launch.json {path}/{dir_name}/.vscode/launch.json')

if simple_initialize:
    print('Done!')
    exit(0)

if not 'url' in parameters:
    print('Didn\'t find \'url\' in parameters.')
    exit(4)

url = parameters['url']
lines = urllib.request.urlopen(url).read().decode().split('\n')

INPUT = 'Input</div><pre>'
OUTPUT = 'Output</div><pre>'
SEPARATOR = '</pre></div>'

test_num = 0
for pos in range(len(lines)):
    if lines[pos].count(INPUT):
        test_num += 1
        new_pos = pos + 1
        with open(f'{path}/{dir_name}/in{test_num}', 'w') as input:
            while new_pos < len(lines) and lines[new_pos].count(SEPARATOR) == 0:
                print(lines[new_pos], file=input)
                new_pos += 1

        if create_ans_file and new_pos < len(lines) and lines[new_pos].count(OUTPUT):
            with open(f'{path}/{dir_name}/out{test_num}', 'w') as output:
                new_pos += 1
                while new_pos < len(lines) and lines[new_pos].count(SEPARATOR) == 0:
                    print(lines[new_pos], file=output)
                    new_pos += 1

print(f'Done! Number of created tests: {test_num}.')
