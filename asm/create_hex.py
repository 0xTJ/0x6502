import sys
import os
import collections

if len(sys.argv) < 2:
    print('Must provide hex file as first argument.')
    sys.exit()

iFile = sys.argv[1]
path = os.path.split(iFile)

incStr = "_".join(['INCLUDE'] + [x.upper() for x in path[1].split('.')] + ["H"])

entries = open(iFile).readlines()

HexEntry = collections.namedtuple('HexEntry', ['byteCount', 'address', 'recordType', 'data', 'checksum'])
hexList = []

for line in entries:
    if line[:1] != ':':
        break
    
    line = line[1:]
    byteCount = int(line[:2], 16)
    line = line[2:]
    address = int(line[:4], 16)
    line = line[4:]
    recordType = int(line[:2], 16)
    line = line[2:]
    if byteCount > 0:
        data = line[:(byteCount * 2)]
        line = line[(byteCount * 2):]
    else:
        data = ''
    checksum = int(line[:2], 16)
    line = line[2:]
    
    thisEntry = HexEntry(byteCount, address, recordType, data, checksum)
    hexList = hexList + [thisEntry]

import re

def clean(s):
   s = re.sub('[^0-9a-zA-Z_]', '', s)
   s = re.sub('^[^a-zA-Z_]+', '', s)
   return s

oFile = os.path.join(path[0], path[1] + '.h')

with open(oFile, 'w') as o:
    o.write('#ifndef ' + incStr + '\n#define ' + incStr + '\n\n')
    for i in range(len(hexList)):
        hexEntry = hexList[i]
        data = hexEntry.data
        o.write('const uint8_t data' + str(i) + '[] PROGMEM = {')
        while len(data) > 0:
            o.write(' ' + format(int(data[:2], 16), '#04x'))
            data = data[2:]
            if len(data) > 0:
                o.write(',')
        o.write(' };\n');
    o.write('\n\n')
    o.write('struct RomHex {\n    const uint8_t byte_count;\n    const uint16_t address;\n    const uint8_t record_type;\n    const uint8_t *data;\n} const ')
    o.write('rom_hex[] PROGMEM = {\n')
    for i in range(len(hexList)):
        hexEntry = hexList[i]
        o.write('    {\n')
        o.write('        .byte_count = ' + format(hexEntry.byteCount, '#04x') + ',\n')
        o.write('        .address = ' + format(hexEntry.address, '#06x') + ',\n')
        o.write('        .record_type = ' + format(hexEntry.recordType, '#04x') + ',\n')
        o.write('        .data = data' + str(i))
        o.write('\n')
        o.write('    }')
        if i != len(hexList) - 1:
            o.write(',')
        o.write('\n')
    o.write('};\n\n')
    o.write('#endif  // NOT ' + incStr + '\n')
    
    
    
    
    
    
