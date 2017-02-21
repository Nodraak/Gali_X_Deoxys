#!/usr/bin/env python
# -*- coding: utf-8 -*-

import subprocess
import json
import sys


def usage():
    print('Usage: %s <board:str> <flash:int> <ram:int>' % sys.argv[0])
    exit(1)

def norm(n):
    if n < 1024:
        return '%d B' % n
    else:
        return '%d KB' % (n/1024)

def format_stats_to_dic(summary, what):
    ret = {
        'used': summary['total_%s' % what],
        'free': BOARD[what]-summary['total_%s' % what],
        'total': BOARD[what],
    }

    ret['used_p'] = '%d' % (100*ret['used']/ret['total'])
    ret['free_p'] = '%d' % (100*ret['free']/ret['total'])

    ret['used'] = norm(ret['used'])
    ret['free'] = norm(ret['free'])
    ret['total'] = norm(ret['total'])

    return ret


def main(BOARD):
    out = subprocess.check_output([
        'python', 'mbed-os/tools/memap.py',
        '.build/NUCLEO_%s/GCC_ARM/out.map' % BOARD['name'],
        '-t', 'GCC_ARM',
        '-e', 'json'
    ])

    all_data = json.loads(out)
    summary = all_data[-1]['summary']



    flash = format_stats_to_dic(summary, 'flash')
    ram = format_stats_to_dic(summary, 'ram')

    print('+-------+----------------+----------------+----------+')
    print('| What  |       Used (%) |       Free (%) |    Total |')
    print('+-------+----------------+----------------+----------+')
    print('| Flash | {used:>8s} ({used_p:>2s}%) | {free:>8s} ({free_p:>2s}%) | {total:>8s} |'.format(**flash))
    print('| RAM   | {used:>8s} ({used_p:>2s}%) | {free:>8s} ({free_p:>2s}%) | {total:>8s} |'.format(**ram))
    print('+-------+----------------+----------------+----------+')


if __name__ == '__main__':
    if len(sys.argv) != 4:
        usage()

    try:
        BOARD = {
            'name': sys.argv[1],
            'flash': int(sys.argv[2]),
            'ram': int(sys.argv[3]),
        }
    except ValueError:
        usage()

    main(BOARD)
