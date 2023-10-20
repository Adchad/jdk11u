import pandas as pd
import matplotlib.pyplot as plt


f = pd.read_csv("alloc_occurences")

f.hist()
plt.show()
