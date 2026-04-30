import re

def analyse_valgrind(file_path):
    errors = 0
    leaks = 0
    others = 0

    with open(file_path, "r") as f:
        for line in f:
            # erreurs
            if "ERROR SUMMARY" in line:
                match = re.search(r'(\d+)\s+errors', line)
                if match:
                    errors += int(match.group(1))

            # leaks
            if ("definitely lost" in line or
                "indirectly lost" in line or
                "possibly lost" in line):
                leaks += 1

            # autres infos suspectes
            if "invalid" in line.lower() or "uninitialised" in line.lower():
                others += 1

    return errors, leaks, others


def analyse_helgrind(file_path):
    errors = 0
    others = 0

    with open(file_path, "r") as f:
        for line in f:
            # erreurs helgrind
            if "ERROR SUMMARY" in line:
                match = re.search(r'(\d+)\s+errors', line)
                if match:
                    errors += int(match.group(1))

            # conflits potentiels
            if ("Possible data race" in line or
                "conflict" in line.lower()):
                others += 1

    return errors, others


def main(val_file, hel_file):
    val_errors, val_leaks, val_others = analyse_valgrind(val_file)
    hel_errors, hel_others = analyse_helgrind(hel_file)

    print("===== VALGRIND =====")
    print(f"Erreurs : {val_errors}")
    print(f"Leaks   : {val_leaks}")
    print(f"Autres  : {val_others}")

    print("\n===== HELGRIND =====")
    print(f"Erreurs : {hel_errors}")
    print(f"Autres  : {hel_others}")


if __name__ == "__main__":
    main("log_val.txt", "log_hel.txt")