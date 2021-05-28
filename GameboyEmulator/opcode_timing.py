# Read opcode timings from https://izik1.github.io/gbops/, stored in opcode_table.html, and place into a C++ file
from bs4 import BeautifulSoup
import functools

with open('opcode_table.html', 'r') as f:
    contents = f.read()

timings = []

soup = BeautifulSoup(contents, 'html.parser')
table = soup.find('tbody')
for item in table.find_all('td'):
    if 'unused' in item['class']:
        timings.append('0')
    else:
        timings.append(item.find('pre').get_text().splitlines()[1].strip())

def get_timing_from_string(timing, branched=False):
    if timing == '0':
        return timing
    t_value = timing.split(' ')[1]
    if t_value[:-1].isnumeric():
        return t_value[:-1]
    else:
        branchless_val, branch_val = t_value.split('-')
        if branched:
            return branch_val[:-1]
        else:
            return branchless_val[:-1]

branchless_timings = list(map(get_timing_from_string, timings))
branched_timings = list(map(functools.partial(get_timing_from_string, branched=True), timings))

def convert_to_c_array(timings, name):
    return f"const byte {name}[{len(branchless_timings)}] = {{ \n \t {', '.join(timings)} \n}};\n"

with open('opcode_timings.h', 'w') as f:
    f.write('#include "common.h"\n\n')
    f.write(convert_to_c_array(branchless_timings, 'branchlessCycles'))
    f.write(convert_to_c_array(branched_timings, 'branchedCycles'))