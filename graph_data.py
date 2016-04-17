import pylab as plt
import operator
import math
import numpy as np
import scipy.stats as stats
from bisect import bisect_left
from scipy.stats import norm

def plot(input_file):

    x_vals = []
    y_vals = []
    name = ""
    line_types = [ '-' , '--' , '-.' , ':']
    i = 0

    fig = plt.figure()
    plt.style.use('bmh')

    with open(input_file) as f:
        content = f.readlines()

    for line in content:
        if "insert" in line:
            pass
            line = line.replace(',', "")
            line = line.split(":")

            size = line[0].split()[-1]
            time = line[1].split()[0]

            x_vals.append(int(size))
            y_vals.append(int(time))
        
        elif "delete" in line:
            pass
            # line = line.replace(',', "")
            # line = line.split(":")

            # size = line[0].split()[-1]
            # time = line[1].split()[0]

            # x_vals.append(int(size))
            # y_vals.append(int(time))
        
        elif len(line) > 1:
            if len(x_vals) == 0:
                name = line.strip()
            else:
                if i < 3:
                    plt.plot(x_vals, y_vals, line_types[i], label=name)
                else:
                    plt.plot(x_vals, y_vals, marker="o", label=name)
                name = line.strip()
                x_vals = []
                y_vals = []
                i += 1

    plt.plot(x_vals, y_vals, marker='.', label=name)
    plt.legend(loc='upper left')
    # plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0.)

    

    # fig.suptitle('Average Delete Time', fontsize=13)
    fig.suptitle('Average Insert Time', fontsize=13)
    
    plt.ylabel('Average Time (ns)', fontsize=12)
    plt.xlabel('Input Size', fontsize=12)
    
    # plt.plot(x_vals, y_vals) # including h here is crucial
    plt.show()

if __name__ == "__main__":
    plot("./data.txt")
