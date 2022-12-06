import pandas as pd
from matplotlib import pyplot as plt
from matplotlib.pyplot import figure

# Set the figure size
def plotPics(fileName):

    plt.rcParams["figure.figsize"] = [7.00, 3.50]
    plt.rcParams["figure.autolayout"] = True

    # Make a list of columns
    columns = ['Disk Reads', 'Disk Writes']

    # Read a CSV file
    df = pd.read_csv("csv-files/"+fileName+".csv", usecols=columns)

    # # Plot the lines and save the picture
    ax = df.plot.line()
    ax.set(xlabel='Frames')
    plt.title(fileName+' graph')

    ax.figure.savefig(fileName+'.png')

plotPics("fifo-sort")
plotPics("fifo-scan")
plotPics("fifo-focus")
plotPics("rand-sort")
plotPics("rand-scan")
plotPics("rand-focus")
plotPics("custom-sort")
plotPics("custom-scan")
plotPics("custom-focus")


# generate graph for algorithm comparison

algo = ["fifo", "rand", "custom"]
program = ["sort", "scan", "focus"]


pf = []
dw = []
x = [i for i in range(2, 101)]

for i, s in enumerate(algo):
    pf.append([])
    dw.append([])
    for j, a in enumerate(program):
        pf[i].append([])
        dw[i].append([])
        with open("csv-files/{}-{}.csv".format(s, a), "r") as fd:
            count = 0
            for line in fd:
                line = line.strip("\n").strip("\r").split(",")
                count += 1
                if len(line) == 0 or count == 1:
                    continue
                _frame = int(line[1])
                if _frame == 1:
                    continue
                _page_fault = int(line[2])
                _disk_writes = int(line[4])
                
                pf[i][j].append(_page_fault)
                dw[i][j].append(_disk_writes)


fig = figure(num=None, figsize=(30,16), dpi=80, facecolor='w', edgecolor='k')
fig.suptitle('Using page faults and disk writes to compare paging replacement algorithm for different programs', fontsize=18)

ax1 = fig.add_subplot(231)
ax1.title.set_text('Frames vs Page Faults - Sort')

ax2 = fig.add_subplot(232)
ax2.title.set_text('Frames vs Page Faults - Scan')

ax3 = fig.add_subplot(233)
ax3.title.set_text('Frames vs Page Faults - Focus')

ax4 = fig.add_subplot(234)
ax4.title.set_text('Frames vs Disk Writes - Sort')

ax5 = fig.add_subplot(235)
ax5.title.set_text('Frames vs Disk Writes - Scan')

ax6 = fig.add_subplot(236)
ax6.title.set_text('Frames vs Disk Writes - Focus')

count = 1
for j in range(0, 3):
    plt.subplot(2, 3, count)
    count += 1
    plt.xlabel('Frames')
    plt.ylabel('Page faults')
    plt.plot(x, pf[0][j], 'ro-', label = "fifo", alpha=.5, linewidth=4.5)
    plt.plot(x, pf[1][j], 'go-', label = "rand")
    plt.plot(x, pf[2][j], 'bo-', label = "custom", alpha=.5)
    plt.legend(loc="upper left")
    
for j in range(0, 3):
    plt.subplot(2, 3, count)
    count += 1
    plt.xlabel('Frames')
    plt.ylabel('Disk Writes')
    plt.plot(x, dw[0][j], 'ro-', label = "fifi", alpha=.5, linewidth=4.5)
    plt.plot(x, dw[1][j], 'go-', label = "rand")
    plt.plot(x, dw[2][j], 'bo-', label = "custom", alpha=.5)
    plt.legend(loc="upper left")

fig.savefig('Compare_algorithms.png')