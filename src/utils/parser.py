import nmap
import json
from typing import Dict, Optional

class NmapParser:
    def __init__(self):
        self.nm = nmap.PortScanner()

    def scan_to_json(self, target: str, options: str = '-sV -p-') -> Dict:
        try:
            scan_results = self.nm.scan(target, arguments=options)
            #print("Structure complète:", json.dumps(scan_results, indent=2))  # Pour debug
            return scan_results
        except Exception as e:
            return {'error': True, 'message': str(e)}

    def format_results(self, scan_results: Dict) -> str:
        """
        Formate les résultats JSON en texte lisible
        """
        if scan_results.get('error'):
            return f"Erreur lors du scan : {scan_results['message']}"

        output = []
        
        # Informations générales du scan
        scan_info = scan_results.get('nmap', {})
        output.append("=== Informations du Scan ===")
        output.append(f"Version Nmap: {scan_info.get('version', 'Inconnue')}")
        output.append(f"Commande: {scan_info.get('command_line', 'Inconnue')}")
        
        # Pour chaque hôte scanné
        scan_data = scan_results.get('scan', {})
        for host, host_data in scan_data.items():
            output.append(f"\nHôte: {host}")
            output.append(f"État: {host_data.get('status', {}).get('state', 'inconnu')}")
            
            # Informations sur les ports
            if 'tcp' in host_data:
                output.append("\nPorts TCP:")
                for port, port_data in host_data['tcp'].items():
                    service_info = []
                    if port_data.get('name'):
                        service_info.append(port_data['name'])
                    if port_data.get('product'):
                        service_info.append(port_data['product'])
                    if port_data.get('version'):
                        service_info.append(port_data['version'])
                    
                    service_str = " - ".join(service_info) if service_info else "inconnu"
                    output.append(f"  {port}/{port_data['state']}: {service_str}")
            elif 'udp' in host_data:
                output.append("\nPorts UDP:")
                for port, port_data in host_data['udp'].items():
                    output.append(f"  {port}/{port_data['state']}: {port_data.get('name', 'inconnu')}")
                    
        return "\n".join(output)
