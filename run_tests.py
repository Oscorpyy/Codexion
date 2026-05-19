import subprocess
import sys

def run_tests(num_runs=10):
    success_count = 0
    # La phrase clé qui indique que tout s'est bien passé
    success_message = "🎉 All done and everything is perfect! Congratulations! 🎉"

    print(f"Lancement de {num_runs} tests en boucle...\n")

    for i in range(1, num_runs + 1):
        print(f"⏳ Exécution du test {i}/{num_runs}...", end="", flush=True)

        # Lance la commande 'make test'
        result = subprocess.run(["make", "test"], capture_output=True, text=True)

        # Vérifie si le message de succès est dans l'output
        if success_message in result.stdout:
            print(" ✅ Succès")
            success_count += 1
        else:
            print(" ❌ Échec")
            # Décommenter ces lignes si tu veux afficher l'output quand ça rate
            # print("=== OUTPUT EN CAS D'ÉCHEC ===")
            # print(result.stdout)
            # print("=============================")

    # Affichage du bilan
    print("\n" + "=" * 30)
    print(f"🏆 Bilan final : {success_count}/{num_runs} tests réussis")
    print("=" * 30)


if __name__ == "__main__":
    argv = sys.argv[1:]
    if len(argv) > 0:
        try:
            num_runs = int(argv[0])
            run_tests(num_runs)
        except ValueError:
            print("Usage: python run_tests.py <chemin_vers_le_dossier_du_"
                  "projet> [num_runs (optional, default=10)]")
            print("num_runs doit être un entier.")
    else:
        run_tests(10)
