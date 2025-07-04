import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("intercept_counts.csv", header=None, names=["Zone", "Count"])

plt.bar(df["Zone"], df["Count"], color="skyblue")
plt.xlabel("Radar Zone (MPI Process ID)")
plt.ylabel("Number of Interceptions")
plt.title("Interceptions per Radar Zone")
plt.grid(axis="y")
plt.tight_layout()
plt.savefig("interception_heatmap.png")
plt.show()
