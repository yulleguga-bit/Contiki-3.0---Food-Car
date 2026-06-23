import xml.etree.ElementTree as ET

try:
    tree = ET.parse('trace.xml')
    root = tree.getroot()
    
    with open('mobilidade_cooja.txt', 'w') as f:
        for timestep in root.findall('timestep'):
            time = timestep.get('time')
            for vehicle in timestep.findall('vehicle'):
                veh_id = vehicle.get('id')
                
                # Extrai apenas os numeros do ID do SUMO (ex: veh0 vira 0)
                num_id = ''.join(c for c in veh_id if c.isdigit())
                if not num_id:
                    num_id = "0"
                
                # Soma +5 para alinhar com os nos de veiculo do Cooja (5, 6, 7...)
                cooja_id = int(num_id) + 5
                
                x = vehicle.get('x')
                y = vehicle.get('y')
                
                # Escreve no formato exato do Cooja: id tempo x y
                f.write("{0} {1} {2} {3}\n".format(cooja_id, time, x, y))
                
    print("Sucesso! O arquivo 'mobilidade_cooja.txt' foi corrigido para o Cooja.")
except Exception as e:
    print("Erro na conversao: " + str(e))
