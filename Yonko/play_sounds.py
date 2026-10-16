#!/usr/bin/env python3
"""Plays a song on the Mac, controlled by the Arduino.

  Force sensor pressed  -> song starts (from the beginning if it had finished)
  Mushroom knob turned  -> volume follows the Arduino's musicVolume (0 to 30)
  Mushroom knob pushed  -> pause, push again to continue
  Hand near ultrasonic  -> plays a short sound effect

Setup (once):   python3 -m pip install pyserial pygame
Run:            python3 play_sounds.py
Pick a port:    python3 play_sounds.py /dev/cu.usbmodem14101
Stop:           Ctrl+C

Close the Arduino IDE Serial Monitor first, only one program can use the port.
"""

import glob
import os
import subprocess
import sys
import time

os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"
import pygame  # plays the song
import serial  # from pyserial, talks to the Arduino

BAUD = 9600
HERE = os.path.dirname(os.path.abspath(__file__))
SOUNDS_DIR = os.path.join(HERE, "sounds")
MAC_SOUNDS = "/System/Library/Sounds"

SONG = "song.mp3"

START_SONG_ON = "START"

LOOP_SONG = True

MAX_VOLUME = 30      # same top value as musicVolume in the Mushroom tab

# Short sound effects for other events (played with the Mac's afplay).
# Names not in "sounds" are looked up in the Mac's built-in sounds. None = nothing.
EFFECTS = {
    "NEAR": "Purr.aiff",
    "FAR": None,
    "RELEASE": None,
}

# True = also print the DATA lines (force and distance values).
SHOW_DATA = False


def find_port():
    # Uno shows up as usbmodem, the NG (and many clones) as usbserial
    ports = sorted(glob.glob("/dev/cu.usbmodem*") + glob.glob("/dev/cu.usbserial*")
                   + glob.glob("/dev/cu.wchusbserial*"))
    if not ports:
        sys.exit("No Arduino found. Is it plugged in? Try: ls /dev/cu.*")
    if len(ports) > 1:
        print("Several ports found, using the first one:", ", ".join(ports))
    return ports[0]


def sound_path(name):
    for folder in (SOUNDS_DIR, MAC_SOUNDS):
        path = os.path.join(folder, name)
        if os.path.exists(path):
            return path
    print(f"  (sound file not found: {name})")
    return None


class Player:
    def __init__(self):
        pygame.mixer.init()
        self.volume = 0.5
        self.paused = False
        self.song_path = sound_path(SONG)
        if self.song_path:
            pygame.mixer.music.load(self.song_path)
        else:
            print(f"Put your song in {SOUNDS_DIR} and set SONG at the top of this file.")

    def start_song(self):
        if not self.song_path:
            return
        if self.paused or pygame.mixer.music.get_busy():
            return  # paused with the knob, or already playing
        pygame.mixer.music.play(loops=-1 if LOOP_SONG else 0)
        pygame.mixer.music.set_volume(self.volume)
        print("  song started")

    def set_volume(self, level):
        self.volume = min(1.0, max(0.0, level / MAX_VOLUME))
        pygame.mixer.music.set_volume(self.volume)
        print(f"  volume {round(self.volume * 100)}%")

    def pause(self):
        self.paused = True
        pygame.mixer.music.pause()
        print("  paused")

    def play(self):
        self.paused = False
        pygame.mixer.music.unpause()
        print("  playing")


def play_effect(name):
    path = sound_path(name)
    if path:
        subprocess.Popen(["afplay", path])  # afplay is built into macOS


def main():
    port = sys.argv[1] if len(sys.argv) > 1 else find_port()
    print(f"Connecting to {port} ...")
    try:
        arduino = serial.Serial(port, BAUD, timeout=0.1)
    except serial.SerialException as e:
        sys.exit(f"Can't open {port}: {e}\nIs the Serial Monitor still open?")
    time.sleep(2)  # the Arduino restarts when the port opens

    player = Player()
    if START_SONG_ON == "START":
        player.start_song()
    print("Listening. Ctrl+C to stop.")

    while True:
        line = arduino.readline().decode("ascii", errors="ignore").strip()
        if not line:
            continue
        if line.startswith("DATA"):
            if SHOW_DATA:
                print(line)
            continue
        print(line)

        if line == START_SONG_ON:
            player.start_song()
        if line.startswith("VOLUME"):
            try:
                player.set_volume(int(line.split()[1]))
            except (IndexError, ValueError):
                pass  # half a line, ignore it
        elif line == "PAUSE":
            player.pause()
        elif line == "PLAY":
            player.play()
        elif EFFECTS.get(line):
            play_effect(EFFECTS[line])


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nStopped.")
