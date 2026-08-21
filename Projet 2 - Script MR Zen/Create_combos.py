import time
import sys
import os
import pygame
import threading
import queue
from pygame._sdl2 import controller

TRIGGER_PRESS_THRESHOLD = 12000
TRIGGER_RELEASE_THRESHOLD = 6000

STICK_DEADZONE = 9000
STICK_CHANGE_THRESHOLD = 5000

RESET = "\033[0m"
RED = "\033[91m"
GREEN = "\033[92m"
YELLOW = "\033[93m"
BLUE = "\033[94m"

BUTTON_NAMES = {
    0: "✕",
    1: "○",
    2: "□",
    3: "△",
    4: "Create",
    5: "PS",
    6: "Options",
    7: "L3",
    8: "R3",
    9: "L1",
    10: "R1",
    11: "D-Pad ↑",
    12: "D-Pad ↓",
    13: "D-Pad ←",
    14: "D-Pad →",
    15: "Touchpad",
}

AXIS_LEFT_X = 0
AXIS_LEFT_Y = 1
AXIS_RIGHT_X = 2
AXIS_RIGHT_Y = 3
AXIS_L2 = 4
AXIS_R2 = 5

last_press_time = None
l2_pressed = False
r2_pressed = False

current_sequence = []
command_queue = queue.Queue()
sequence_lock = threading.Lock()

left_stick = [0, 0]
right_stick = [0, 0]

last_left_stick = [0, 0]
last_right_stick = [0, 0]


def ask_stick_mode():
    while True:
        answer = input(
            "Afficher les mouvements des joysticks ? (o/n) : "
        ).strip().lower()

        if answer in ("o", "oui", "y", "yes"):
            return True

        if answer in ("n", "non", "no"):
            return False

        print("Réponds par o ou n.")


def get_button_color(button_name):
    if button_name in ("✕", "○", "□", "△"):
        return BLUE

    if button_name.startswith("D-Pad"):
        return GREEN

    if button_name in ("L1", "L2", "R1", "R2"):
        return GREEN

    return RED


def colored(text, color):
    return f"{color}{text}{RESET}"


def register_press(button_name):
    global last_press_time

    current_time = time.perf_counter()

    with sequence_lock:
        if last_press_time is None:
            delta_ms = 0.0
        else:
            delta_ms = (current_time - last_press_time) * 1000

        last_press_time = current_time

        current_sequence.append({
            "button": button_name,
            "delay": delta_ms,
            "type": "button"
        })

    color = get_button_color(button_name)

    print(
        f"{colored(button_name, color):<26} "
        f"| +{delta_ms:9.3f} ms",
        flush=True
    )


def register_stick(stick_name, x, y):
    global last_press_time

    current_time = time.perf_counter()

    with sequence_lock:
        if last_press_time is None:
            delta_ms = 0.0
        else:
            delta_ms = (current_time - last_press_time) * 1000

        last_press_time = current_time

        current_sequence.append({
            "button": f"{stick_name} X={x} Y={y}",
            "delay": delta_ms,
            "type": "stick"
        })

    name = f"{stick_name} X={x:+6d} Y={y:+6d}"

    print(
        f"{colored(name, YELLOW):<38} "
        f"| +{delta_ms:9.3f} ms",
        flush=True
    )


def sequence_to_text(sequence):
    if not sequence:
        return "(séquence vide)"

    parts = []

    for i, action in enumerate(sequence):
        if i == 0:
            parts.append(action["button"])
        else:
            parts.append(
                f"{action['delay']:.3f}ms -> {action['button']}"
            )

    return " -> ".join(parts)


def save_sequence(name):
    global last_press_time

    with sequence_lock:
        if not current_sequence:
            print("\nAucune séquence à sauvegarder.\n")
            return

        sequence_copy = list(current_sequence)
        current_sequence.clear()
        last_press_time = None

    sequence_text = sequence_to_text(sequence_copy)

    print()
    print("=" * 70)
    print(f"COMBO SAUVEGARDÉ : {name}")
    print(sequence_text)
    print("=" * 70)
    print()

    with open("combos.txt", "a", encoding="utf-8") as file:
        file.write(f"{name}\n")
        file.write(f"{sequence_text}\n")
        file.write("\n")


def clear_sequence():
    global last_press_time

    with sequence_lock:
        current_sequence.clear()
        last_press_time = None

    print()
    print("=" * 70)
    print("SÉQUENCE SUPPRIMÉE")
    print("=" * 70)
    print()


def print_sequence():
    with sequence_lock:
        sequence_copy = list(current_sequence)

    print()
    print("=" * 70)
    print("SÉQUENCE ACTUELLE")
    print(sequence_to_text(sequence_copy))
    print("=" * 70)
    print()


def command_listener_windows():
    import msvcrt

    while True:
        char = msvcrt.getwch().lower()

        if char == "s":
            print("\nNom du combo : ", end="", flush=True)

            name_chars = []

            while True:
                key = msvcrt.getwch()

                if key == "\r":
                    print()
                    break

                if key == "\b":
                    if name_chars:
                        name_chars.pop()
                        print("\b \b", end="", flush=True)
                    continue

                if key in ("\x00", "\xe0"):
                    msvcrt.getwch()
                    continue

                if key.isprintable():
                    name_chars.append(key)
                    print(key, end="", flush=True)

            name = "".join(name_chars).strip()

            if not name:
                name = "Combo sans nom"

            command_queue.put(("save", name))

        elif char == "d":
            command_queue.put(("delete", None))

        elif char == "p":
            command_queue.put(("print", None))

        elif char == "q":
            command_queue.put(("quit", None))
            break


def command_listener_unix():
    import termios
    import tty

    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)

    try:
        tty.setcbreak(fd)

        while True:
            char = sys.stdin.read(1).lower()

            if char == "s":
                termios.tcsetattr(
                    fd,
                    termios.TCSADRAIN,
                    old_settings
                )

                print("\nNom du combo : ", end="", flush=True)
                name = input().strip()

                if not name:
                    name = "Combo sans nom"

                command_queue.put(("save", name))

                tty.setcbreak(fd)

            elif char == "d":
                command_queue.put(("delete", None))

            elif char == "p":
                command_queue.put(("print", None))

            elif char == "q":
                command_queue.put(("quit", None))
                break

    finally:
        termios.tcsetattr(
            fd,
            termios.TCSADRAIN,
            old_settings
        )


def command_listener():
    if os.name == "nt":
        command_listener_windows()
    else:
        command_listener_unix()


show_sticks = ask_stick_mode()

pygame.init()
controller.init()

if controller.get_count() == 0:
    print("Aucune manette détectée.")
    pygame.quit()
    raise SystemExit

controller_index = None

for i in range(controller.get_count()):
    if controller.is_controller(i):
        controller_index = i
        break

if controller_index is None:
    print("Manette non reconnue par SDL.")
    pygame.quit()
    raise SystemExit

gamepad = controller.Controller(controller_index)

print()
print("=" * 70)
print("LECTEUR D'INPUTS PS5")
print("=" * 70)
print()
print(f"{BLUE}✕ ○ □ △{RESET}")
print(f"{GREEN}L1 L2 R1 R2{RESET}")
print(f"{GREEN}D-Pad{RESET}")
print(f"{RED}L3 R3 Create Options PS Touchpad{RESET}")

if show_sticks:
    print(f"{YELLOW}Joysticks : ACTIVÉS{RESET}")
else:
    print("Joysticks : DÉSACTIVÉS")

print()
print("COMMANDES :")
print("S = sauvegarder")
print("D = supprimer")
print("P = afficher")
print("Q = quitter")
print()
print("=" * 70)
print()

threading.Thread(
    target=command_listener,
    daemon=True
).start()

running = True

try:
    while running:

        while not command_queue.empty():
            command, value = command_queue.get()

            if command == "save":
                save_sequence(value)
                print("Nouvelle séquence :")
                print()

            elif command == "delete":
                clear_sequence()
                print("Nouvelle séquence :")
                print()

            elif command == "print":
                print_sequence()

            elif command == "quit":
                running = False

        for event in pygame.event.get():

            if event.type == pygame.QUIT:
                running = False

            elif event.type == pygame.CONTROLLERBUTTONDOWN:
                button_id = event.button

                button_name = BUTTON_NAMES.get(
                    button_id,
                    f"Bouton inconnu ({button_id})"
                )

                register_press(button_name)

            elif event.type == pygame.CONTROLLERAXISMOTION:
                axis = event.axis
                value = event.value

                if axis == AXIS_L2:
                    if (
                        not l2_pressed
                        and value >= TRIGGER_PRESS_THRESHOLD
                    ):
                        l2_pressed = True
                        register_press("L2")

                    elif (
                        l2_pressed
                        and value <= TRIGGER_RELEASE_THRESHOLD
                    ):
                        l2_pressed = False

                elif axis == AXIS_R2:
                    if (
                        not r2_pressed
                        and value >= TRIGGER_PRESS_THRESHOLD
                    ):
                        r2_pressed = True
                        register_press("R2")

                    elif (
                        r2_pressed
                        and value <= TRIGGER_RELEASE_THRESHOLD
                    ):
                        r2_pressed = False

                elif show_sticks:

                    if axis == AXIS_LEFT_X:
                        left_stick[0] = value

                    elif axis == AXIS_LEFT_Y:
                        left_stick[1] = value

                    elif axis == AXIS_RIGHT_X:
                        right_stick[0] = value

                    elif axis == AXIS_RIGHT_Y:
                        right_stick[1] = value

                    if axis in (AXIS_LEFT_X, AXIS_LEFT_Y):
                        x, y = left_stick

                        active = (
                            abs(x) >= STICK_DEADZONE
                            or abs(y) >= STICK_DEADZONE
                        )

                        changed = (
                            abs(x - last_left_stick[0])
                            >= STICK_CHANGE_THRESHOLD
                            or
                            abs(y - last_left_stick[1])
                            >= STICK_CHANGE_THRESHOLD
                        )

                        if active and changed:
                            register_stick(
                                "Stick gauche",
                                x,
                                y
                            )

                            last_left_stick[0] = x
                            last_left_stick[1] = y

                    elif axis in (AXIS_RIGHT_X, AXIS_RIGHT_Y):
                        x, y = right_stick

                        active = (
                            abs(x) >= STICK_DEADZONE
                            or abs(y) >= STICK_DEADZONE
                        )

                        changed = (
                            abs(x - last_right_stick[0])
                            >= STICK_CHANGE_THRESHOLD
                            or
                            abs(y - last_right_stick[1])
                            >= STICK_CHANGE_THRESHOLD
                        )

                        if active and changed:
                            register_stick(
                                "Stick droit",
                                x,
                                y
                            )

                            last_right_stick[0] = x
                            last_right_stick[1] = y

        time.sleep(0.001)

except KeyboardInterrupt:
    print("\nArrêt.")

finally:
    try:
        gamepad.quit()
    except Exception:
        pass

    pygame.quit()