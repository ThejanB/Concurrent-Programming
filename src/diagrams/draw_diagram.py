import matplotlib.pyplot as plt
import numpy as np

def parse_and_filter_results(serial_path, mutex_path, rw_lock_path, filters):
    data = {f'{k}': {'Serial': [(2, 0, 0), (4, 0, 0), (8, 0, 0)], 'Mutex': [], 'ReadWriteLock': []} for k in filters.keys()}
    
    # Parse a single file and append data to the right category
    def parse_file(file_path, implementation):
        with open(file_path, 'r') as file:
            for line in file:
                print(line)
                parts = line.strip().split('|')
                mMember = parts[2].split(':')[1].strip()
                mInsert = parts[3].split(':')[1].strip()
                mDelete = parts[4].split(':')[1].strip()
                param_key = f'{mMember}_{mInsert}_{mDelete}'
                print(param_key)

                if param_key in data:
                    thread_count = int(parts[-3].split(':')[1].strip())
                    mean = float(parts[-2].split(':')[1].strip())
                    std_dev = float(parts[-1].split(':')[1].strip())
                    data[param_key][implementation].append((thread_count, mean, std_dev))

    parse_file(serial_path, 'Serial')
    parse_file(mutex_path, 'Mutex')
    parse_file(rw_lock_path, 'ReadWriteLock')
    print(data)
    return data

def plot_data(data, param_key, title):
    x = np.arange(4)  # Thread counts: 1, 2, 4, 8
    width = 0.25  # Bar width

    fig, ax = plt.subplots()

    for i, imp in enumerate(['Serial', 'Mutex', 'ReadWriteLock']):
        if data[param_key][imp]:  # Check if there is data for the implementation
            tc_sorted = sorted(data[param_key][imp])  # Sort by thread count
            means = [d[1] for d in tc_sorted]
            std_devs = [d[2] for d in tc_sorted]
            ax.bar(x + i * width, means, width, label=imp, yerr=std_devs, capsize=5)

    ax.set_xlabel('Thread Count')
    ax.set_ylabel('Mean Time (ms)')
    ax.set_title(title)
    ax.set_xticks(x + width)
    ax.set_xticklabels([1, 2, 4, 8])
    ax.legend()

    fig.tight_layout()
    plt.show()

mMember = float(input("Enter mMember value: "))
mInsert = float(input("Enter mInsert value: "))
mDelete = float(input("Enter mDelete value: "))

mMember_str = f"{mMember:.6f}%"
mInsert_str = f"{mInsert:.6f}%"
mDelete_str = f"{mDelete:.6f}%"

filters ={
    f"{mMember_str}_{mInsert_str}_{mDelete_str}" : {}
}

# File paths (adjust these to your actual files)
serial_path = '../results/serial_results.txt'
mutex_path = '../results/mutex_results.txt'
rw_lock_path = '../results/rw_lock_results.txt'

all_data = parse_and_filter_results(serial_path, mutex_path, rw_lock_path, filters)

for key, title in zip(filters.keys(), [
    'mMember = 0.99, mInsert = 0.005, mDelete = 0.005',
    'mMember = 0.90, mInsert = 0.05, mDelete = 0.05',
    'mMember = 0.50, mInsert = 0.25, mDelete = 0.25']):
    plot_data(all_data, key, title)
