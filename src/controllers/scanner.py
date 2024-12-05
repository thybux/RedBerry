import subprocess


class Scanner:
    def __init__(self):
        pass
    def run_nmap(self, target: str, option: str = "-sV"):
        try:
            command = ["nmap"] + option.split() + [target]
            result = subprocess.run(command, capture_output=True, text=True, check=True)
            return result.stdout
        except subprocess.CalledProcessError as e:
            return f"Erreur lors de l'exécution de Nmap : {e.stderr}"
        except Exception as e:
            return f"Erreur inattendue : {str(e)}"
