#!/usr/bin/python3

from random import randrange, shuffle
from os import system

test_num = 0
while True:
    test_num += 1
    print(f'testing on test {test_num}', end='\r')

    with open('test', 'w') as test:
        # write generator here
        pass

    system('./a < test > wa 2> /dev/null')
    system('./brute < test > ok 2> /dev/null')
    if open('ok', 'r').read().strip() != open('wa', 'r').read().strip():
        print(f'wrong answer on test {test_num}:')
        system('cat test')
        print('\nyour answer:')
        system('cat wa')
        print('\ncorrect answer:')
        system('cat ok')
        break
