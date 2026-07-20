#!/usr/bin/env python3
"""Dependency-free end-to-end smoke tests for the UCI process."""

import os
import queue
import subprocess
import sys
import tempfile
import threading
import time


class Engine:
    def __init__(self, executable):
        self.process = subprocess.Popen(
            [executable],
            cwd=tempfile.gettempdir(),
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1,
        )
        self.lines = queue.Queue()
        self.reader = threading.Thread(target=self._read_stdout, daemon=True)
        self.reader.start()

    def _read_stdout(self):
        for line in self.process.stdout:
            self.lines.put(line.rstrip("\n"))
        self.lines.put(None)

    def send(self, command):
        self.process.stdin.write(command + "\n")
        self.process.stdin.flush()

    def read_until(self, predicate, timeout=5.0):
        deadline = time.monotonic() + timeout
        lines = []
        while time.monotonic() < deadline:
            try:
                line = self.lines.get(timeout=deadline - time.monotonic())
            except queue.Empty:
                break
            if line is None:
                stderr = self.process.stderr.read()
                raise AssertionError(f"engine exited early ({self.process.poll()}): {stderr}")
            lines.append(line)
            if predicate(line):
                return lines
        raise AssertionError(f"timed out waiting for engine output; received: {lines}")

    def bestmove(self, command, timeout=5.0):
        self.send(command)
        lines = self.read_until(lambda line: line.startswith("bestmove "), timeout)
        move = lines[-1].split()[1]
        assert move == "0000" or (
            len(move) in (4, 5)
            and move[0] in "abcdefgh"
            and move[1] in "12345678"
            and move[2] in "abcdefgh"
            and move[3] in "12345678"
            and (len(move) == 4 or move[4] in "nbrq")
        ), move
        return move, lines

    def close(self):
        if self.process.poll() is None:
            self.send("quit")
            self.process.wait(timeout=3)
        stderr = self.process.stderr.read()
        assert self.process.returncode == 0, stderr


def main():
    executable = os.path.abspath(sys.argv[1])
    engine = Engine(executable)
    try:
        engine.send("uci")
        handshake = engine.read_until(lambda line: line == "uciok")
        assert "id name ChessEngine" in handshake
        assert "id author Bhoomish" in handshake

        engine.send("isready")
        assert engine.read_until(lambda line: line == "readyok")[-1] == "readyok"

        engine.send("position startpos moves e2e4 e7e5")
        move, lines = engine.bestmove("go depth 2")
        assert move != "0000"
        assert any(line.startswith("info depth 2 ") for line in lines), lines

        # Legal special moves must be accepted in long algebraic UCI form.
        engine.send("position startpos moves e2e4 e7e5 g1f3 b8c6 f1e2 g8f6 e1g1")
        assert engine.bestmove("go depth 1")[0] != "0000"
        engine.send("position startpos moves e2e4 a7a6 e4e5 d7d5 e5d6")
        assert engine.bestmove("go depth 1")[0] != "0000"
        engine.send("position fen 7k/P7/8/8/8/8/8/7K w - - 0 1 moves a7a8q")
        assert engine.bestmove("go depth 1")[0] != "0000"

        # Invalid updates are transactional: this checkmated board remains active.
        engine.send("position fen 7k/6Q1/6K1/8/8/8/8/8 b - - 0 1")
        engine.send("position startpos moves e2e5")
        assert engine.read_until(lambda line: "illegal move" in line)[-1].startswith("info string")
        assert engine.bestmove("go depth 1")[0] == "0000"

        engine.send("position fen 8/8/8/8/8/8/8/8 w - - 0 1")
        assert "exactly one king" in engine.read_until(
            lambda line: "invalid position" in line
        )[-1]

        engine.send("position startpos")
        started = time.monotonic()
        assert engine.bestmove("go movetime 40", timeout=2.0)[0] != "0000"
        assert time.monotonic() - started < 1.5

        engine.send("position startpos")
        engine.send("go infinite")
        time.sleep(0.05)
        engine.send("isready")
        assert engine.read_until(lambda line: line == "readyok", timeout=2.0)[-1] == "readyok"
        assert engine.bestmove("stop", timeout=2.0)[0] != "0000"

        engine.send("ucinewgame")
        engine.send("position startpos")
        started = time.monotonic()
        assert engine.bestmove("go wtime 1000 btime 1000 winc 0 binc 0", timeout=2.0)[0] != "0000"
        assert time.monotonic() - started < 1.5
    finally:
        engine.close()

    # Quitting must cancel and join an active worker without waiting for a result.
    quitting_engine = Engine(executable)
    quitting_engine.send("go infinite")
    time.sleep(0.02)
    quitting_engine.send("quit")
    quitting_engine.process.wait(timeout=2.0)
    assert quitting_engine.process.returncode == 0, quitting_engine.process.stderr.read()

    print("UCI smoke tests passed")


if __name__ == "__main__":
    main()
