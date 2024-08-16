import os

dst_dir = 'eigen-dst'
os.makedirs(dst_dir, exist_ok=True)
with open('Amalgamate.h', 'r') as f:
    lines = f.readlines()

f = None
prefix = '// cubao-amalgamate-source: '
for line in lines:
    if line.startswith(prefix):
        path = line[len(prefix):].strip()
        path = f'{dst_dir}/{path}'
        os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
        f = open(path, 'w')
        f.write(line)
        continue
    if f is not None:
        f.write(line)
