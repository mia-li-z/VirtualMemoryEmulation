# import pandas as pd
# from matplotlib import pyplot as plt

# # Set the figure size
# plt.rcParams["figure.figsize"] = [7.00, 3.50]
# plt.rcParams["figure.autolayout"] = True

# # Make a list of columns
# columns = ['Page Faults', 'Disk Reads', 'Disk Writes']

# # Read a CSV file
# df = pd.read_csv("csv-files/rand-scan.csv", usecols=columns)

# # Plot the lines
# df.plot()

# plt.show()



# # You do not need that many import statements, so we just import
# # numpy and matplotlib using the common alias 'np' and 'plt'.
# import numpy as np
# import matplotlib.pyplot as plt
# matplotlib.style.use('ggplot')

# # Using numpy we can use the function loadtxt to load your CSV file.
# # We ignore the first line with the column names and use ',' as a delimiter.
# data = np.loadtxt('csv-files/rand-scan.csv', delimiter=',', skiprows=1)

# # You can access the columns directly, but let us just define them for clarity.
# # This uses array slicing/indexing to cut the correct columns into variables.
# time = data[:,0]
# ground_speed = data[:,1]
# voltage = data[:,2]
# airspeed = data[:,3]

# # With matplotlib we define a new subplot with a certain size (10x10)
# fig, ax = plt.subplots(figsize=(10,10))

# ax.plot(time, ground_speed, label='Ground speed [m/s]')
# ax.plot(time, voltage, label='Voltage [V]')

# # Show the legend
# plt.legend()

import matplotlib.pyplot as plt
import csv
  
x = []
y = []
  
with open('csv-files/rand-scan.csv','r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    for row in lines:
        print(row)
        while(row[0]!='Pages'):
            x.append(int(row[1]))
            y.append(int(row[2]))
  
plt.plot(x, y, color = 'g', linestyle = 'dashed',
         marker = 'o',label = "Weather Data")
  
plt.xticks(rotation = 25)
plt.xlabel('Dates')
plt.ylabel('Temperature(°C)')
plt.title('Weather Report', fontsize = 20)
plt.grid()
plt.legend()
plt.show()

# import pandas as pd
# import matplotlib.pyplot as mp
 
# # take data
# data = pd.read_csv("csv-files/rand-scan.csv")
 
# # form dataframe
# data = data.head()
# print(data)
 
# df = pd.DataFrame(data, columns=["Frames", "Page Faults", "Disk Reads", "Disk Writes"])
 
# # plot the dataframe
# df.plot(x="Frames", y=["Page Faults", "Disk Reads", "Disk Writes"], kind="line", figsize=(10, 10))
 
# # print bar graph
# mp.show()
plt.savefig('foo.png')