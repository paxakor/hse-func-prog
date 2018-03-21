#!/usr/bin/python3

from random import randint
from socket import gethostname
from subprocess import run, DEVNULL
from time import sleep, time

import os

id2hash = dict()

def get_node_name(node_number):
    global id2hash
    if node_number not in id2hash:
        id2hash[node_number] = "Node" + str(abs(hash(str(node_number + time()))) % 10**5)
    return id2hash[node_number]


def get_full_node_name(node_number):
    return get_node_name(node_number) + "@" + gethostname()


class Config():
    NodeCount = 1 + 3

    Cookie = "mega_secret_cookie"
    MasterNode = randint(0, NodeCount - 1)
    FileName = os.path.abspath("./parallel_map.exs")


def magic_run(args, arg_pairs, call):
    for pair in arg_pairs:
        args.extend(pair)
    # print(" ".join(args))
    call(args)
    print("ENDED:", " ".join(args))


def run_master():
    sleep(0.5)  # wait until all workers start
    args = ["elixir"]
    arg_pairs = [
        ("--sname", get_node_name(Config.MasterNode)),
        ("--cookie", Config.Cookie),
        ("-S", Config.FileName),
        ("--workers", ','.join((get_full_node_name(i) for i in range(Config.NodeCount) if i != Config.MasterNode))),
    ]
    magic_run(args, arg_pairs, call=run)


def run_slave(node_number):
    # args = ["elixir"]
    args = ["iex"]
    arg_pairs = [
        ("--sname", get_node_name(node_number)),
        ("--cookie", Config.Cookie),
        ("-S", Config.FileName),
        ("--master", get_full_node_name(Config.MasterNode)),
    ]
    magic_run(args, arg_pairs, call=lambda *x: run(*x, stdout=DEVNULL))


def main():
    pids = []
    for i in range(Config.NodeCount):
        pid = os.fork()
        if not pid:
            if i == Config.MasterNode:
                return run_master
            else:
                return lambda: run_slave(i)
        else:
            if i == Config.MasterNode:
                pids.append(pid)

    def wait_children():
        for pid in pids:
            os.waitpid(pid, 0)

    return wait_children

if __name__ == '__main__':
    for i in range(Config.NodeCount):
        get_node_name(i)
    main()()
