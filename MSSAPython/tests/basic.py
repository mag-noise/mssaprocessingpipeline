# Running: go to folder with built functions and run is file from there
# Must match up python versions. Locally for me, activate the correct python environment
# pyenv-venv activate mssavenv
# 

import sys,os
sys.path.append(os.getcwd())
import spacepy.toolbox as tb
from spacepy import pycdf
import datetime
import calendar
import numpy as np
import MSSAPython as py
import pandas as pd 

# giving directory name
dirname = "C:/Users/klsteele/source/repos/mssaprocessingpipeline/MSSAPython/tests"
 
# giving file extension
ext = '.cdf'
 
# iterating over all files
for files in os.listdir(dirname):
    if files.endswith(ext):
        cdfLocation = os.path.join(dirname, files)
        print(cdfLocation)
        tb.update(leapsecs=True)
        cdf = pycdf.CDF(cdfLocation)
        #print(cdf)


        arr1 = np.array(cdf['CombinedMag1'])
        arr2 = np.array(cdf['CombinedMag2'])
        timestamp = np.array(cdf['Epoch'])


        lengthoftime=timestamp.size
        #print(lengthoftime)


        timeLam = lambda t: calendar.timegm(t.timetuple())
        timeFunc = np.vectorize(timeLam)
        timestamp = timeFunc(timestamp)

        a = {'inboard_input': arr1, 'outboard_input': arr2, 
        'timestamp': timestamp, 'dimensions': 3, 'alpha_threshold': 0.95, 
        'segment_size': lengthoftime, 'window_size': 10}

        outputData = py.pyEntry(a)
        outputData['inboard_output'] = outputData['inboard_output'].tolist()
        outputData['outboard_output'] = outputData['outboard_output'].tolist()
        outputData['inboard_wheel'] = outputData['inboard_wheel'].tolist()
        outputData['outboard_wheel'] = outputData['outboard_wheel'].tolist()
        # outputData['flags'] = outputData['flags'].tolist()
        del outputData['flags']
        outputData['inboard_real']=np.array(cdf['GeoSignalMag1']).flatten().tolist()
        outputData['outboard_real']=np.array(list(cdf['GeoSignalMag2'])).flatten().tolist()
        outputData['inboard_wheel_expected']=np.array(cdf['InterMag1']).flatten().tolist()
        outputData['outboard_wheel_expected']=np.array(cdf['InterMag2']).flatten().tolist()

        for key, value in outputData.items(): print(f"Size of {key}: {len(value)}")
        print(cdf['CombinedMag1'][0])
        print(cdf['TrendMag1'][0])
        print(cdf['GeoSignalMag1'][0])
        print(cdf['InterMag1'][0])
        print(outputData['inboard_output'][0], outputData['inboard_output'][1], outputData['inboard_output'][2])
        print(outputData['inboard_wheel'][0], outputData['inboard_wheel'][1], outputData['inboard_wheel'][2])


   
        # Create the pandas DataFrame
        df = pd.DataFrame.from_dict(outputData)
        df.to_csv(cdfLocation.replace(".cdf", "alpha95.csv"), index = False)
    else:
        continue
