import matplotlib.pyplot as plt
import numpy as np

def parse_single_result(file_path):
    thread_counts = []
    means = []
    std_devs = []
    with open(file_path, 'r') as file:
        for line in file:
            parts = line.strip().split('|')
            thread_count = int(parts[-3].split(':')[1].strip())
            mean = float(parts[-2].split(':')[1].strip())
            std_dev = float(parts[-1].split(':')[1].strip())
            thread_counts.append(thread_count)
            means.append(mean)
            std_devs.append(std_dev)
    return thread_counts, means, std_devs

# Example of how to call this function:
# serial_thread_counts, serial_means, serial_std_devs = parse_single_result('/path/to/serial_results.txt')


# File paths
serial_path = '../results/serial_results.txt'
mutex_path ='../results/mutex_results.txt'
rw_lock_path = '../results/rw_lock_results.txt'

# Read the data
serial_thread_counts, serial_means, serial_std_devs = parse_single_result(serial_path)
mutex_thread_counts, mutex_means, mutex_std_devs = parse_single_result(mutex_path)
rw_lock_thread_counts, rw_lock_means, rw_lock_std_devs = parse_single_result(rw_lock_path)


# Assuming thread counts are the same across files; if not, you will need to handle this
x = np.arange(len(mutex_thread_counts))  # the label locations
width = 0.25  # the width of the bars

fig, ax = plt.subplots()
rects1 = ax.bar(x - width, serial_means, width, label='Serial', yerr=serial_std_devs, capsize=5)
rects2 = ax.bar(x, mutex_means, width, label='Mutex', yerr=mutex_std_devs, capsize=5)
rects3 = ax.bar(x + width, rw_lock_means, width, label='ReadWriteLock', yerr=rw_lock_std_devs, capsize=5)

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_xlabel('Thread Count')
ax.set_ylabel('Mean Time (ms)')
ax.set_title('Performance by Implementation and Thread Count')
ax.set_xticks(x)
ax.set_xticklabels(serial_thread_counts)
ax.legend()
ax.set_ylim(0, max(serial_means + mutex_means + rw_lock_means) * 1.1)  # Adjust y-axis scale dynamically


fig.tight_layout()

plt.show()
