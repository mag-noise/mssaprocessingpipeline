import sys,os
sys.path.append(os.getcwd())

import MSSAPython as py
a = {'inboard_input': [[1, 2, 3], [4, 5, 6], [7, 8, 9]], 'outboard_input': [[1, 2, 3], [4, 5, 6], [7, 8, 9]], 'timestamp': [0, 1, 2, 3, 4, 5, 6, 7, 8], 'dimensions': 3, 'alpha_threshold': 0.05, 'segment_size': 3, 'window_size': 3}
print(py.pyEntry(a))